/*
 * \brief  Singlethreaded minimalistic kernel
 * \author Martin Stein
 * \author Stefan Kalkowski
 * \date   2011-10-20
 *
 * This kernel is the only code except the mode transition PIC, that runs in
 * privileged CPU mode. It has two tasks. First it initializes the process
 * 'core', enriches it with the whole identically mapped address range,
 * joins and applies it, assigns one thread to it with a userdefined
 * entrypoint (the core main thread) and starts this thread in userland.
 * Afterwards it is called each time an exception occurs in userland to do
 * a minimum of appropriate exception handling. Thus it holds a CPU context
 * for itself as for any other thread. But due to the fact that it never
 * relies on prior kernel runs this context only holds some constant pointers
 * such as SP and IP.
 */

/*
 * Copyright (C) 2011-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* core includes */
#include <kernel/pd.h>
#include <kernel/vm.h>
#include <platform_pd.h>
#include <trustzone.h>
#include <timer.h>
#include <pic.h>
#include <map_local.h>

/* base includes */
#include <unmanaged_singleton.h>
#include <base/native_types.h>

/* base-hw includes */
#include <kernel/irq.h>
#include <kernel/perf_counter.h>
using namespace Kernel;

extern Genode::Native_thread_id _main_thread_id;
extern "C" void CORE_MAIN();
extern void * _start_secondary_processors;
extern int _prog_img_beg;
extern int _prog_img_end;

Genode::Native_utcb * _main_thread_utcb;

namespace Kernel
{
	/**
	 * Return interrupt-controller singleton
	 */
	Pic * pic() { return unmanaged_singleton<Pic>(); }

	/* import Genode types */
	typedef Genode::umword_t       umword_t;
	typedef Genode::Core_thread_id Core_thread_id;
}

namespace Kernel
{
	Pd_ids * pd_ids() { return unmanaged_singleton<Pd_ids>(); }
	Thread_ids * thread_ids() { return unmanaged_singleton<Thread_ids>(); }
	Signal_context_ids * signal_context_ids() { return unmanaged_singleton<Signal_context_ids>(); }
	Signal_receiver_ids * signal_receiver_ids() { return unmanaged_singleton<Signal_receiver_ids>(); }

	Pd_pool * pd_pool() { return unmanaged_singleton<Pd_pool>(); }
	Thread_pool * thread_pool() { return unmanaged_singleton<Thread_pool>(); }
	Signal_context_pool * signal_context_pool() { return unmanaged_singleton<Signal_context_pool>(); }
	Signal_receiver_pool * signal_receiver_pool() { return unmanaged_singleton<Signal_receiver_pool>(); }

	/**
	 * Static kernel PD that describes core
	 */
	Pd * core_pd()
	{
		typedef Early_translations_slab      Slab;
		typedef Early_translations_allocator Allocator;
		typedef Genode::Translation_table    Table;

		constexpr addr_t table_align = 1 << Table::ALIGNM_LOG2;

		struct Core_pd : Platform_pd, Pd
		{
			/**
			 * Establish initial one-to-one mappings for core/kernel.
			 * This function avoids to take the core-pd's translation table
			 * lock in contrast to normal translation insertions to
			 * circumvent strex/ldrex problems in early bootstrap code
			 * on some ARM SoCs.
			 *
			 * \param start   physical/virtual start address of area
			 * \param end     physical/virtual end address of area
			 * \param io_mem  true if it should be marked as device memory
			 */
			void map(addr_t start, addr_t end, bool io_mem)
			{
				using namespace Genode;

				Translation_table *tt = Platform_pd::translation_table();
				const Page_flags flags =
					Page_flags::apply_mapping(true, io_mem ? UNCACHED : CACHED,
					                          io_mem);

				start = trunc_page(start);
				size_t size  = round_page(end) - start;

				try {
					tt->insert_translation(start, start, size, flags, page_slab());
				} catch(Page_slab::Out_of_slabs) {
					PERR("Not enough page slabs");
				} catch(Allocator::Out_of_memory) {
					PERR("Translation table needs to much RAM");
				} catch(...) {
					PERR("Invalid mapping %p -> %p (%zx)", (void*)start,
					     (void*)start, size);
				}
			}

			/**
			 * Constructor
			 */
			Core_pd(Table * const table, Slab * const slab)
			: Platform_pd(table, slab), Pd(table, this)
			{
				using namespace Genode;

				Platform_pd::_id = Pd::id();

				/* map exception vector for core */
				Kernel::mtc()->map(table, slab);

				/* map core's program image */
				map((addr_t)&_prog_img_beg, (addr_t)&_prog_img_end, false);

				/* map core's mmio regions */
				Native_region * r = Platform::_core_only_mmio_regions(0);
				for (unsigned i = 0; r;
				     r = Platform::_core_only_mmio_regions(++i))
					map(r->base, r->base + r->size, true);
			}
		};

		Allocator * const alloc = unmanaged_singleton<Allocator>();
		Table     * const table = unmanaged_singleton<Table, table_align>();
		Slab      * const slab  = unmanaged_singleton<Slab, Slab::ALIGN>(alloc);
		return unmanaged_singleton<Core_pd>(table, slab);
	}

	/**
	 * Return wether an interrupt is private to the kernel
	 *
	 * \param interrupt_id  kernel name of the targeted interrupt
	 */
	bool private_interrupt(unsigned const interrupt_id)
	{
		bool ret = 0;
		for (unsigned i = 0; i < PROCESSORS; i++) {
			ret |= interrupt_id == Timer::interrupt_id(i);
		}
		return ret;
	}
}


namespace Kernel
{
	/**
	 * Get attributes of the mode transition region in every PD
	 */
	addr_t mode_transition_base() { return mtc()->VIRT_BASE; }
	size_t mode_transition_size() { return mtc()->SIZE; }

	/**
	 * Get attributes of the kernel objects
	 */
	size_t thread_size()          { return sizeof(Thread); }
	size_t signal_context_size()  { return sizeof(Signal_context); }
	size_t signal_receiver_size() { return sizeof(Signal_receiver); }
	size_t vm_size()              { return sizeof(Vm); }
	unsigned pd_alignm_log2() { return Genode::Translation_table::ALIGNM_LOG2; }
	size_t pd_size() { return sizeof(Genode::Translation_table) + sizeof(Pd); }

	enum { STACK_SIZE = 64 * 1024 };

	/**
	 * Return lock that guards all kernel data against concurrent access
	 */
	Lock & data_lock()
	{
		static Lock s;
		return s;
	}

	addr_t   core_tt_base;
	unsigned core_pd_id;
}


/**
 * Enable kernel-entry assembly to get an exclusive stack at every processor
 */
unsigned kernel_stack_size = Kernel::STACK_SIZE;
char     kernel_stack[PROCESSORS][Kernel::STACK_SIZE]
         __attribute__((aligned(16)));


/**
 * Setup kernel enviroment before activating secondary processors
 */
extern "C" void init_kernel_uniprocessor()
{
	/************************************************************************
	 ** As atomic operations are broken in physical mode on some platforms **
	 ** we must avoid the use of 'cmpxchg' by now (includes not using any  **
	 ** local static objects.                                              **
	 ************************************************************************/

	/* calculate in advance as needed later when data writes aren't allowed */
	core_tt_base = (addr_t) core_pd()->translation_table();
	core_pd_id   = core_pd()->id();

	/* initialize all processor objects */
	processor_pool();

	/* go multiprocessor mode */
	Processor::start_secondary_processors(&_start_secondary_processors);
}

/**
 * Setup kernel enviroment after activating secondary processors
 */
extern "C" void init_kernel_multiprocessor()
{
	/***********************************************************************
	 ** As updates on a cached kernel lock might not be visible to        **
	 ** processors that have not enabled caches, we can't synchronize the **
	 ** activation of MMU and caches. Hence we must avoid write access to **
	 ** kernel data by now.                                               **
	 ***********************************************************************/

	/* synchronize data view of all processors */
	Processor::invalidate_data_caches();
	Processor::invalidate_instr_caches();
	Processor::data_synchronization_barrier();

	/* initialize processor in physical mode */
	Processor::init_phys_kernel();

	/* switch to core address space */
	Processor::init_virt_kernel(core_tt_base, core_pd_id);

	/************************************
	 ** Now it's safe to use 'cmpxchg' **
	 ************************************/

	Lock::Guard guard(data_lock());

	/*******************************************
	 ** Now it's save to write to kernel data **
	 *******************************************/

	/*
	 * TrustZone initialization code
	 *
	 * FIXME This is a plattform specific feature
	 */
	init_trustzone(pic());

	/*
	 * Enable performance counter
	 *
	 * FIXME This is an optional processor specific feature
	 */
	perf_counter()->enable();

	/* locally initialize processor */
	unsigned const processor_id = Processor::executing_id();
	Processor * const processor = processor_pool()->processor(processor_id);
	processor->init_processor_local();

	/* locally initialize interrupt controller */
	pic()->init_processor_local();
	pic()->unmask(Timer::interrupt_id(processor_id), processor_id);

	/* as primary processor create the core main thread */
	if (Processor::primary_id() == processor_id)
	{
		/* get stack memory that fullfills the constraints for core stacks */
		enum {
			STACK_ALIGNM = 1 << Genode::CORE_STACK_ALIGNM_LOG2,
			STACK_SIZE   = DEFAULT_STACK_SIZE,
		};
		static_assert(STACK_SIZE <= STACK_ALIGNM - sizeof(Core_thread_id),
		              "stack size does not fit stack alignment of core");
		static char s[STACK_SIZE] __attribute__((aligned(STACK_ALIGNM)));

		/* provide thread ident at the aligned base of the stack */
		*(Core_thread_id *)s = 0;

		/* start thread with stack pointer at the top of stack */
		static Native_utcb utcb;
		static Thread t(Priority::MAX, "core");
		_main_thread_id = t.id();
		_main_thread_utcb = &utcb;
		_main_thread_utcb->start_info()->init(t.id(), Genode::Native_capability());
		t.ip = (addr_t)CORE_MAIN;;
		t.sp = (addr_t)s + STACK_SIZE;
		t.init(processor, core_pd(), &utcb, 1);

		/* initialize interrupt objects */
		static Genode::uint8_t _irqs[Pic::NR_OF_IRQ * sizeof(Irq)];
		for (unsigned i = 0; i < Pic::NR_OF_IRQ; i++) {
			if (private_interrupt(i)) { continue; }
			new (&_irqs[i * sizeof(Irq)]) Irq(i);
		}
		/* kernel initialization finished */
		Genode::printf("kernel initialized\n");
	}
}


/**
 * Main routine of every kernel pass
 */
extern "C" void kernel()
{
	/* ensure that no other processor accesses kernel data while we do */
	data_lock().lock();

	/* determine local processor object and let it handle its exception */
	unsigned const processor_id = Processor::executing_id();
	Processor * const processor = processor_pool()->processor(processor_id);
	processor->exception();
}


Kernel::Cpu_context::Cpu_context(Genode::Translation_table * const table)
{
	_init(STACK_SIZE, (addr_t)table);
	sp = (addr_t)kernel_stack;
	ip = (addr_t)kernel;
	core_pd()->admit(this);
}
