/*
 * \brief  libc file handling function emulation (open/read/write/mmap)
 * \author Sebastian Sumpf
 * \date   2009-10-26
 */

/*
 * Copyright (C) 2009-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */
#include <base/allocator_avl.h>
#include <base/printf.h>
#include <ldso/arch.h>
#include <rom_session/connection.h>
#include <rm_session/connection.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <util/list.h>
#include <util/construct_at.h>

#include "file.h"

extern int debug;

namespace Genode {

	/**
	 * Managed dataspace for ELF files (singelton)
	 */
	class Rm_area : public Rm_connection
	{
		private:

			/* size of dataspace */
			enum { RESERVATION = 160 * 1024 * 1024 };

			addr_t        _base;  /* base address of dataspace */
			Allocator_avl _range; /* VM range allocator */

		protected:

			Rm_area(addr_t base)
			: Rm_connection(0, RESERVATION), _range(env()->heap())
			{
				on_destruction(KEEP_OPEN);

				_base = (addr_t) env()->rm_session()->attach_at(dataspace(), base);
				_range.add_range(base, RESERVATION);
			}

		public:

			static Rm_area *r(addr_t base = 0)
			{
				/*
				 * The capabilities in this class become invalid when doing a
				 * fork in the noux environment. Hence avoid destruction of
				 * the singleton object as the destructor would try to access
				 * the capabilities also in the forked process.
				 */
				static bool constructed = 0;
				static char placeholder[sizeof(Rm_area)];
				if (!constructed) {
					construct_at<Rm_area>(placeholder, base);
					constructed = 1;
				}
				return reinterpret_cast<Rm_area *>(placeholder);
			}

			/**
			 * Reserve VM region of 'size' at 'vaddr'. Allocate any free region if
			 * 'vaddr' is zero
			 */
			addr_t alloc_region(size_t size, addr_t vaddr = 0)
			{
				addr_t addr = vaddr;

				if (addr && (_range.alloc_addr(size, addr).is_error()))
					throw Region_conflict();
				else if (!addr && _range.alloc_aligned(size, (void **)&addr, 12).is_error())
					throw Region_conflict();

				return addr;
			}

			void free_region(addr_t vaddr) { _range.free((void *)vaddr); }

			/**
			 * Overwritten from 'Rm_connection'
			 */
			Local_addr attach_at(Dataspace_capability ds, addr_t local_addr,
			                     size_t size = 0, off_t offset = 0) {
				return Rm_connection::attach_at(ds, local_addr - _base, size, offset); }

			/**
			 * Overwritten from 'Rm_connection'
			 */
			Local_addr attach_executable(Dataspace_capability ds, addr_t local_addr,
			                             size_t size = 0, off_t offset = 0) {
				return Rm_connection::attach_executable(ds, local_addr - _base, size, offset); }

			void detach(Local_addr local_addr) {
				Rm_connection::detach((addr_t)local_addr - _base); }
	};


	class Fd_handle : public List<Fd_handle>::Element
	{
		private:

			addr_t                   _phdr;
			addr_t                   _vaddr;  /* image start */
			addr_t                   _daddr;  /* data start */
			Rom_dataspace_capability _ds_rom; /* image ds */
			Ram_dataspace_capability _ds_ram; /* data ds */
			int                      _fd;     /* file handle */
			Session_capability       _rom_cap;

		public:

			Fd_handle(int fd, Rom_dataspace_capability ds_rom, Session_capability rom_cap)
			: _vaddr(~0UL),  _ds_rom(ds_rom), _fd(fd), _rom_cap(rom_cap)
			{
				_phdr = (addr_t)env()->rm_session()->attach(_ds_rom, PAGE_SIZE);
			}

			addr_t                   vaddr()      { return _vaddr; }
			void                     vaddr(addr_t vaddr) { _vaddr = vaddr; }
			Rom_dataspace_capability dataspace()  { return _ds_rom; }
			addr_t                   phdr()       { return _phdr; }

			void setup_data(addr_t vaddr, addr_t vlimit, addr_t flimit, off_t offset)
			{
				/* allocate data segment */
				_ds_ram = env()->ram_session()->alloc(vlimit - vaddr);
				Rm_area::r()->attach_at(_ds_ram, vaddr);

				/* map rom data segment */
				void *rom_data = env()->rm_session()->attach(_ds_rom, 0, offset);
				
				/* copy data */
				memcpy((void *)vaddr, rom_data, flimit - vaddr);
				env()->rm_session()->detach(rom_data);

				/* set parent cap (arch.lib.a) */
				set_parent_cap_arch((void *)vaddr);

				_daddr = vaddr;
			}

			void setup_text(addr_t vaddr, size_t size, off_t offset)
			{
				_vaddr = vaddr;
				Rm_area::r()->attach_executable(_ds_rom, vaddr, size, offset);
			}

			addr_t alloc_region(addr_t vaddr, addr_t vlimit)
			{
				Rm_area *a = Rm_area::r(vaddr);
				return a->alloc_region(vlimit - vaddr, vaddr);
			}

			static List<Fd_handle> *file_list()
			{
				static List<Fd_handle> _file_list;
				return &_file_list;
			}

			static Fd_handle *find_handle(int fd, addr_t vaddr = 0)
			{
				Fd_handle *h = file_list()->first();

				while (h) {

					if (vaddr && vaddr == h->_vaddr)
						return h;
					if (h->_fd == fd)
						return h;
					h = h->next();
				}

				return 0;
			}

			static void free(void *addr)
			{
				addr_t vaddr = (addr_t) addr;

				Fd_handle *h = file_list()->first();

				while (h) {

					if (h->_vaddr != vaddr) {
						h = h->next();
						continue;
					}

					destroy(env()->heap(), h);
					return;
				}
			}
	
			~Fd_handle()
			{
				file_list()->remove(this);

				if (_vaddr != ~0UL) {
					Rm_area::r()->detach(_vaddr);
					Rm_area::r()->detach(_daddr);
					Rm_area::r()->free_region(_vaddr);
					env()->ram_session()->free(_ds_ram);
					env()->rm_session()->detach(_phdr);
				}

				if (_rom_cap.valid())
					env()->parent()->close(_rom_cap);
			}
	};
}


extern "C" int open(const char *pathname, int flags)
{
	using namespace Genode;
	static int fd = -1;
	static int i = 0;
	i++;

	/* skip directory part from pathname, leaving only the plain filename */
	const char *filename = pathname;
	for (; *pathname; pathname++)
		if (*pathname == '/')
			filename = pathname + 1;

	try {
		/* open the file dataspace and attach it */
		Rom_connection rom(filename);
		rom.on_destruction(Rom_connection::KEEP_OPEN);

		Fd_handle::file_list()->insert(new(env()->heap())
		                               Fd_handle(++fd, rom.dataspace(), rom.cap()));

		Fd_handle *h;
		if (!(h = Fd_handle::find_handle(fd))) {
			PERR("Could not open %s\n", filename);
			return -1;
		}
	} catch (...) {
		PERR("Rom connection failed for %s", filename);
		return -1;
	}

	return fd;
}


extern "C" void *file_phdr(const char *pathname, void *vaddr)
{
	using namespace Genode;

	Fd_handle *h = 0;
	if (!(h = Fd_handle::find_handle(-1, (addr_t)vaddr))) {
		int fd = open(pathname, 0);
		h = Fd_handle::find_handle(fd);
		h->vaddr((addr_t)vaddr);
	}

	return (void *)h->phdr();
}


extern "C" int find_binary_name(int fd, char *buf, size_t buf_size)
{
	using namespace Genode;

	Fd_handle *h;
	if (!(h = Fd_handle::find_handle(fd))) {
		PERR("handle not found\n");
		return -1;
	}

	return binary_name(h->dataspace(), buf, buf_size);
}


extern "C" ssize_t read(int fd, void *buf, size_t count)
{
	using namespace Genode;

	Fd_handle *h;

	if (!(h = Fd_handle::find_handle(fd))) {
		PERR("handle not found\n");
		return -1;
	}
	
	try {
		void *base = env()->rm_session()->attach(h->dataspace(), count);
		memcpy(buf, base, count);
		env()->rm_session()->detach(base);
	}
	catch (...) {
		return -1;
	}

	return count;
}


extern "C" ssize_t write(int fd, const void *buf, size_t count) 
{
	Genode::printf("%p", buf);
	return count;
}


extern "C" int munmap(void *addr, size_t /* length */) 
{
	using namespace Genode;
	Fd_handle::free(addr);
	return 0;
}


extern "C" void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	using namespace Genode;

	if(!(flags & MAP_ANON)) {
		PERR("No MAP_ANON");
		return MAP_FAILED;
	}

	/* called during ldso relocation */
	if (flags & MAP_LDSO) {
		enum { MEM_SIZE = 128 * 1024 };
		static char _mem[MEM_SIZE];

		/* generate fault on allocation */
		if (length > MEM_SIZE) {
			int *fault = (int *)0xa110ce88;
			*fault = 1;
		}
		return _mem;
	}
	/* memory allocation */
	else {
		void *base;

		try {
			Ram_dataspace_capability ds_cap = env()->ram_session()->alloc(round_page(length));
			base = env()->rm_session()->attach(ds_cap, length, 0,
			                                   (addr) ? true : false, (addr_t)addr);
		}
		catch(...) {
			PERR("Anonmymous mmap failed\n");
			return MAP_FAILED;
		}

		if (debug)
			PDBG("base %p", base);

		return base;
	}

	return MAP_FAILED;
}


extern "C" void *genode_map(int fd, Elf_Phdr **segs)
{
	using namespace Genode;

	if (1 > 1) {
		PERR("More than two segments in ELF");
		return MAP_FAILED;
	}

	Fd_handle *h;
	if (!(h = Fd_handle::find_handle(fd))) {
		PERR("handle not found\n");
		return MAP_FAILED;
	}

	addr_t base_vaddr  = trunc_page(segs[0]->p_vaddr);
	addr_t base_offset = trunc_page(segs[0]->p_offset);
	addr_t base_msize  = round_page(segs[1]->p_vaddr - base_vaddr);
	addr_t base_vlimit = round_page(segs[1]->p_vaddr + segs[1]->p_memsz);
	/* is this a fixed address */
	bool   fixed       = base_vaddr ? true : false;

	try {
		base_vaddr = h->alloc_region(base_vaddr, base_vlimit);
	} catch (...) {
		PERR("Region allocation failed: %lx-%lx", base_vaddr, base_vlimit);
		return MAP_FAILED;
	}

	/* map text segment */
	h->setup_text(base_vaddr, base_msize, base_offset);

	addr_t offset      = fixed ? 0 : base_vaddr;
	base_vlimit       += offset;
	addr_t base_flimit = offset + segs[1]->p_vaddr + segs[1]->p_filesz;
	base_vaddr         = offset + trunc_page(segs[1]->p_vaddr);
	base_offset        = trunc_page(segs[1]->p_offset);
	
	/* copy data segment */
	h->setup_data(base_vaddr, base_vlimit, base_flimit, base_offset);

	return (void *)h->vaddr();
}

