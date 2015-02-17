/*
 * \brief  Programmable interrupt controller for core
 * \author Norman Feske
 * \date   2013-04-05
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _PIC_H_
#define _PIC_H_

/* Genode includes */
#include <util/mmio.h>

/* core includes */
#include <board.h>

namespace Genode
{
	/**
	 * Programmable interrupt controller for core
	 */
	class Pic;
}

class Genode::Pic : Mmio
{
	public:

		enum { NR_OF_IRQ = 64 };

	private:

		struct Irq_pending_basic : Register<0x0, 32>
		{
			struct Timer : Bitfield<0, 1> { };
			struct Gpu   : Bitfield<8, 2> { };
		};

		struct Irq_pending_gpu_1  : Register<0x04, 32> { };
		struct Irq_pending_gpu_2  : Register<0x08, 32> { };
		struct Irq_enable_gpu_1   : Register<0x10, 32> { };
		struct Irq_enable_gpu_2   : Register<0x14, 32> { };
		struct Irq_enable_basic   : Register<0x18, 32> { };
		struct Irq_disable_gpu_1  : Register<0x1c, 32> { };
		struct Irq_disable_gpu_2  : Register<0x20, 32> { };
		struct Irq_disable_basic  : Register<0x24, 32> { };

		/**
		 * Return true if specified interrupt is pending
		 */
		static bool _is_pending(unsigned i, uint32_t p1, uint32_t p2)
		{
			return i < 32 ? (p1 & (1 << i)) : (p2 & (1 << (i - 32)));
		}

	public:

		/**
		 * Constructor
		 */
		Pic() : Mmio(Board::IRQ_CONTROLLER_BASE) { mask(); }

		void init_processor_local() { }

		bool take_request(unsigned &irq)
		{
			/* read basic IRQ status mask */
			uint32_t const p = read<Irq_pending_basic>();


			/* read GPU IRQ status mask */
			uint32_t const p1 = read<Irq_pending_gpu_1>(),
			               p2 = read<Irq_pending_gpu_2>();

			if (Irq_pending_basic::Timer::get(p)) {
				irq = Irq_pending_basic::Timer::SHIFT;
				return true;
			}

			/* search for lowest set bit in pending masks */
			for (unsigned i = 0; i < NR_OF_IRQ; i++) {
				if (!_is_pending(i, p1, p2))
					continue;

				irq = Board_base::GPU_IRQ_BASE + i;
				return true;
			}

			return false;
		}

		void finish_request() { }

		void mask()
		{
			write<Irq_disable_basic>(~0);
			write<Irq_disable_gpu_1>(~0);
			write<Irq_disable_gpu_2>(~0);
		}

		void unmask(unsigned const i, unsigned)
		{
			if (i < 8)
				write<Irq_enable_basic>(1 << i);
			else if (i < 32 + 8)
				write<Irq_enable_gpu_1>(1 << (i - 8));
			else
				write<Irq_enable_gpu_2>(1 << (i - 8 - 32));
		}

		void mask(unsigned const i)
		{
			if (i < 8)
				write<Irq_disable_basic>(1 << i);
			else if (i < 32 + 8)
				write<Irq_disable_gpu_1>(1 << (i - 8));
			else
				write<Irq_disable_gpu_2>(1 << (i - 8 - 32));
		}

		/**
		 * Wether an interrupt is inter-processor interrupt of a processor
		 *
		 * \param interrupt_id  kernel name of the interrupt
		 * \param processor_id  kernel name of the processor
		 */
		bool is_ip_interrupt(unsigned const interrupt_id,
		                     unsigned const processor_id)
		{
			return false;
		}

		/**
		 * Trigger the inter-processor interrupt of a processor
		 *
		 * \param processor_id  kernel name of the processor
		 */
		void trigger_ip_interrupt(unsigned const processor_id) { }
};

namespace Kernel { class Pic : public Genode::Pic { }; }

#endif /* _PIC_H_ */
