/*
 * \brief  Utilities for working with trees of QWidgets
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

#ifndef _INCLUDE__QOOST__WIDGET_TREE_H_
#define _INCLUDE__QOOST__WIDGET_TREE_H_

#include <QWidget>

/**
 * Apply 'FUNCTOR' to each child within the specified widget tree
 *
 * The functor takes the QWidget pointer of the widget to process as first
 * argument.
 */
template <typename FUNCTOR>
static inline void for_widget_tree(QWidget *widget, FUNCTOR func)
{
	Q_FOREACH(QObject *child, widget->children()) {
		QWidget *c = qobject_cast<QWidget *>(child);
		if (c)
			for_widget_tree(c, func);
	}
	func(widget);
}


/**
 * Return the immediate child of 'parent' that contains the widget 'w' within
 * its subtree
 */
static inline QWidget *immediate_child_containing(QWidget *parent, QWidget *w)
{
	for (; w && w->parent(); w = qobject_cast<QWidget *>(w->parent()))
		if (w->parent() == parent)
			return w;

	return 0;
}


/**
 * Return parent of class type CLASS of widget 'w'
 */
template <typename CLASS>
static inline CLASS *find_parent_class(QWidget *w)
{
	for (; w && w->parent(); w = qobject_cast<QWidget *>(w->parent())) {
		CLASS *c = dynamic_cast<CLASS *>(w);
		if (c) return c;
	}

	return 0;
}


/**
 * Find first child of class CLASS in widget tree
 */
template <typename CLASS>
static inline CLASS *find_child_class(QWidget *w)
{
	Q_FOREACH(QObject *child, w->children()) {
		CLASS *c = dynamic_cast<CLASS *>(child);
		if (c)
			return c;

		QWidget *w = qobject_cast<QWidget *>(child);
		if (!w)
			continue;

		if((c = find_child_class<CLASS>(w)))
			return c;
	}

	return 0;
}

/**
 * Find child of type CLASS at global pos
 */
template <typename CLASS>
static inline CLASS *find_child_at(QWidget *w, const QPoint &pos)
{
	QList<CLASS *>list = w->findChildren<CLASS *>();

	while (!list.isEmpty()) {
		CLASS *p = list.takeFirst();
		QPoint child_pos = p->mapFromGlobal(pos);

		if (p->rect().contains(child_pos))
			return p;
	}

	return 0;
}


/**
 * Remove widget from widget tree
 *
 * To prevent a top-level widget from popping up, the widget has to be hidden
 * before unsetting the parent.
 */
static inline void unparent_widget(QWidget *w)
{
	w->hide();
	w->setParent(0);
}


/**
 * Safe deletion of a widget
 *
 * Unsetting the parent before deleteLater() seems to be crucial. Otherwise, a
 * null pointer is dereferenced on next repolish. I do not know why...
 */
static inline void delete_widget(QWidget *w)
{
	unparent_widget(w);
	w->deleteLater();
}

#endif /* _INCLUDE__QOOST__WIDGET_TREE_H_ */
