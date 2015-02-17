/*
 * \brief  Form field for dynamic dialogs
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

#ifndef _INCLUDE__QOOST__FORM_FIELD_H_
#define _INCLUDE__QOOST__FORM_FIELD_H_

/* Qt4 includes */
#include <QLabel>
#include <QDebug>

/* Qoost includes */
#include <qoost/slot.h>
#include <qoost/translatable.h>
#include <qoost/compound_widget.h>


/**
 * Functor interface for querying hint text as used by 'Form_field'
 */
struct Hint_functor_base
{
	virtual QString operator() () const = 0;
	virtual ~Hint_functor_base() { }
};


/**
 * Functor that calls a pointer-to-member function to query hint text
 */
template <typename T>
struct Hint_functor : public Hint_functor_base
{
	QString (T::*_member) () const;
	T *_obj;

	Hint_functor(QString (T::*member) () const, T *obj) : _member(member), _obj(obj) { }

	QString operator() () const { return (_obj->*_member)(); }
};


/**
 * Slot type used by 'Form_field'
 */
struct Update_hint { };


/**
 * Widget type used for the form field name
 */
class Form_field_name : public Translatable<QLabel> { Q_OBJECT };


/**
 * Widget type used for the form field hint text
 */
class Form_field_hint : public Translatable<QLabel> { Q_OBJECT };


/**
 * Form field for dynamic dialogs
 *
 * The 'Form_field' displays a 'WIDGET' accompanied with a 'name' label and
 * a hint text. The 'name' holds the description of the form field and is
 * normally displayed on top of the actual field. The hint text displayed
 * below the field provides the user with assistance while filling out the
 * field. In contrast to the static 'name', the hint text is dynamic and
 * depends on the current state of the form field and potentially other context
 * information.  Hence, the hint text policy is provided by the creator of
 * the 'Form_field' that knows about the relevant context.
 *
 * The 'Form_field' provides a 'Update_hint' slot, which triggers the
 * update of the hint text. When using an entry field a 'WIDGET', this slot
 * can be connected to the 'textChanged' signal. This way, the hint text
 * gets updated with each key stroke.
 */
template <typename WIDGET>
class Form_field : public Compound_widget<QWidget, QVBoxLayout>,
                   public Slot<Form_field<WIDGET>, Update_hint>
{
	private:

		QMember<Form_field_name> _name;
		QMember<WIDGET>          _widget;
		QMember<Form_field_hint> _hint;
		Hint_functor_base       *_hint_text;

		/**
		 * Slot function for 'Update_hint' signals
		 */
		void _update_hint() { _hint->setText((*_hint_text)()); }

	public:

		/**
		 * Constructor
		 *
		 * \param HINT_POLICY  class that implements the 'hint_text' function
		 *                     for the form field
		 * \param hint_text    pointer-to-member function that providing the
		 *                     hint text to be displayed below the form field
		 * \param hint_policy  instance of 'HINT_POLICY' to be used for calling
		 *                     'text_hint'
		 */
		template <typename HINT_POLICY>
		Form_field(QString (HINT_POLICY::*hint_text) () const, HINT_POLICY *hint_policy)
		:
			Slot<Form_field<WIDGET>, Update_hint> (&Form_field<WIDGET>::_update_hint),
			_hint_text(new Hint_functor<HINT_POLICY>(hint_text, hint_policy))
		{
			_layout->addWidget(_name);
			_layout->addWidget(_widget);
			_layout->addWidget(_hint, 0, Qt::AlignHCenter);

			_name->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
			_hint->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

			Slot<Form_field<WIDGET>, Update_hint>::invoke();
		}

		virtual ~Form_field() { }

		void name(QString const &name) { _name->setText(name); }

		WIDGET       *widget()       { return _widget; }
		WIDGET const *widget() const { return _widget; }
};

#endif /* _INCLUDE__QOOST__FORM_FIELD_H_ */
