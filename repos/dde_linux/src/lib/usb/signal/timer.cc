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
#include "signal.h"

static void handler(void *timer);


/* our local incarnation of sender and receiver */
static Signal_helper *_signal = 0;


/**
 * Signal context for time-outs
 */
template<typename CLASS>
class Timer_context
{
	private:

		CLASS                                   *_timer;     /* Linux timer */
		dde_kit_timer                           *_dde_timer; /* DDE kit timer */
		Genode::Signal_rpc_member<Timer_context> _dispatcher;

		/* call timer function */
		void _handle(unsigned) { _timer->function(_timer->data); }

	public:

		Timer_context(CLASS *timer)
		: _timer(timer), _dde_timer(0),
		  _dispatcher(_signal->ep(), *this, &Timer_context::_handle) {}

		/* schedule next timeout */
		void schedule(unsigned long expires)
		{
			if (!_dde_timer)
				_dde_timer = dde_kit_timer_add(handler, this, expires);
			else
				dde_kit_timer_schedule_absolute(_dde_timer, expires);
		}

		char const *debug() { return "Timer_context"; }

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

		/**
		 * Convert 'timer_list' to 'Timer_conext'
		 */
		static Timer_context *to_ctx(CLASS const *timer) {
			return static_cast<Timer_context<CLASS> *>(timer->timer); }

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
	Timer_context<timer_list> *t = static_cast<Timer_context<timer_list> *>(timer);

	/* set context and submit */
	_signal->sender().context(t->cap());
	_signal->sender().submit();
}


void Timer::init(Server::Entrypoint &ep) {
	_signal = new (Genode::env()->heap()) Signal_helper(ep); }


/*******************
 ** linux/timer.h **
 *******************/

void init_timer(struct timer_list *timer) {
	timer->timer = (void *) new (Genode::env()->heap()) Timer_context<timer_list>(timer); }


int mod_timer(struct timer_list *timer, unsigned long expires)
{
	dde_kit_log(DEBUG_TIMER, "Timer: %p j: %lu ex: %lu func %p",
	            timer, jiffies, expires, timer->function);
	Timer_context<timer_list>::to_ctx(timer)->schedule(expires);
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
	bool pending = Timer_context<timer_list>::to_ctx(timer)->pending();
	dde_kit_log(DEBUG_TIMER, "Pending %p %u", timer, pending);
	return pending;
}


int del_timer(struct timer_list *timer)
{
	dde_kit_log(DEBUG_TIMER, "Delete timer %p", timer);
	Timer_context<timer_list>::to_ctx(timer)->remove();
	return 0;
}


/*********************
 ** linux/hrtimer.h **
 *********************/

void hrtimer_init(struct hrtimer *timer, clockid_t clock_id, enum hrtimer_mode mode)
{
	timer->timer = (void *) new (Genode::env()->heap()) Timer_context<hrtimer>(timer);
	timer->data  = timer;
}


int hrtimer_start_range_ns(struct hrtimer *timer, ktime_t tim,
                           unsigned long delta_ns, const enum hrtimer_mode mode)
{
	unsigned long expires = tim.tv64 / (NSEC_PER_MSEC * DDE_KIT_HZ);
	dde_kit_log(DEBUG_TIMER, "HR: e: %lu j %lu", jiffies, expires);
	Timer_context<hrtimer>::to_ctx(timer)->schedule(expires);
	return 0;
}


int hrtimer_cancel(struct hrtimer *timer)
{
	Timer_context<hrtimer>::to_ctx(timer)->remove();
	return 0;
}
