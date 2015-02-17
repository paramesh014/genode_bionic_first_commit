/*
 * \brief  Custom event class
 * \author Genode Labs <qoost@genode-labs.com>
 * \date   2011-03-01
 *
 * Copyright (C) 2011 by Genode Labs GmbH
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

#ifndef _INCLUDE__QOOST__CUSTOM_EVENT_H_
#define _INCLUDE__QOOST__CUSTOM_EVENT_H_

#include <QEvent>


/**
 * Custom event template that registers one event type per CLASS
 */
template <typename CLASS>
class Custom_event : public QEvent
{
	private:

		/* register event type only once per CLASS */
		template <typename SINGELTON> 
		static QEvent::Type _type()
		{
			static QEvent::Type type = static_cast<QEvent::Type>(QEvent::registerEventType());
			return type;
		}

	public:

		static QEvent::Type type() { return Custom_event::_type<CLASS>(); }

		Custom_event() : QEvent(Custom_event::type()) {}
		virtual ~Custom_event() {}
};

#endif /* _INCLUDE__QOOST__CUSTOM_EVENT_H_ */
