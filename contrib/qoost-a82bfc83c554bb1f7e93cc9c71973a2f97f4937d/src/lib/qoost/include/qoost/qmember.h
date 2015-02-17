/*
 * \brief  Helper for aggregating QObjects in one class
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

#ifndef _INCLUDE__QOOST__QMEMBER_H_
#define _INCLUDE__QOOST__QMEMBER_H_

/* Qt4 includes */
#include <QObject>  /* for 'QObject::deleteLater()' */

/* Qoost includes */
#include <qoost/noncopyable.h>


/**
 * Smart pointer, which manages QObject life time (allocation/deallocation)
 *
 * With Qt4, QObjects must not be aggregated in a aggregating object as member
 * variables. Qt4 expects each QObject to be created via the 'new' operator.
 * Because the framework manages the life time of QObjects (via the parent-child
 * relationship), it invokes the 'delete' operator. However, this operator
 * cannot be called for pointers to members or static objects. The QMember
 * class solves the inconvenience of constructing QObject members via 'new' by
 * hand. For example, by using QMember, the following compound class:
 *
 * ! class Compound
 * ! {
 * !   private:
 * !     QLabel *_label_1, *_label_2;
 * !
 * !   public:
 * !     Compound() :
 * !       _label_1(new QLabel()),
 * !       _label_2(new QLabel())
 * !     { }
 * !
 * !     ~Compound() {
 * !       _label_2->deleteLater();
 * !       _label_1->deleteLater();
 * !     }
 * ! };
 *
 * becomes
 *
 * ! class Compound
 * ! {
 * !   private:
 * !     QMember<QLabel> _label_1, _label_2;
 * ! };
 */
template <typename T>
struct QMember : Noncopyable
{
	T *_ptr;

	/** Default constructor */
	QMember() : _ptr(new T()) { }

	/** One-argument constructor */
	template <typename AT> QMember(AT       &arg) : _ptr(new T(arg)) { }
	template <typename AT> QMember(AT const &arg) : _ptr(new T(arg)) { }

	/** Two-argument constructor */
	template <typename AT0, typename AT1> QMember(AT0       &arg0, AT1       &arg1) : _ptr(new T(arg0, arg1)) { }
	template <typename AT0, typename AT1> QMember(AT0 const &arg0, AT1       &arg1) : _ptr(new T(arg0, arg1)) { }
	template <typename AT0, typename AT1> QMember(AT0       &arg0, AT1 const &arg1) : _ptr(new T(arg0, arg1)) { }
	template <typename AT0, typename AT1> QMember(AT0 const &arg0, AT1 const &arg1) : _ptr(new T(arg0, arg1)) { }

	~QMember() { _ptr->deleteLater(); }

	T *      operator -> ()       { return _ptr; }
	T const* operator -> () const { return _ptr; }

	operator T *      ()       { return _ptr; }
	operator T const* () const { return _ptr; }
};

#endif /* _INCLUDE__QOOST__QMEMBER_H_ */
