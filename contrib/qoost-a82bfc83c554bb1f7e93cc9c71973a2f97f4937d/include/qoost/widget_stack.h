/*
 * \brief  Transparent stack of widgets in one compound
 * \author Genode Labs <qoost@genode-labs.com>
 * \date   2011-04-19
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

#ifndef _INCLUDE__QOOST__WIDGET_STACK_H_
#define _INCLUDE__QOOST__WIDGET_STACK_H_

#include <QStackedLayout>

struct Widget_stack : Compound_widget<QWidget, QStackedLayout>
{
	Widget_stack() { _layout->setStackingMode(QStackedLayout::StackAll); }

	void add_widget(QWidget *w)
	{
		_layout->addWidget(w);
		_layout->setCurrentWidget(w);
	}
};

#endif /* _INCLUDE__QOOST__WIDGET_STACK_H_ */
