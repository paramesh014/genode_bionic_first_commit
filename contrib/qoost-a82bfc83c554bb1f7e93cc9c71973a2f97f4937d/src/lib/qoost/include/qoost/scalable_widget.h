/*
 * \brief  Scalable and scrollable widgets
 * \author Genode Labs <qoost@genode-labs.com>
 * \date   2011-01-12
 *
 * Copyright (C) 2011 by Genode Labs GmbH
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

#ifndef _INCLUDE__QOOST__SCALABLE_WIDGET_H_
#define _INCLUDE__QOOST__SCALABLE_WIDGET_H_

/* Qt4 includes */
#include <QFrame>

/* Qoost includes */
#include <qoost/widget_tree.h>
#include <qoost/animated_geometry.h>


enum { INVALID_SIZE = 0 };

class Scalable_child : public QFrame
{
	Q_OBJECT

	protected:

		unsigned _start;
		unsigned _length;

	public:

		Scalable_child() {}
		Scalable_child(unsigned start, unsigned length) : _start(start), _length(length) {}

		unsigned  start() { return _start; }
		virtual void set_start(unsigned start) { _start = start; }

		unsigned length() { return _length; };
		void set_length(unsigned length) { _length = length; }

		/* give child classes the opportunity  to overwrite move / pos */
		virtual void   move(int x, int y) { QWidget::move(x, y); }
		virtual QPoint pos () { return QWidget::pos(); }
		virtual int x() { return QWidget::x(); }
};


/**
 * Layout widget that manages multiple items horizontally, item start positions and
 * sizes are scaled according to the widget's size and a limit value (i.e.,
 * maximum number of frames).
 */
class Scalable_hbox : public QFrame
{
	private:

		int _limit;

		struct Update_children
		{
			int _width;
			int _limit;

			Update_children(int width, int limit) : _width(width), _limit(limit) {}

			void operator () (QWidget *w)
			{
				Scalable_child *child = dynamic_cast<Scalable_child *>(w);

				if (!child || !_limit) return;

				int start  = ((qint64)child->start() * _width) / _limit;
				int width  = ((qint64)child->length() * _width) / _limit;

				child->resize(width, child->height());
				child->move(start, 0);
			}
		};

		/* resize and reposition children */
		void _update_children()
		{
			for_widget_tree(this, Update_children(width(), _limit));
		}

	public:

		Scalable_hbox() :  _limit(0) { }

		virtual void add_widget(QWidget *w) { w->setParent(this); w->show(); _update_children(); }

		/* update a single child widget */
		void update_child(QWidget *child)
		{
			Update_children update(width(), _limit);
			update(child);
		}

		/**
		 * Set value that corresponds to 100%
		 */
		void limit(int limit)
		{
			_limit = limit;
			_update_children();
		}

		int limit() { return _limit; }

	protected:

		void resizeEvent(QResizeEvent *) { _update_children(); }
};


/**
 * Layout widget that manages one potentially large content widget,
 * i.e., 'Scalable_hbox'
 *
 * Manages:
 *
 * - Width of content (-> zoom)
 * - Horizontal position of content (-> scroll)
 * - Selection
 * - Cursor
 */
class Scroll_view : public QWidget
{
	Q_OBJECT

	protected:

		QWidget           *_content;
		Animated_geometry *_geometry;

	public:

		Scroll_view() : _content(0), _geometry(0) {}

		void assign_content(QWidget *content)
		{
			_content = content;
			_content->setParent(this);
			_content->show();

			_geometry = new Animated_geometry(_content, 200);
		}

		QSize sizeHint() const
		{
			return QSize(INVALID_SIZE, _content->sizeHint().height());
		}

		QSize minimumSizeHint() const
		{
			return _content->minimumSize().isValid()
			     ? QSize(INVALID_SIZE, _content->minimumSize().height())
			     : QSize(INVALID_SIZE, _content->minimumSizeHint().height());
		}

		QWidget *content() { return _content; }

	protected:

		void resizeEvent(QResizeEvent *)
		{
			if (_content->width() < width()) {
				_content->resize(width(), height());
				_geometry->reset();
			}

			Q_EMIT geometry_changed(-_content->x(), _content->width());
		}

	public Q_SLOTS:

		/* update child position */
		void position_update(int x, bool animate = false)
		{
			if (animate) {
				_geometry->move(-x, 0);
				_geometry->animate();
			}
			else
				_content->move(-x, 0);
		}

		/* update size of child */
		void geometry_update(int x, int width)
		{
			_geometry->move(-x, 0, width, _content->height()); 
			_geometry->animate();
		}

	Q_SIGNALS:

		/* position of child has changed */
		void position_changed(int x);

		/* sync with other boxes */
		void sync_position(int x, bool animate = false);

		/* promote resize event */
		void geometry_changed(int x, int width);
};

#endif /* _INCLUDE__QOOST__SCALABLE_WIDGET_H_ */
