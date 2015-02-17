/*
 * \brief  Signal context for timer events
 * \author Sebastian Sumpf <sebastian.sumpf@genode-labs.com>
 * \date   2012-05-23
 */

/*
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <base/env.h>
#include <base/printf.h>

#include <lx_emul.h>
#include <env.h>

extern "C" {
#include <dde_kit/timer.h>
}

static void handler(void *timer);

/**
 * Signal context for time-outs
 */
class Timer_context
{
	private:

		timer_list                              *_timer;     /* Linux timer */
		dde_kit_timer                           *_dde_timer; /* DDE kit timer */
		Genode::Signal_dispatcher<Timer_context> _dispatcher;

		/* call timer function */
		void _handle(unsigned) { _timer->function(_timer->data); }

	public:

		Timer_context(timer_list *timer)
		: _timer(timer), _dde_timer(0),
		  _dispatcher(*Net::Env::receiver(), *this, &Timer_context::_handle) {}

		/* schedule next timeout */
		void schedule(unsigned long expires)
		{
			if (!_dde_timer)
				_dde_timer = dde_kit_timer_add(handler, this, expires);
			else
				dde_kit_timer_schedule_absolute(_dde_timer, expires);
		}

		/**
		 * Return true if timer is pending
		 */
		bool pending() const
		{
			return _dde_timer ? dde_kit_timer_pending(_dde_timer) : false;
		}

		/**
		 * Return internal signal cap
		 */
		Genode::Signal_context_capability cap() const  { return _dispatcher; }
		void submit() { _dispatcher.submit(1); }

		/**
		 * Convert 'timer_list' to 'Timer_conext'
		 */
		static Timer_context *to_ctx(timer_list const *timer) {
			return static_cast<Timer_context *>(timer->timer); }

		void remove()
		{
			if (_dde_timer)
				dde_kit_timer_del(_dde_timer);

			_dde_timer = 0;
		}
};


/**
 * C handler for DDE timer interface
 */
static void handler(void *timer)
{
	Timer_context *t = static_cast<Timer_context *>(timer);
	t->submit();
	/* set context and submit
	Genode::Signal_transmitter transmitter(t->cap());
	transmitter.submit(); */
}


/*******************
 ** linux/timer.h **
 *******************/

void init_timer(struct timer_list *timer) {
	timer->timer = (void *) new (Genode::env()->heap()) Timer_context(timer); }


void add_timer(struct timer_list *timer)
{
	BUG_ON(timer_pending(timer));
	mod_timer(timer, timer->expires);
}


int mod_timer(struct timer_list *timer, unsigned long expires)
{
	dde_kit_log(DEBUG_TIMER, "Timer: %p j: %lu ex: %lu func %p",
	            timer, jiffies, expires, timer->function);
	Timer_context::to_ctx(timer)->schedule(expires);
	return 0; 
}


void setup_timer(struct timer_list *timer,void (*function)(unsigned long),
                 unsigned long data)
{
	timer->function = function;
	timer->data     = data;
	init_timer(timer);
}


int timer_pending(const struct timer_list * timer)
{
	bool pending = Timer_context::to_ctx(timer)->pending();
	dde_kit_log(DEBUG_TIMER, "Pending %p %u", timer, pending);
	return pending;
}


int del_timer(struct timer_list *timer)
{
	dde_kit_log(DEBUG_TIMER, "Delete timer %p", timer);
	Timer_context::to_ctx(timer)->remove();
	return 0;
}

