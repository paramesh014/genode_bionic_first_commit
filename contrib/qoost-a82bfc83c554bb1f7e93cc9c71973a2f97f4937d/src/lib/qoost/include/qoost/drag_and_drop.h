/*
 * \brief  Drag-and-drop helper
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

#ifndef _INCLUDE__QOOST__DRAG_AND_DROP_H_
#define _INCLUDE__QOOST__DRAG_AND_DROP_H_

/* Qt4 includes */
#include <QWidget>

/**
 * Marker for widgets that are subject of drag-and-drop handling
 *
 * Each widget to be used as handle for drag-and-drop operations, i.e.,
 * organized in a 'Sorted_item_browser' must inherit this class. The
 * organizing widget uses this as criterion to decide whether to
 * catch events (via an event filter) of a child or not.
 */
class Draggable { };

/**
 * Functor for adding an event filter to a widget
 */
struct Install_event_filter_to_draggable
{
	QWidget *_ef;

	Install_event_filter_to_draggable(QWidget *ef): _ef(ef) { }

	void operator () (QWidget *w)
	{
		if (w->inherits("Draggable"))
			w->installEventFilter(_ef);
	}
};

#endif /* _INCLUDE__QOOST__DRAG_AND_DROP_H_ */
