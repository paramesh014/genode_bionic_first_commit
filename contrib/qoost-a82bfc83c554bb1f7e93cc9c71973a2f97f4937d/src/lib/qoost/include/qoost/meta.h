/*
 * \brief  Meta-programmed helpers
 * \author Genode Labs <qoost@genode-labs.com>
 * \date   2011-03-18
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

#ifndef _INCLUDE__QOOST__META_H_
#define _INCLUDE__QOOST__META_H_

#include <QDebug>


/***********************
 ** Type-list support **
 ***********************/

/**
 * Recursion terminator for type list (i.e., last element)
 */
struct Empty { };


/**
 * Element of recursive type list
 */
template <typename HEAD, typename TAIL>
struct Type_tuple
{
	typedef HEAD Head;
	typedef TAIL Tail;
};


/******************
 ** Type mapping **
 ******************/

/**
 * Association of two types (i.e., one line in the type map)
 */
template <typename T1, typename T2>
struct Type_association
{
	typedef T1 T1_type;
	typedef T2 T2_type;
};


/**
 * Type-map iteration helper class
 */
template <typename TYPE_MAP, typename FUNC>
struct _for_each_type_association
{
	private:

		template <typename T1, typename T2>
		struct _call_functor
		{
			_call_functor(T1 *a, T2 *b, FUNC const &f) { f(a,b);}
		};

	public:

		template <typename T1_BASE, typename T2_BASE>
		_for_each_type_association(T1_BASE *t1, T2_BASE *t2, FUNC const &func)
		{
			typedef typename TYPE_MAP::Head::T1_type T1_type;
			typedef typename TYPE_MAP::Head::T2_type T2_type;

			T1_type *typed_t1 = dynamic_cast<T1_type *>(t1);
			T2_type *typed_t2 = dynamic_cast<T2_type *>(t2);
			if (typed_t1)
				_call_functor<T1_type, T2_type>(typed_t1, typed_t2, func);
			else
				_for_each_type_association<typename TYPE_MAP::Tail, FUNC>(t1, t2, func);
		}
};


/**
 * Type-map iteration termination as specialized template
 *
 * Correct users will never call this contructor in their programs. Buggy
 * programs will print an error message here.
 */
template <typename FUNC>
struct _for_each_type_association<Empty, FUNC>
{
	template <typename T1_BASE, typename T2_BASE>
	_for_each_type_association(T1_BASE *t1, T2_BASE *t2, FUNC const &)
	{
		qDebug() << "Type of" << t1
		         << "not associated with type of" << t2;
	}
};


/**
 * Call functor for specific type association given base pointers
 *
 * The type map is a type list constructed of nested Type_tuple templates using
 * type associations. For example we use the two base classes 'Left' and
 * 'Right':
 *
 * ! struct Left { }; struct Right { };
 *
 * Also, we need specific subtypes for categories on the left and on the right
 * side (e.g., platform type and corresponding widget):
 *
 * ! struct Left_1 : Left; struct Right_1 : Right;
 * ! struct Left_2 : Left; struct Right_2 : Right;
 * ! ...
 *
 * The associations of our types are put into the type map, which is terminated
 * by 'Empty':
 *
 * ! typedef Type_association<Left_1, Right_1> Assoc_1;
 * ! typedef Type_association<Left_2, Right_2> Assoc_2;
 * ! typedef Type_tuple<Assoc_1,
 * !         Type_tuple<Assoc_2,
 * !         ...
 * !         Empty> ... > Type_map;
 *
 * The functor implements the '()' operator for the specific type(s):
 *
 * ! struct Functor
 * ! {
 * !   void operator () (Left_1 *l, Right_1 *r) const { ... }
 * !   void operator () (Left_2 *l, Right_2 *r) const { ... }
 * !   ...
 * ! };
 *
 * Now the map may be used to call a functor with the actual typed pointers for
 * two base class pointers according to the type map:
 *
 * ! Left *l = new Left_1; Right *r = new Left_2;
 * ! for_each_type_association<Type_map>(l, r, Functor());
 */
template <typename TYPE_MAP, typename T1_BASE, typename T2_BASE, typename FUNC>
void for_each_type_association(T1_BASE *t1, T2_BASE *t2, FUNC const &func)
{
	_for_each_type_association<TYPE_MAP, FUNC>(t1, t2, func);
}

#endif /* _INCLUDE__QOOST__META_H_ */
