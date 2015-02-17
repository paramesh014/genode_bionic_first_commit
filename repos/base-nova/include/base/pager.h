/*
 * \brief  Paging-server framework
 * \author Norman Feske
 * \date   2006-04-28
 */

/*
 * Copyright (C) 2006-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__BASE__PAGER_H_
#define _INCLUDE__BASE__PAGER_H_

#include <base/thread.h>
#include <base/object_pool.h>
#include <base/ipc_pager.h>
#include <base/capability.h>
#include <cap_session/cap_session.h>
#include <pager/capability.h>

namespace Genode {

	class Pager_entrypoint;

	/*
	 * On NOVA, each pager object is an EC that corresponds to one user thread.
	 */
	class Pager_object : public Object_pool<Pager_object>::Entry,
	                     Thread_base
	{
		private:

			void entry() { }
			void start() { }

			unsigned long _badge;  /* used for debugging */

			/**
			 * User-level signal handler registered for this pager object via
			 * 'Cpu_session::exception_handler()'.
			 */
			Signal_context_capability _exception_sigh;

			/**
			 * Portal selector for object cleanup/destruction
			 */
			addr_t _pt_cleanup;

			addr_t _initial_esp;
			addr_t _initial_eip;
			addr_t _client_exc_pt_sel;
			addr_t _client_exc_vcpu;

			struct
			{
				struct Thread_state thread;
				addr_t sel_client_ec;
				enum {
					VALID         = 0x1U,
					DEAD          = 0x2U,
					SINGLESTEP    = 0x4U,
					CLIENT_CANCEL = 0x8U,
					SIGNAL_SM     = 0x10U,
				};
				uint8_t _status;

				/* convenience function to access pause/recall state */
				inline bool is_valid() { return _status & VALID; }
				inline void mark_valid() { _status |= VALID; }
				inline void mark_invalid() { _status &= ~VALID; }

				inline bool is_client_cancel() { return _status & CLIENT_CANCEL; }
				inline void mark_client_cancel() { _status |= CLIENT_CANCEL; }
				inline void unmark_client_cancel() { _status &= ~CLIENT_CANCEL; }

				inline void mark_dead() { _status |= DEAD; }
				inline bool is_dead() { return _status & DEAD; }

				inline bool singlestep() { return _status & SINGLESTEP; }

				inline void mark_signal_sm() { _status |= SIGNAL_SM; }
				inline bool has_signal_sm() { return _status & SIGNAL_SM; }
			} _state;

			Thread_capability _thread_cap;

			void _copy_state(Nova::Utcb * utcb);

			/**
			 * Semaphore selector to synchronize pause/state/resume operations
			 */
			addr_t sm_state_notify() { return _pt_cleanup + 1; }

			static void _page_fault_handler();
			static void _startup_handler();
			static void _invoke_handler();
			static void _recall_handler();

			__attribute__((regparm(1)))
			static void _exception_handler(addr_t portal_id);

			static Nova::Utcb * _check_handler(Thread_base *&, Pager_object *&);

		public:

			Pager_object(unsigned long badge, Affinity::Location location);

			virtual ~Pager_object();

			unsigned long badge() const { return _badge; }

			virtual int pager(Ipc_pager &ps) = 0;

			/**
			 * Assign user-level exception handler for the pager object
			 */
			void exception_handler(Signal_context_capability sigh)
			{
				_exception_sigh = sigh;
			}

			/**
			 * Return base of initial portal window
			 */
			addr_t ec_sel() { return _tid.ec_sel; }

			/**
			 * Return base of initial portal window
			 */
			addr_t exc_pt_sel() { return _tid.exc_pt_sel; }
			addr_t exc_pt_sel_client() { return _client_exc_pt_sel; }
			addr_t exc_pt_vcpu() { return _client_exc_vcpu; }

			/**
			 * Set initial stack pointer used by the startup handler
			 */
			addr_t initial_esp() { return _initial_esp; }
			void initial_esp(addr_t esp) { _initial_esp = esp; }

			/**
			 * Set initial instruction pointer used by the startup handler
			 */
			void initial_eip(addr_t eip) { _initial_eip = eip; }

			/**
			 * Continue execution of pager object
			 */
			void wake_up();

			/**
			 * Notify exception handler about the occurrence of an exception
			 */
			bool submit_exception_signal()
			{
				if (!_exception_sigh.valid()) return false;

				Signal_transmitter transmitter(_exception_sigh);
				transmitter.submit();

				return true;
			}

			/**
			 * Return entry point address
			 */
			addr_t handler_address()
			{
				return reinterpret_cast<addr_t>(_invoke_handler);
			}

			/**
			 * Return semaphore to block on until state of a recall is
			 * available.
			 */
			Native_capability notify_sm()
			{
				if (_state.is_valid() || _state.is_dead())
					return Native_capability();

				return Native_capability(sm_state_notify());
			}

			/**
			 * Copy thread state of recalled thread.
			 */
			bool copy_thread_state(Thread_state * state_dst)
			{
				if (!state_dst || !_state.is_valid())
					return false;

				*state_dst = _state.thread;

				return true;
			}

			/**
			 * Cancel blocking in a lock so that recall exception can take
			 * place.
			 */
			void    client_cancel_blocking();

			uint8_t client_recall();
			void    client_set_ec(addr_t ec) { _state.sel_client_ec = ec; }

			inline void single_step(bool on)
			{
				if (on)
					_state._status |= _state.SINGLESTEP;
				else
					_state._status &= ~_state.SINGLESTEP;
			}

			/**
			 * Remember thread cap so that rm_session can tell thread that
			 * rm_client is gone.
			 */
			Thread_capability thread_cap() { return _thread_cap; } const
			void thread_cap(Thread_capability cap) { _thread_cap = cap; }

			/*
			 * Note in the thread state that an unresolved page
			 * fault occurred.
			 */
			void unresolved_page_fault_occurred()
			{
				_state.thread.unresolved_page_fault = true;
			}

			/**
			 * Make sure nobody is in the handler anymore by doing an IPC to a
			 * local cap pointing to same serving thread (if not running in the
			 * context of the serving thread). When the call returns
			 * we know that nobody is handled by this object anymore, because
			 * all remotely available portals had been revoked beforehand.
			 */
			void cleanup_call();

			/**
			 * Open receive window for initial portals for vCPU.
			 */
			void prepare_vCPU_portals()
			{
				_client_exc_vcpu = cap_map()->insert(Nova::NUM_INITIAL_VCPU_PT_LOG2);

				Nova::Utcb *utcb = reinterpret_cast<Nova::Utcb *>(Thread_base::utcb());

				utcb->crd_rcv = Nova::Obj_crd(_client_exc_vcpu, Nova::NUM_INITIAL_VCPU_PT_LOG2); 
			}
	};


	/**
	 * Dummy pager activation
	 *
	 * Because on NOVA each pager object can be invoked separately,
	 * there is no central pager activation.
	 */
	class Pager_activation_base { };


	template <unsigned STACK_SIZE>
	class Pager_activation : public Pager_activation_base
	{ };


	/**
	 * Dummy pager entrypoint
	 */
	class Pager_entrypoint : public Object_pool<Pager_object>
	{
		private:

			Cap_session *_cap_session;

		public:

			Pager_entrypoint(Cap_session *cap_session,
			                 Pager_activation_base *a = 0)
			: _cap_session(cap_session) { }

			/**
			 * Return capability for 'Pager_object'
			 */
			Pager_capability manage(Pager_object *obj);

			/**
			 * Dissolve 'Pager_object' from entry point
			 */
			void dissolve(Pager_object *obj);
	};
}

#endif /* _INCLUDE__BASE__PAGER_H_ */
