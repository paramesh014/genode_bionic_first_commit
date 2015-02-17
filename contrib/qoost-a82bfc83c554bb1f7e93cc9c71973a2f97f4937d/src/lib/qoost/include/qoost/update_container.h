/*
 * \brief  Helper for synchronizing two Qt4 containers
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

#ifndef _INCLUDE__QOOST__UPDATE_CONTAINER_H_
#define _INCLUDE__QOOST__UPDATE_CONTAINER_H_


/**
 * Update output container from input container according to a policy
 *
 * Often when implementing adapters, it is inevitable to synchronize containers
 * on both sides of the adapter in the event of changes. The logic to achieve
 * this is always the same:
 *
 * 1) Remove elements from the output container that no longer reside in the
 *    input container.
 * 2) Update the remaining output container elements from the according
 *    elements of the input container.
 * 3) Add new elements to the output container that where added to the input
 *    container.
 * 4) Adjust output container order to the input container.
 *
 * On the other hand, the policy, which elements of both containers are related
 * and how adding, removing, and sorting is conducted, is fairly application
 * specific.
 *
 * The 'update_container' template function implements the generic logic and
 * utilizes a user-provided policy object for conducting application-specific
 * operations.
 *
 * Example policy class
 * --------------------
 *
 * ! struct Policy
 * ! {
 * !   typedef ... In;
 * !   typedef ... Out;
 *
 * The policy class must define the output and input container types, e.g.
 * 'QList<QWidget *>'.
 *
 * !   bool match(In::value_type const input_elem, Out::value_type const output_elem)
 *
 * This function is called to match elements of both containers and used as
 * invariant for conducting further operations, e.g. if both match update
 * output from input.
 *
 * !   void update(In::value_type const input_elem, Out::value_type output_elem)
 *
 * 'update' is called to synchronize two related elements of the containers.
 *
 * !   void add_new(In::value_type const input_elem)
 * !
 * !   void remove_old(Out::value_type output_elem)
 *
 * These function are called in the event of redundant or missing elements in
 * the ouput container.
 *
 * !   void rearrange(Out::value_type elem, Out::value_type successor)
 *
 * The last operation during 'update_container' is sorting the output container
 * according to the input container. Therefore, rearrangement is carried out
 * from begin to end of the container by putting elements in front of others.
 * Incidentally, this operation is finished on the next to last element, so no
 * element has to be moved to the end of the container.
 *
 * ! }
 *
 * Additionaly, policy classes may contain further context-specific members,
 * e.g. references to the original containers.
 */

template <typename POLICY>
static void update_container(POLICY                    &policy,
                             typename POLICY::In const &input_cont,
                             typename POLICY::Out      &output_cont)
{
	typename POLICY::In::const_iterator in;
	typename POLICY::Out::iterator      out;

	/*
	 * Remove old elements from output container
	 *
	 * To avoid problems caused by concurrently manipulating and iterating the
	 * 'output_cont', we reset the iteration each time, when removing a
	 * container entry.
	 */
	for (;;) {
		for (out = output_cont.begin(); out != output_cont.end(); ++out) {
			bool found = false;
			for (in = input_cont.begin(); in != input_cont.end(); ++in)
				if ((found = policy.match(*in, *out)))
					break;
			if (!found) {
				policy.remove_old(*out);
				break;
			}
		}
		if (out == output_cont.end())
			break;
	}

	/* update elements in output container */
	for (out = output_cont.begin(); out < output_cont.end(); ++out)
		for (in = input_cont.begin(); in < input_cont.end(); ++in)
			if (policy.match(*in, *out))
				policy.update(*in, *out);

	/* add new elements */
	for (in = input_cont.begin(); in < input_cont.end(); ++in) {
		bool found = false;
		for (out = output_cont.begin(); out < output_cont.end(); ++out)
			if ((found = policy.match(*in, *out)))
				break;
		if (!found)
			policy.add_new(*in);
	}

	/* sort container */
	for (int i = 0; i < input_cont.count(); ++i)
		if (!policy.match(input_cont.at(i), output_cont.at(i)))
			for (int j = i; j < output_cont.count(); ++j) {
				if (policy.match(input_cont.at(i), output_cont.at(j))) {
					policy.rearrange(output_cont.at(j), output_cont.at(i));
					break;
				}
			}
}

#endif /* _INCLUDE__QOOST__UPDATE_CONTAINER_H_ */
