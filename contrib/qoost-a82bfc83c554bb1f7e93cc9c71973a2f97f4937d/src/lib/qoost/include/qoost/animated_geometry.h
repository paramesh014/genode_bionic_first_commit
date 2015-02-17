/*
 * \brief  Utility for animating the geometry of a widget
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

#ifndef _INCLUDE__QOOST__ANIMATED_GEOMETRY_H_
#define _INCLUDE__QOOST__ANIMATED_GEOMETRY_H_

#include <QPropertyAnimation>

/**
 * Helper class for managing the geometry animation of a widget
 */
class Animated_geometry : public QPropertyAnimation
{
	private:

		QWidget * const _widget;
		int             _duration;
		QRect           _next_end_value;

	public:

		/**
		 * Constructor
		 *
		 * \param widget    widget to control
		 * \param duration  animation duration in milliseconds
		 */
		Animated_geometry(QWidget * const widget, int duration = 0)
		:
			_widget(widget), _duration(duration),
			_next_end_value(_widget->geometry())
		{
			setPropertyName("geometry");
			setTargetObject(_widget);
			setEasingCurve(QEasingCurve::InOutQuad);
		}

		/**
		 * Set duration of animatio in milliseconds
		 */
		void duration(int duration) { _duration = duration; }

		/**
		 * Get maximum height of the associated widget
		 */
		int  maximumHeight() const { return _widget->maximumHeight(); }

		/**
		 * Start animated movement of widget to specified position
		 */
		void move(int x, int y)
		{
			move(x, y, _widget->width(), _widget->height());
		}

		/**
		 * Start animated movement of widget to specified position and size
		 */
		void move(int x, int y, int w, int h)
		{
			_next_end_value = QRect(x, y, w, h);
		}

		/**
		 * Use the widget's current geometry as target geometry
		 */
		void reset() { _next_end_value = _widget->geometry(); }

		/**
		 * Start animating the widget's position
		 */
		void animate(bool restart = true)
		{
			/* set up animation */
			setStartValue(_widget->geometry());
			setEndValue(_next_end_value);

			if (restart) {
				stop();
				setDuration(_duration);
			}

			start();
		}
};

#endif /* _INCLUDE__QOOST__ANIMATED_GEOMETRY_H_ */
