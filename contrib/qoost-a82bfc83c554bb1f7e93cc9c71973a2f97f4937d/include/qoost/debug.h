/*
 * \brief  Debugging helpers
 * \author Genode Labs <qoost@genode-labs.com>
 * \date   2011-03-03
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

#ifndef _INCLUDE__QOOST__DEBUG_H_
#define _INCLUDE__QOOST__DEBUG_H_

/* Qt4 includes */
#include <QDebug>
#include <QString>


#define PRINT_WIDGET_SIZES(w)                                  \
	do {                                                       \
		qDebug() << "Widget:" << w << "\n"                     \
		         << "   SH:" << (w)->sizeHint() << "\n"        \
		         << "  MSH:" << (w)->minimumSizeHint() << "\n" \
		         << "  MIN:" << (w)->minimumSize() << "\n"     \
		         << "  MAX:" << (w)->maximumSize() << "\n"     \
		         << " GEOM:" << (w)->geometry();               \
	} while (0)

#define PRINT_LAYOUT_SIZES(l)                              \
	do {                                                   \
		qDebug() << "Layout:" << l << "\n"                 \
		         << "   SH:" << (l)->sizeHint() << "\n"    \
		         << "  MIN:" << (l)->minimumSize() << "\n" \
		         << "  MAX:" << (l)->maximumSize() << "\n" \
		         << " GEOM:" << (l)->geometry();           \
	} while (0)


static void print_children(QObject *parent, int indent = 0)
{
	qDebug() << qPrintable(QString().fill(' ', 2 * indent))
	         << parent;
	Q_FOREACH(QObject *c, parent->children())
		print_children(c, indent + 1);
}

#endif /* _INCLUDE__QOOST__DEBUG_H_ */
