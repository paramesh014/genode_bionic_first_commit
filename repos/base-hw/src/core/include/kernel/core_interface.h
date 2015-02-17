/*
 * \brief  Parts of the kernel interface that are restricted to core
 * \author Martin stein
 * \date   2014-03-15
 */

/*
 * Copyright (C) 2014 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _KERNEL__CORE_INTERFACE_H_
#define _KERNEL__CORE_INTERFACE_H_

/* base-hw includes */
#include <kernel/interface.h>

namespace Kernel
{
	addr_t   mode_transition_base();
	size_t   mode_transition_size();
	size_t   thread_size();
	size_t   pd_size();
	unsigned pd_alignment_log2();
	size_t   signal_context_size();
	size_t   signal_receiver_size();
	size_t   vm_size();

	/**
	 * Kernel names of the kernel calls
	 */
	constexpr Call_arg call_id_new_thread()          { return 14; }
	constexpr Call_arg call_id_bin_thread()          { return 15; }
	constexpr Call_arg call_id_start_thread()        { return 16; }
	constexpr Call_arg call_id_resume_thread()       { return 17; }
	constexpr Call_arg call_id_access_thread_regs()  { return 18; }
	constexpr Call_arg call_id_route_thread_event()  { return 19; }
	constexpr Call_arg call_id_update_pd()           { return 20; }
	constexpr Call_arg call_id_new_pd()              { return 21; }
	constexpr Call_arg call_id_bin_pd()              { return 22; }
	constexpr Call_arg call_id_new_signal_receiver() { return 23; }
	constexpr Call_arg call_id_new_signal_context()  { return 24; }
	constexpr Call_arg call_id_bin_signal_context()  { return 25; }
	constexpr Call_arg call_id_bin_signal_receiver() { return 26; }
	constexpr Call_arg call_id_new_vm()              { return 27; }
	constexpr Call_arg call_id_run_vm()              { return 28; }
	constexpr Call_arg call_id_pause_vm()            { return 29; }
	constexpr Call_arg call_id_pause_thread()        { return 30; }

	/**
	 * Create a domain
	 *
	 * \param dst  appropriate memory donation for the kernel object
	 * \param pd   core local Platform_pd object
	 *
	 * \retval >0  kernel name of the new domain
	 * \retval  0  failed
	 */
	inline unsigned new_pd(void * const dst, Platform_pd * const pd)
	{
		return call(call_id_new_pd(), (Call_arg)dst, (Call_arg)pd);
	}


	/**
	 * Destruct a domain
	 *
	 * \param pd_id  kernel name of the targeted domain
	 *
	 * \retval  0  succeeded
	 * \retval -1  failed
	 */
	inline int bin_pd(unsigned const pd_id)
	{
		return call(call_id_bin_pd(), pd_id);
	}


	/**
	 * Update locally effective domain configuration to in-memory state
	 *
	 * \param pd_id  kernel name of the targeted domain
	 *
	 * Kernel and/or hardware may cache parts of a domain configuration. This
	 * function ensures that the in-memory state of the targeted domain gets
	 * processor locally effective.
	 */
	inline void update_pd(unsigned const pd_id)
	{
		call(call_id_update_pd(), pd_id);
	}


	/**
	 * Create a thread
	 *
	 * \param p         memory donation for the new kernel thread object
	 * \param priority  scheduling priority of the new thread
	 * \param label     debugging label of the new thread
	 *
	 * \retval >0  kernel name of the new thread
	 * \retval  0  failed
	 */
	inline unsigned new_thread(void * const p, unsigned const priority,
	                           char const * const label)
	{
		return call(call_id_new_thread(), (Call_arg)p, (Call_arg)priority,
		            (Call_arg)label);
	}


	/**
	 * Pause execution of a specific thread
	 *
	 * \param thread_id  kernel name of the targeted thread
	 */
	inline void pause_thread(unsigned const thread_id)
	{
		call(call_id_pause_thread(), thread_id);
	}


	/**
	 * Destruct a thread
	 *
	 * \param thread_id  kernel name of the targeted thread
	 */
	inline void bin_thread(unsigned const thread_id)
	{
		call(call_id_bin_thread(), thread_id);
	}


	/**
	 * Start execution of a thread
	 *
	 * \param thread_id  kernel name of the targeted thread
	 * \param cpu_id     kernel name of the targeted processor
	 * \param pd_id      kernel name of the targeted domain
	 * \param utcb       core local pointer to userland thread-context
	 */
	inline Tlb * start_thread(unsigned const thread_id, unsigned const cpu_id,
	                          unsigned const pd_id, Native_utcb * const utcb)
	{
		return (Tlb *)call(call_id_start_thread(), thread_id, cpu_id, pd_id,
		                   (Call_arg)utcb);
	}


	/**
	 * Cancel blocking of a thread if possible
	 *
	 * \param thread_id  kernel name of the targeted thread
	 *
	 * \return  wether thread was in a cancelable blocking beforehand
	 */
	inline bool resume_thread(unsigned const thread_id)
	{
		return call(call_id_resume_thread(), thread_id);
	}


	/**
	 * Set or unset the handler of an event that can be triggered by a thread
	 *
	 * \param thread_id          kernel name of the targeted thread
	 * \param event_id           kernel name of the targeted thread event
	 * \param signal_context_id  kernel name of the handlers signal context
	 *
	 * \retval  0  succeeded
	 * \retval -1  failed
	 */
	inline int route_thread_event(unsigned const thread_id,
	                              unsigned const event_id,
	                              unsigned const signal_context_id)
	{
		return call(call_id_route_thread_event(), thread_id,
		            event_id, signal_context_id);
	}


	/**
	 * Access plain member variables of a kernel thread-object
	 *
	 * \param thread_id  kernel name of the targeted thread
	 * \param reads      amount of read operations
	 * \param writes     amount of write operations
	 * \param values     base of the value buffer for all operations
	 *
	 * \return  amount of undone operations according to the execution order
	 *
	 * Operations are executed in order of the appearance of the register names
	 * in the callers UTCB. If reads = 0, read_values is of no relevance. If
	 * writes = 0, write_values is of no relevance.
	 *
	 * Expected structure at the callers UTCB base:
	 *
	 *                    0 * sizeof(addr_t): read register name #1
	 *                  ...                   ...
	 *          (reads - 1) * sizeof(addr_t): read register name #reads
	 *          (reads - 0) * sizeof(addr_t): write register name #1
	 *                  ...                   ...
	 * (reads + writes - 1) * sizeof(addr_t): write register name #writes
	 *
	 * Expected structure at values:
	 *
	 *                    0 * sizeof(addr_t): read destination #1
	 *                  ...                   ...
	 *          (reads - 1) * sizeof(addr_t): read destination #reads
	 *          (reads - 0) * sizeof(addr_t): write value #1
	 *                  ...                   ...
	 * (reads + writes - 1) * sizeof(addr_t): write value #writes
	 */
	inline unsigned access_thread_regs(unsigned const thread_id,
	                                   unsigned const reads,
	                                   unsigned const writes,
	                                   addr_t * const values)
	{
		return call(call_id_access_thread_regs(), thread_id, reads, writes,
		            (Call_arg)values);
	}


	/**
	 * Create a signal receiver
	 *
	 * \param p  memory donation for the kernel signal-receiver object
	 *
	 * \retval >0  kernel name of the new signal receiver
	 * \retval  0  failed
	 */
	inline unsigned new_signal_receiver(addr_t const p)
	{
		return call(call_id_new_signal_receiver(), p);
	}


	/**
	 * Create a signal context and assign it to a signal receiver
	 *
	 * \param p         memory donation for the kernel signal-context object
	 * \param receiver  kernel name of targeted signal receiver
	 * \param imprint   user label of the signal context
	 *
	 * \retval >0  kernel name of the new signal context
	 * \retval  0  failed
	 */
	inline unsigned new_signal_context(addr_t const   p,
	                                   unsigned const receiver,
	                                   unsigned const imprint)
	{
		return call(call_id_new_signal_context(), p, receiver, imprint);
	}


	/**
	 * Destruct a signal context
	 *
	 * \param context  kernel name of the targeted signal context
	 *
	 * \retval  0  suceeded
	 * \retval -1  failed
	 */
	inline int bin_signal_context(unsigned const context)
	{
		return call(call_id_bin_signal_context(), context);
	}


	/**
	 * Destruct a signal receiver
	 *
	 * \param receiver  kernel name of the targeted signal receiver
	 *
	 * \retval  0  suceeded
	 * \retval -1  failed
	 */
	inline int bin_signal_receiver(unsigned const receiver)
	{
		return call(call_id_bin_signal_receiver(), receiver);
	}


	/**
	 * Create a virtual machine that is stopped initially
	 *
	 * \param dst                memory donation for the VM object
	 * \param state              location of the processor state of the VM
	 * \param signal_context_id  kernel name of the signal context for VM events
	 *
	 * \retval >0  kernel name of the new VM
	 * \retval  0  failed
	 *
	 * Regaining of the supplied memory is not supported by now.
	 */
	inline unsigned new_vm(void * const dst, void * const state,
	                  unsigned const signal_context_id)
	{
		return call(call_id_new_vm(), (Call_arg)dst, (Call_arg)state,
		            signal_context_id);
	}


	/**
	 * Execute a virtual-machine (again)
	 *
	 * \param vm_id  kernel name of the targeted VM
	 */
	inline void run_vm(unsigned const vm_id)
	{
		call(call_id_run_vm(), vm_id);
	}


	/**
	 * Stop execution of a virtual-machine
	 *
	 * \param vm_id  kernel name of the targeted VM
	 */
	inline void pause_vm(unsigned const vm_id)
	{
		call(call_id_pause_vm(), vm_id);
	}
}

#endif /* _KERNEL__CORE_INTERFACE_H_ */
