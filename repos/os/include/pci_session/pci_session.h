/*
 * \brief  PCI session interface
 * \author Norman Feske
 * \date   2008-01-28
 */

/*
 * Copyright (C) 2008-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__PCI_SESSION__PCI_SESSION_H_
#define _INCLUDE__PCI_SESSION__PCI_SESSION_H_

#include <pci_device/pci_device.h>
#include <session/session.h>
#include <ram_session/ram_session.h>

namespace Pci {

	typedef Genode::Capability<Device> Device_capability;

	struct Session : Genode::Session
	{
		static const char *service_name() { return "PCI"; }

		virtual ~Session() { }

		/**
		 * Find first accessible device
		 */
		virtual Device_capability first_device(unsigned, unsigned) = 0;

		/**
		 * Find next accessible device
		 *
		 * \param prev_device  previous device
		 *
		 * The 'prev_device' argument is used to iterate through all
		 * devices.
		 */
		virtual Device_capability next_device(Device_capability prev_device,
		                                      unsigned, unsigned) = 0; 

		/**
		 * Free server-internal data structures representing the device
		 *
		 * Use this function to relax the heap partition of your PCI session.
		 */
		virtual void release_device(Device_capability device) = 0;

		/**
		 * Provide mapping to device configuration space of 4k, known as
		 * "Enhanced Configuration Access Mechanism (ECAM) for PCI Express
		 */
		virtual Genode::Io_mem_dataspace_capability config_extended(Device_capability) = 0;

		/**
		 * Allocate memory suitable for DMA.
		 */
		virtual Genode::Ram_dataspace_capability alloc_dma_buffer(Device_capability,
		                                                          Genode::size_t) = 0;

		/**
		 * Free previously allocated DMA memory
		 */
		virtual void free_dma_buffer(Device_capability,
		                             Genode::Ram_dataspace_capability) = 0;

		/*********************
		 ** RPC declaration **
		 *********************/

		GENODE_RPC(Rpc_first_device, Device_capability, first_device,
		           unsigned, unsigned);
		GENODE_RPC(Rpc_next_device, Device_capability, next_device,
		           Device_capability, unsigned, unsigned);
		GENODE_RPC(Rpc_release_device, void, release_device, Device_capability);
		GENODE_RPC(Rpc_config_extended, Genode::Io_mem_dataspace_capability,
		           config_extended, Device_capability);
		GENODE_RPC_THROW(Rpc_alloc_dma_buffer, Genode::Ram_dataspace_capability,
		                 alloc_dma_buffer,
		                 GENODE_TYPE_LIST(Genode::Ram_session::Quota_exceeded),
		                 Device_capability, Genode::size_t);
		GENODE_RPC(Rpc_free_dma_buffer, void, free_dma_buffer,
		                 Device_capability, Genode::Ram_dataspace_capability);

		GENODE_RPC_INTERFACE(Rpc_first_device, Rpc_next_device,
		                     Rpc_release_device, Rpc_config_extended,
		                     Rpc_alloc_dma_buffer, Rpc_free_dma_buffer);
	};
}

#endif /* _INCLUDE__PCI_SESSION__PCI_SESSION_H_ */
