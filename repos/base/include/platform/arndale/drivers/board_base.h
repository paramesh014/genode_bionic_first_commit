/*
 * \brief  Driver base for the InSignal Arndale 5 board
 * \author Martin stein
 * \date   2013-01-09
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__DRIVERS__BOARD_BASE_H_
#define _INCLUDE__DRIVERS__BOARD_BASE_H_

/* Genode includes */
#include <platform_exynos5/board_base.h>

namespace Genode
{
	/**
	 * Board driver base
	 */
	struct Board_base : Exynos5
	{
		enum
		{
			/* clock management unit */
			CMU_MMIO_BASE = 0x10010000,
			CMU_MMIO_SIZE = 0x24000,

			/* power management unit */
			PMU_MMIO_BASE = 0x10040000,
			PMU_MMIO_SIZE = 0x5000,

			/* interrupt controller */
			GIC_CPU_MMIO_BASE = 0x10480000,
			GIC_CPU_MMIO_SIZE = 0x00010000,

			/* UART */
			UART_2_MMIO_BASE = 0x12C20000,
			UART_2_CLOCK     = 100000000,
			UART_2_IRQ       = 85,

			/* USB */
			USB_HOST20_IRQ = 103,
			USB_DRD30_IRQ  = 104,

			/* SATA/AHCI */
			SATA_IRQ = 147,

			/* I2C */
			I2C_HDMI_IRQ = 96,

			/* SD card */
			SDMMC0_IRQ = 107,

			/* wether board provides security extension */
			SECURITY_EXTENSION = 1,

			/* IRAM */
			IRAM_BASE = 0x02020000,

			/* hardware name of the primary processor */
			PRIMARY_MPIDR_AFF_0 = 0,
		};
	};
}

#endif /* _INCLUDE__DRIVERS__BOARD_BASE_H_ */
