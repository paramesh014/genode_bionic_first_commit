/*
 * \brief  Widget with associated layout
 * \author Genode Labs <qoost@genode-labs.com>
 * \date   2011-02-18
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

#ifndef _INCLUDE__QOOST__COMPOUND_WIDGET_H_
#define _INCLUDE__QOOST__COMPOUND_WIDGET_H_

/* Qt4 includes */
#include <QLayout>

/* Qoost includes */
#include <qoost/qmember.h>

/**
 * The 'Compound_widget' template is a utility to avoid code duplications in
 * the common use case of hosting static a number of child widgets in a
 * compound parent widget.
 */
template <typename WIDGET, typename LAYOUT, int SPACING = 0>
class Compound_widget : public WIDGET
{
	private:

		void _init()
		{
			_layout->setSpacing(SPACING);
			_layout->setContentsMargins(0, 0, 0, 0);
			WIDGET::setLayout(_layout);
		}

	protected:

		QMember<LAYOUT> _layout;

	public:

		/** Default constructor */
		Compound_widget() { _init(); }

		/** One-argument constructor */
		template <typename AT> Compound_widget(AT       &arg) : WIDGET(arg) { _init(); }
		template <typename AT> Compound_widget(AT const &arg) : WIDGET(arg) { _init(); }

		QSize sizeHint() const
		{
			return WIDGET::sizeHint().expandedTo(_layout->sizeHint());
		}

		QSize minimumSizeHint() const
		{
			return WIDGET::minimumSizeHint().expandedTo(_layout->minimumSize());
		}
};

#endif /* _INCLUDE__QOOST__COMPOUND_WIDGET_ */
