/*
 * \brief   Kernel backend for execution contexts in userland
 * \author  Martin Stein
 * \date    2012-11-30
 */

/*
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _KERNEL__THREAD_H_
#define _KERNEL__THREAD_H_

/* core includes */
#include <kernel/configuration.h>
#include <kernel/signal_receiver.h>
#include <kernel/ipc_node.h>
#include <kernel/processor.h>
#include <kernel/thread_base.h>

namespace Kernel
{
	class Thread;
	class Pd;

	typedef Genode::Native_utcb Native_utcb;

	/**
	 * Kernel backend for userland execution-contexts
	 */
	class Thread;

	class Thread_ids : public Id_allocator<MAX_THREADS> { };
	typedef Object_pool<Thread> Thread_pool;

	Thread_ids  * thread_ids();
	Thread_pool * thread_pool();
}

class Kernel::Thread
:
	public Cpu::User_context,
	public Object<Thread, MAX_THREADS, Thread_ids, thread_ids, thread_pool>,
	public Processor_client,
	public Processor_domain_update,
	public Ipc_node,
	public Signal_context_killer,
	public Signal_handler,
	public Thread_base
{
	friend class Thread_event;

	private:

		enum { START_VERBOSE = 0 };

		enum State
		{
			SCHEDULED                   = 1,
			AWAITS_START                = 2,
			AWAITS_IPC                  = 3,
			AWAITS_RESUME               = 4,
			AWAITS_SIGNAL               = 5,
			AWAITS_SIGNAL_CONTEXT_KILL  = 6,
			STOPPED                     = 7,
		};

		State              _state;
		Pd *               _pd;
		Native_utcb *      _utcb_phys;
		Signal_receiver *  _signal_receiver;
		char const * const _label;

		/**
		 * Notice that another thread yielded the CPU to this thread
		 */
		void _receive_yielded_cpu();

		/**
		 * Attach or detach the handler of a thread-triggered event
		 *
		 * \param event_id           kernel name of the thread event
		 * \param signal_context_id  kernel name signal context or 0 to detach
		 *
		 * \retval  0  succeeded
		 * \retval -1  failed
		 */
		int _route_event(unsigned const event_id,
		                 unsigned const signal_context_id);

		/**
		 * Map kernel name of thread event to the corresponding member
		 *
		 * \param id  kernel name of targeted thread event
		 *
		 * \retval  0  failed
		 * \retval >0  targeted member pointer
		 */
		Thread_event Thread::* _event(unsigned const id) const;

		/**
		 * Return wether this is a core thread
		 */
		bool _core() const;

		/**
		 * Resume execution rawly
		 */
		void _schedule();

		/**
		 * Pause execution rawly
		 */
		void _unschedule(State const s);

		/**
		 * Pause execution
		 */
		void _pause();

		/**
		 * Suspend unrecoverably from execution
		 */
		void _stop();

		/**
		 * Cancel blocking if possible
		 *
		 * \return  wether thread was in a cancelable blocking beforehand
		 */
		bool _resume();

		/**
		 * Handle an exception thrown by the memory management unit
		 */
		void _mmu_exception();

		/**
		 * Handle kernel-call request of the thread
		 */
		void _call();

		/**
		 * Read a thread register
		 *
		 * \param id     kernel name of targeted thread register
		 * \param value  read-value buffer
		 *
		 * \retval  0  succeeded
		 * \retval -1  failed
		 */
		int _read_reg(addr_t const id, addr_t & value) const;

		/**
		 * Override a thread register
		 *
		 * \param id     kernel name of targeted thread register
		 * \param value  write-value buffer
		 *
		 * \retval  0  succeeded
		 * \retval -1  failed
		 */
		int _write_reg(addr_t const id, addr_t const value);

		/**
		 * Map kernel names of thread registers to the corresponding data
		 *
		 * \param id  kernel name of thread register
		 *
		 * \retval  0  failed
		 * \retval >0  pointer to register content
		 */
		addr_t Thread::* _reg(addr_t const id) const;

		/**
		 * Print table of all threads and their current activity
		 */
		void _print_activity_table();

		/**
		 * Print the activity of the thread
		 *
		 * \param printing_thread  wether this thread caused the debugging
		 */
		void _print_activity(bool const printing_thread);

		/**
		 * Print the activity of the thread when it awaits a message
		 */
		void _print_activity_when_awaits_ipc();

		/**
		 * Print activity info that is printed regardless of the thread state
		 */
		void _print_common_activity();


		/*********************************************************
		 ** Kernel-call back-ends, see kernel-interface headers **
		 *********************************************************/

		void _call_new_pd();
		void _call_bin_pd();
		void _call_new_thread();
		void _call_bin_thread();
		void _call_start_thread();
		void _call_pause_current_thread();
		void _call_pause_thread();
		void _call_resume_thread();
		void _call_resume_local_thread();
		void _call_yield_thread();
		void _call_await_request_msg();
		void _call_send_request_msg();
		void _call_send_reply_msg();
		void _call_update_pd();
		void _call_update_data_region();
		void _call_update_instr_region();
		void _call_print_char();
		void _call_new_signal_receiver();
		void _call_new_signal_context();
		void _call_await_signal();
		void _call_signal_pending();
		void _call_submit_signal();
		void _call_ack_signal();
		void _call_kill_signal_context();
		void _call_bin_signal_context();
		void _call_bin_signal_receiver();
		void _call_new_vm();
		void _call_run_vm();
		void _call_pause_vm();
		void _call_access_thread_regs();
		void _call_route_thread_event();


		/***************************
		 ** Signal_context_killer **
		 ***************************/

		void _signal_context_kill_pending();
		void _signal_context_kill_failed();
		void _signal_context_kill_done();


		/********************
		 ** Signal_handler **
		 ********************/

		void _await_signal(Signal_receiver * const receiver);
		void _receive_signal(void * const base, size_t const size);


		/**************
		 ** Ipc_node **
		 **************/

		void _send_request_succeeded();
		void _send_request_failed();
		void _await_request_succeeded();
		void _await_request_failed();


		/*****************************
		 ** Processor_domain_update **
		 *****************************/

		void _processor_domain_update_unblocks() { _resume(); }

	public:

		/**
		 * Constructor
		 *
		 * \param priority  scheduling priority
		 * \param label     debugging label
		 */
		Thread(unsigned const priority, char const * const label);

		/**
		 * Prepare thread to get scheduled the first time
		 *
		 * \param processor  targeted processor
		 * \param pd         targeted domain
		 * \param utcb       core local pointer to userland thread-context
		 * \param start      wether to start executing the thread
		 */
		void init(Processor * const processor, Pd * const pd,
		          Native_utcb * const utcb, bool const start);


		/**********************
		 ** Processor_client **
		 **********************/

		void exception(unsigned const processor_id);
		void proceed(unsigned const processor_id);


		/***************
		 ** Accessors **
		 ***************/

		unsigned     id() const { return Object::id(); }
		char const * label() const { return _label; };
		unsigned     pd_id() const;
		char const * pd_label() const;
};

#endif /* _KERNEL__THREAD_H_ */
