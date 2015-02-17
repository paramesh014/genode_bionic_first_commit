/*
 * \brief  Validator for IP addresses
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

#ifndef _INCLUDE__QOOST__VALIDATE_IP_ADDRESS_H_
#define _INCLUDE__QOOST__VALIDATE_IP_ADDRESS_H_

#include <QValidator>

class Ip_address_validator : public QValidator
{
	private:

		/**
		 * Return true if string list contains no empty elements and
		 * has the specified number of elements.
		 */
		bool _field_list_complete(const QStringList &list,
		                          int expected_length) const
		{
			if (list.count() != expected_length)
				return false;

			for (int i = 0; i < list.count(); i++)
				if (list[i].length() == 0)
					return false;

			return true;
		}

		/**
		 * Return true if string contains a value in the specified range
		 * or no value at all
		 */
		bool _value_in_range(const QString &string, int min, int max) const
		{
			if (string.length()) {
				bool ok = false;
				int value = string.toInt(&ok);
				if (!ok || value < min || value > max)
					return false;
			}
			return true;
		}

	public:

		void fixup(QString &) const { }

		State validate(QString &input, int &) const
		{
			QStringList ip_netmask = input.split("/");

			/*
			 * Check for invalid input
			 */
			if (ip_netmask.count() > 2)
				return Invalid;

			/* validate field values of IP address */
			QStringList addr_fields = ip_netmask[0].split(".");

			/* do not accept more than 4 fields */
			if (addr_fields.count() > 4)
				return Invalid;

			/* only accept numbers between 0 and 255 as field values */
			for (int i = 0; i < addr_fields.count(); i++)
				if (!_value_in_range(addr_fields[i], 0, 255))
					return Invalid;

			/* validate range of netmask value */
			if (ip_netmask.count() > 1
			 && !_value_in_range(ip_netmask[1], 0, 32))
				return Invalid;

			/*
			 * Check for completeness
			 */
			if (!_field_list_complete(ip_netmask, 2)
			 || !_field_list_complete(addr_fields, 4))
				return Intermediate;

			return Acceptable;
		}
};

#endif /* _INCLUDE__QOOST__VALIDATE_IP_ADDRESS_H_ */
