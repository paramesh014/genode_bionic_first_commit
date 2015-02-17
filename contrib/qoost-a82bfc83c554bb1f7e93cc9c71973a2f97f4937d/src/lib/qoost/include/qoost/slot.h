/*
 * \brief  Utility for supporting Qt slots in C++ templates
 * \author Genode Labs <qoost@genode-labs.com>
 * \date   2011-03-08
 *
 * Copyright (C) 2011 by secunet Security Networks AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _INCLUDE__QOOST__SLOT_H_
#define _INCLUDE__QOOST__SLOT_H_

/* Qt4 includes */
#include <QObject>

/* Qoost includes */
#include <qoost/qmember.h>


/**
 * Abstract interface for calling a slot function
 *
 * This interface is provided by each 'Slot' instance and called by the
 * 'Slot_proxy' on the reception of signals.
 */
struct Slot_base
{
	virtual void call() = 0;
	virtual ~Slot_base() { }
};


/**
 * Proxy 'QObject' that directs each incoming signal to a virtual function
 * call of its associated 'Slot_base' object.
 *
 * This class should be private to 'Slot'. However, it is not possible to
 * nest 'Q_OBJECT' classes.
 */
class Slot_proxy : public QObject
{
	Q_OBJECT

	private:

		Slot_base *_slot;

	private Q_SLOTS:

		void invoke() { _slot->call(); }

	public:

		Slot_proxy(Slot_base *slot) : _slot(slot) { }
};


/**
 * Representation of a single slot
 *
 * By inheriting a 'Slot' class template, the inheriting class becomes the
 * host of one Qt4 slot. This slot can be connected to Qt4 signals. In
 * contrast to normal Qt4 slot functions, the 'Slot' template is represented
 * by a slot type specified as 'SLOT_TYPE' template argument. Internally,
 * each 'Slot' utilizes a dedicated 'QObject' that acts as a proxy. The
 * 'Slot_proxy' implements a normal Qt4 slot function and propagates the
 * slot invocation to the virtual 'call' function provided by 'Slot'.
 * This call function, in turn, calls a pointer-to-member functions as
 * registered during the construction of the 'Slot'.
 *
 * Example
 * -------
 *
 * ! class State_changed { };
 * !
 * ! class Custom_handler : public Slot<Custom_handler, State_changed>
 * ! {
 * !   private:
 * !
 * !     void _state_changed() { ... }
 * !
 * !   public:
 * !
 * !    Custom_handler() :
 * !      Slot<Custom_handler, State_changed>(&Custom_handler::_state_changed)
 * !    { }
 * ! };
 *
 * The 'State_changed' class represents the slot type. The 'Custom_handler'
 * implements the slot type 'State_changed' by inheriting from the parametrized
 * 'Slot' class template and registering one of its member functions as slot
 * handler to the constructor of 'Slot'.
 *
 * Note that in the example above, 'Custom_handler' is not a Q_OBJECT. Hence,
 * in contrast to normal Qt4 slots, the 'Slot' class template can be used by
 * C++ templates to receive signals.
 *
 * Limitations
 * -----------
 *
 * Slot functions provided via the 'Slot' class template cannot take any
 * arguments. It is only suitable for the classical notification use case for
 * signals where the signal handler responds to a signal by querying state
 * changes explicitly rather than receiving state changes with the signal.
 */
template <typename RECEIVER, typename SLOT_TYPE>
class Slot : public Slot_base
{
	private:

		QMember<Slot_proxy> _slot_proxy;

		void (RECEIVER::*_method) ();

		void call()
		{
			RECEIVER *r = static_cast<RECEIVER *>(this);
			(r->*_method)();
		}

	public:

		Slot(void (RECEIVER::*method)()) : _slot_proxy(this), _method(method) { }

		void connect(QObject const *src_obj, char const *signal)
		{
			QObject::connect(src_obj, signal, _slot_proxy, SLOT(invoke()));
		}

		void invoke()
		{
			QMetaObject::invokeMethod(_slot_proxy, "invoke", Qt::QueuedConnection);
		}
};


/**
 * Connect a Qt4 signal with a 'Slot'
 *
 * This functor connects a Qt4 signal source to a 'Slot' represented by the
 * 'ST' template argument.
 *
 * Example
 * -------
 *
 * ! connect_to_slot<State_changed>(receiver, button, SIGNAL(clicked()));
 */
template <typename ST>
struct connect_to_slot
{
	template <typename T>
	connect_to_slot(QMember<T> &dst_obj, QObject const *src_obj, char const *signal)
	{
		static_cast<Slot<T, ST> *>(dst_obj)->connect(src_obj, signal);
	}

	template <typename T>
	connect_to_slot(T *dst_obj, QObject const *src_obj, char const *signal)
	{
		static_cast<Slot<T, ST> *>(dst_obj)->connect(src_obj, signal);
	}
};

#endif /* _INCLUDE__QOOST__SLOT_H_ */
