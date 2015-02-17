/*
 * \brief  Tweaks for plastique style
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

#ifndef _INCLUDE__QOOST__STYLE_WEAKS_H_
#define _INCLUDE__QOOST__STYLE_WEAKS_H_

#include <QProxyStyle>

class Style_tweaks : public QProxyStyle
{
	public:

		void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
		                   QPainter *painter, const QWidget *widget) const
		{
			/* do not draw focus rectangles - this permits modern styling */
			if (element == QStyle::PE_FrameFocusRect)
				return;

			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}

		int styleHint(StyleHint hint, const QStyleOption *option,
		              const QWidget *widget, QStyleHintReturn *returnData) const
		{
			/* reduce delay before a tool-button menu pops up on long mouse press */
			if (hint == QStyle::SH_ToolButton_PopupDelay)
				return 400;

			return QProxyStyle::styleHint(hint, option, widget, returnData);
		}
};

#endif /* _INCLUDE__QOOST__STYLE_TWEAKS_H_ */
