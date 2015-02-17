/*
 * \brief  Mark class noncopyable
 * \author Genode Labs <qoost@genode-labs.com>
 * \date   2011-04-05
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

#ifndef _INCLUDE__QOOST__NONCOPYABLE_H_
#define _INCLUDE__QOOST__NONCOPYABLE_H_

class Noncopyable
{
	protected:

		/*
		 * Prevent direct construction and destruction of this type - only
		 * allowed from subclasses.
		 */
		Noncopyable() { }
		~Noncopyable() { }

	private:

		/*
		 * Prevent copy construction and assignment completely.
		 */
		Noncopyable(Noncopyable const &);
		Noncopyable & operator=(Noncopyable const &);
};

#endif /* _INCLUDE__QOOST__NONCOPYABLE_H_ */
