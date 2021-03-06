/*
 * \brief  Registry of known guest-physical memory regions
 * \author Norman Feske
 * \date   2013-09-02
 *
 * Contains the mapping of guest-phyiscal to VMM-local addresses.
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is distributed under the terms of the GNU General Public License
 * version 2.
 */

#ifndef _GUEST_MEMORY_H_
#define _GUEST_MEMORY_H_

/*
 * Work-around for a naming conflict between the enum definition of PAGE_SIZE
 * in 'os/attached_ram_dataspace.h' and the VirtualBox #define with the same
 * name.
 */
#define BACKUP_PAGESIZE PAGE_SIZE
#undef  PAGE_SIZE

/* Genode includes */
#include <base/printf.h>
#include <base/env.h>
#include <base/lock.h>
#include <base/flex_iterator.h>
#include <util/list.h>
#include <os/attached_ram_dataspace.h>

/* VirtualBox includes */
#include <VBox/vmm/pgm.h>
#include <VBox/vmm/iom.h>
#include <VBox/vmm/pdmdev.h>

#define PAGE_SIZE BACKUP_PAGESIZE


class Guest_memory
{
	struct Region;

	/*
	 * XXX Use AVL tree instead of a linked list
	 */

	typedef Genode::List<Region> Region_list;
	typedef Genode::Lock         Lock;
	typedef Genode::addr_t       addr_t;

	private:

		struct Region : Region_list::Element
		{
			RTGCPHYS const _GCPhys;    /* guest-physical address */
			RTGCPHYS const _cb;        /* size */
			void   * const _pv;        /* VMM-local address */

			/*
			 * MMIO-specific members
			 */
			PPDMDEVINS      const _pDevIns;
			RTHCPTR         const _pvUser;
			PFNIOMMMIOWRITE const _pfnWriteCallback;
			PFNIOMMMIOREAD  const _pfnReadCallback;
			PFNIOMMMIOFILL  const _pfnFillCallback;
			uint32_t        const _fFlags;

			Region(RTGCPHYS const GCPhys, RTGCPHYS const cb, void *const pv,
			       PPDMDEVINS          pDevIns,
			       RTHCPTR             pvUser,
			       PFNIOMMMIOWRITE     pfnWriteCallback,
			       PFNIOMMMIOREAD      pfnReadCallback,
			       PFNIOMMMIOFILL      pfnFillCallback,
			       uint32_t            fFlags)
			:
				_GCPhys(GCPhys), _cb(cb), _pv(pv),
				_pDevIns          (pDevIns),
				_pvUser           (pvUser),
				_pfnWriteCallback (pfnWriteCallback),
				_pfnReadCallback  (pfnReadCallback),
				_pfnFillCallback  (pfnFillCallback),
				_fFlags           (fFlags)
			{ }

			/**
			 * Return true if region contains specified guest-physical area
			 */
			bool contains(RTGCPHYS GCPhys, size_t size) const
			{
				return (_GCPhys <= GCPhys) && (GCPhys < _GCPhys + _cb) &&
				       (_GCPhys + _cb - GCPhys >= size);
			}

			/**
			 * Return true if region is disjunct to specified guest-physical area
			 */
			bool disjunct(RTGCPHYS GCPhys, size_t size) const
			{
				return (GCPhys + size - 1 < _GCPhys) ||
				       (_GCPhys + _cb - 1 < GCPhys);
			}

			/**
			 * Return guest-physical base address
			 */
			RTGCPHYS GCPhys() const { return _GCPhys; }

			/**
			 * Return VMM-local base address
			 */
			void *pv() const { return _pv; }

			void dump() const
			{
				Genode::printf("phys [0x%16lx-0x%16lx] -> virt [0x%16lx-0x%16lx] (dev='%s')\n",
				               (long)_GCPhys, (long)_GCPhys + (long)_cb - 1,
				               (long)_pv,     (long)_pv     + (long)_cb - 1,
				               _pDevIns && _pDevIns->pReg ? _pDevIns->pReg->szName : 0);
			}

			void *pv_at_offset(addr_t offset)
			{
				if (_pv)
					return (void *)((addr_t)_pv + (addr_t)offset);
				return 0;
			}

			int mmio_write(RTGCPHYS GCPhys, void const *pv, unsigned cb)
			{
				if (!_pfnWriteCallback)
					return VERR_IOM_MMIO_RANGE_NOT_FOUND;

//				PDBG("mmio_write(GCPhys=0x%lx, cb=%u)", GCPhys, cb);

				return _pfnWriteCallback(_pDevIns, _pvUser, GCPhys, pv, cb);
			}

			int mmio_read(RTGCPHYS GCPhys, void *pv, unsigned cb)
			{
				if (!_pfnReadCallback)
					return VERR_IOM_MMIO_RANGE_NOT_FOUND;

//				PDBG("mmio_read(GCPhys=0x%lx, cb=%u)", GCPhys, cb);

				return _pfnReadCallback(_pDevIns, _pvUser, GCPhys, pv, cb);
			}
		};

		Lock        _lock;
		Region_list _ram_regions;
		Region_list _rom_regions;
		Region_list _mmio_regions;

		static Region *_lookup(RTGCPHYS GCPhys, Region_list &regions, size_t size)
		{
			using Genode::addr_t;

			for (Region *r = regions.first(); r; r = r->next())
				if (r->contains(GCPhys, size))
					return r;

			return 0;
		}

		static bool _overlap(RTGCPHYS GCPhys, size_t size, 
		                     Region_list &regions)
		{
			using Genode::addr_t;

			for (Region *r = regions.first(); r; r = r->next())
			{
				if (r->disjunct(GCPhys, size))
					continue;

				return true;
			}

			return false;
		}

		/**
		 * \return  looked-up region, or 0 if lookup failed
		 */
		Region *_lookup(RTGCPHYS GCPhys, size_t size)
		{
			using Genode::addr_t;

			/*
			 * ROM regions may alias RAM regions. For the lookup, always
			 * consider ROM regions first.
			 */

			if (Region *r = _lookup(GCPhys, _rom_regions, size))
				return r;

			if (Region *r = _lookup(GCPhys, _mmio_regions, size))
				return r;

			if (Region *r = _lookup(GCPhys, _ram_regions, size))
				return r;

			return 0;
		}

	public:

		class Region_conflict { };

		/**
		 * \throw Region_conflict
		 */
		void add_ram_mapping(RTGCPHYS const GCPhys, RTGCPHYS const cb, void * const pv)
		{
			/*
			 * XXX check for overlapping regions
			 */
			_ram_regions.insert(new (Genode::env()->heap())
			                        Region(GCPhys, cb, pv, 0, 0, 0, 0, 0, 0));
		}

		/**
		 * \throw Region_conflict
		 */
		void add_rom_mapping(RTGCPHYS const GCPhys, RTGCPHYS const cb,
		                     void const * const pv, PPDMDEVINS      pDevIns)
		{
			/*
			 * XXX check for overlapping regions
			 */
			_rom_regions.insert(new (Genode::env()->heap())
			                   Region(GCPhys, cb,
			                   (void *)pv, pDevIns, 0, 0, 0, 0, 0));
		}

		/**
		 * \throw Region_conflict
		 */
		void add_mmio_mapping(RTGCPHYS const GCPhys, RTGCPHYS const cb,
		                      PPDMDEVINS      pDevIns,
		                      RTHCPTR         pvUser,
		                      PFNIOMMMIOWRITE pfnWriteCallback,
		                      PFNIOMMMIOREAD  pfnReadCallback,
		                      PFNIOMMMIOFILL  pfnFillCallback,
		                      uint32_t        fFlags)
		{
			/*
			 * XXX check for overlapping regions
			 */
			_mmio_regions.insert(new (Genode::env()->heap())
			                     Region(GCPhys, cb, 0,
			                            pDevIns, pvUser, pfnWriteCallback,
			                            pfnReadCallback, pfnFillCallback, fFlags));
		}


		bool remove_mmio_mapping(RTGCPHYS const GCPhys, RTGCPHYS const size)
		{
			Region *r = _lookup(GCPhys, _mmio_regions, size);
			if (!r)
				return false;

			_mmio_regions.remove(r);
			delete r;
			return true;
		}


		void dump() const
		{
			Genode::printf("guest-physical to VMM-local RAM mappings:\n");
			for (Region const *r = _ram_regions.first(); r; r = r->next())
				r->dump();

			Genode::printf("guest-physical to VMM-local ROM mappings:\n");
			for (Region const *r = _rom_regions.first(); r; r = r->next())
				r->dump();

			Genode::printf("guest-physical MMIO regions:\n");
			for (Region const *r = _mmio_regions.first(); r; r = r->next())
				r->dump();
		}

		/**
		 * \return  looked-up VMM-local address, or 0 if lookup failed
		 */
		void *lookup(RTGCPHYS GCPhys, size_t size)
		{
			Region *r = _lookup(GCPhys, size);
			if (!r)
				return 0;

			return r->pv_at_offset(GCPhys - r->GCPhys());
		}

		/**
		 * \return looked-up VMM-local address if Guest address is RAM
		 */
		void *lookup_ram(RTGCPHYS const GCPhys, size_t size,
		                 Genode::Flexpage_iterator &it)
		{
			if (_overlap(GCPhys, size, _rom_regions))
				return 0;

			if (_overlap(GCPhys, size, _mmio_regions))
				return 0;

			if (!_overlap(GCPhys, size, _ram_regions))
				return 0;

			Region *r = _lookup(GCPhys, _ram_regions, size);
			if (!r)
				return 0;

			void * vmm_local = lookup_ram(GCPhys & ~(size * 2UL - 1), size * 2UL, it);
			if (vmm_local)
				return vmm_local;

			it = Genode::Flexpage_iterator((addr_t)r->pv_at_offset(GCPhys - r->GCPhys()), size, GCPhys, size, GCPhys - r->GCPhys());

			return r->pv_at_offset(GCPhys - r->GCPhys());
		}

		/**
		 * \return VirtualBox return code
		 */
		int mmio_write(PVM pVM, RTGCPHYS GCPhys, uint32_t u32Value, size_t cbValue)
		{
			Region *r = _lookup(GCPhys, cbValue);

			if (!r) {
				PERR("Guest_memory::mmio_write: lookup failed");
				PERR("GCPhys=0x%x, u32Value=0x%x, cbValue=%zd",
				     GCPhys, u32Value, cbValue);
				return VERR_IOM_MMIO_RANGE_NOT_FOUND;
			}

			return r->mmio_write(GCPhys, &u32Value, cbValue);
		}

		/**
		 * \return VirtualBox return code
		 */
		int mmio_read(PVM pVM, RTGCPHYS GCPhys, uint32_t *u32Value, size_t cbValue)
		{
			Region *r = _lookup(GCPhys, cbValue);

			if (!r) {
				PERR("Guest_memory::mmio_read: lookup failed");
				PERR("GCPhys=0x%x, u32Value=0x%x, cbValue=%zd",
				     GCPhys, u32Value, cbValue);
				return VERR_IOM_MMIO_RANGE_NOT_FOUND;
			}

			return r->mmio_read(GCPhys, u32Value, cbValue);
		}
};


/**
 * Return pointer to singleton instance
 */
Guest_memory *guest_memory();


#endif /* _GUEST_MEMORY_H_ */
