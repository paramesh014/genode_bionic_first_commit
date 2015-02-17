/*
 * \brief  Dummy definitions for native types used for compiling unit tests
 * \author Norman Feske
 * \date   2009-10-02
 */

/*
 * Copyright (C) 2009-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__BASE__NATIVE_TYPES_H_
#define _INCLUDE__BASE__NATIVE_TYPES_H_

#include <base/native_capability.h>
#include <base/stdint.h>

namespace Codezero {

	struct l4_mutex;

	enum { NILTHREAD = -1 };
}

namespace Genode {

	class Platform_thread;

	struct Cap_dst_policy
	{
		typedef int Dst;

		static bool valid(Dst tid) { return tid != Codezero::NILTHREAD; }
		static Dst  invalid()      { return Codezero::NILTHREAD;        }
		static void copy(void* dst, Native_capability_tpl<Cap_dst_policy>* src);
	};

	typedef Cap_dst_policy::Dst Native_thread_id;

	struct Native_thread
	{
		Native_thread_id l4id;

		/**
		 * Only used in core
		 *
		 * For 'Thread' objects created within core, 'pt' points to the
		 * physical thread object, which is going to be destroyed on
		 * destruction of the 'Thread'.
		 */
		Platform_thread *pt;
	};

	/**
	 * Empty UTCB type expected by the thread library
	 *
	 * On this kernel, UTCBs are not placed within the the context area. Each
	 * thread can request its own UTCB pointer using the kernel interface.
	 * However, we use the 'Native_utcb' member of the thread context to
	 * hold thread-specific data, i.e. the running lock used by the lock
	 * implementation.
	 */
	struct Native_utcb
	{
		private:

			/**
			 * Prevent construction
			 *
			 * A UTCB is never constructed, it is backed by zero-initialized memory.
			 */
			Native_utcb();

			/**
			 * Backing store for per-thread running lock
			 *
			 * The size of this member must equal 'sizeof(Codezero::l4_mutex)'.
			 * Unfortunately, we cannot include the Codezero headers here.
			 */
			int _running_lock;

		public:

			Codezero::l4_mutex *running_lock() {
				return (Codezero::l4_mutex *)&_running_lock; }
	};

	typedef Native_capability_tpl<Cap_dst_policy> Native_capability;
	typedef int Native_connection_state;

	struct Native_config
	{
		/**
		 * Thread-context area configuration.
		 */
		static constexpr addr_t context_area_virtual_base() {
			return 0x40000000UL; }
		static constexpr addr_t context_area_virtual_size() {
			return 0x10000000UL; }

		/**
		 * Size of virtual address region holding the context of one thread
		 */
		static constexpr addr_t context_virtual_size() { return 0x00100000UL; }
	};

	struct Native_pd_args { };
}


#endif /* _INCLUDE__BASE__NATIVE_TYPES_H_ */
