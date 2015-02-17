/*
 * \brief  Controller for widget states
 * \author Genode Labs <qoost@genode-labs.com>
 * \date   2011-02-28
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

#ifndef _INCLUDE__QOOST__WIDGET_STATE_CONTROLLER_H_
#define _INCLUDE__QOOST__WIDGET_STATE_CONTROLLER_H_

/* Qt4 includes */
#include <QWidget>
#include <QList>


template <typename FORM, typename SET_STATE>
class Widget_state_controller
{
	private:

		struct Widget_state_checker
		{
			QWidget *_widget;

			/* provided by 'FORM' */
			bool (FORM::*_check_state) () const;

			void update_state(FORM *form)
			{
				SET_STATE()(_widget, (form->*_check_state)());
			}

			Widget_state_checker(QWidget *widget, bool (FORM::*check_state)() const)
			: _widget(widget), _check_state(check_state) { }
		};

		QList<Widget_state_checker> _state_checkers;

		FORM *_form() { return static_cast<FORM *>(this); }

	public:

		/**
		 * Add state checker for the specified widget
		 */
		void add_state_checker(QWidget *widget, bool (FORM::*check_state)() const)
		{
			Widget_state_checker sc(widget, check_state);
			sc.update_state(_form());
			_state_checkers << sc;
		}

		/**
		 * Evaluate all state checkers
		 */
		void update_state()
		{
			Q_FOREACH(Widget_state_checker sc, _state_checkers)
				sc.update_state(_form());
		}
};


/***********************************
 ** Convenience state controllers **
 ***********************************/

struct Set_visibility_state {
	void operator() (QWidget *widget, bool state) {
		widget->setVisible(state); } };


template <typename FORM>
class Widget_visibility_controller :
	public Widget_state_controller<FORM, Set_visibility_state> { };


struct Set_enabled_state {
	void operator() (QWidget *widget, bool state) {
		widget->setEnabled(state); } };


template <typename FORM>
class Widget_enabled_controller :
	public Widget_state_controller<FORM, Set_enabled_state> { };

#endif /* _INCLUDE__QOOST__WIDGET_STATE_CONTROLLER_H_ */
