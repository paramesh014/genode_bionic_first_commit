/*
 * \brief  IRQ session interface for NOVA
 * \author Norman Feske
 * \date   2010-01-30
 */

/*
 * Copyright (C) 2010-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _CORE__INCLUDE__IRQ_SESSION_COMPONENT_H_
#define _CORE__INCLUDE__IRQ_SESSION_COMPONENT_H_

#include <base/lock.h>
#include <util/list.h>

#include <irq_session/capability.h>

namespace Genode {

	class Irq_session_component : public Rpc_object<Irq_session>,
	                              public List<Irq_session_component>::Element
	{
		private:

			enum { STACK_SIZE = 4096 };

			unsigned               _irq_number;
			Range_allocator       *_irq_alloc;
			Rpc_entrypoint         _entrypoint;
			Irq_session_capability _cap;
			bool                   _attached;

		public:

			/**
			 * Constructor
			 *
			 * \param cap_session  capability session to use
			 * \param irq_alloc    platform-dependent IRQ allocator
			 * \param args         session construction arguments
			 */
			Irq_session_component(Cap_session     *cap_session,
			                      Range_allocator *irq_alloc,
			                      const char      *args);

			/**
			 * Destructor
			 */
			~Irq_session_component();

			/**
			 * Return capability to this session
			 *
			 * If an initialization error occurs, returned capability is invalid.
			 */
			Irq_session_capability cap() const { return _cap; }


			/***************************
			 ** Irq session interface **
			 ***************************/

			void wait_for_irq();
	};
}

#endif /* _CORE__INCLUDE__IRQ_SESSION_COMPONENT_H_ */
