/*
 * \brief  Utilities for working with style sheets
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

#ifndef _INCLUDE__QOOST__STYLE_H_
#define _INCLUDE__QOOST__STYLE_H_

/* Qt4 includes */
#include <QWidget>
#include <QStyle>        /* for dereferencing style() */
#include <QApplication>  /* for sendEvent() */

/**
 * Re-apply style to specified widget and its children
 */
static inline void reapply_style_recursive(QWidget *w)
{
	w->style()->polish(w);

	QEvent e(QEvent::StyleChange);
	QApplication::sendEvent(w, &e);

	Q_FOREACH(QObject *child, w->children()) {
		QWidget *c = qobject_cast<QWidget *>(child);
		if (c)
			reapply_style_recursive(c);
	}
}


/**
 * The qss file refers to QWidget styles by using the object name as style
 * id. Therefore, different widget states can be specifically styled by
 * assigning a new style id. However, to reflect the style change on screen,
 * the style sheet must be re-applied to the widget and its  children. This
 * function takes care of these steps.
 */
static inline void update_style_id(QWidget *w, QString const &style_id)
{
	w->setObjectName(style_id);
	reapply_style_recursive(w);
}

#endif /* _INCLUDE__QOOST__STYLE_H_ */
