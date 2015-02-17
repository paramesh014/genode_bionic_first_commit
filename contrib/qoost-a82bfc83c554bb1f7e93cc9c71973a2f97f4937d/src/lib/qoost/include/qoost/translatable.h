/*
 * \brief  Helper for dynamic translation of 'text' property
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

#ifndef _INCLUDE__QOOST__TRANSLATABLE_H_
#define _INCLUDE__QOOST__TRANSLATABLE_H_

/* Qt4 includes */
#include <QCoreApplication>
#include <QWidget>
#include <QStringList>

/* Qoost includes */
#include <qoost/tooltip.h>


/**
 * Global translation marker for lupdate
 *
 * Fortunately, 'TR' is recognized by lupdate but not implemented in Qt
 * headers. We have to avoid calling 'tr' because 'setText' would never see the
 * original tag.
 */
static inline QString TR(const char *text) { return text; }


/**
 * Marker for class that is a translation context
 *
 * Classes (public) inheriting from this marker terminate the translation
 * lookup from children and, thereby, define the context in which text and
 * tooltip properties are translated. Do not forget to declare 'Q_OBJECT' in
 * the class.
 */
class Translation_context { };


/**
 * Wrapper template to enhance Qt4 widgets with automatic translation support
 *
 * This template can be instantiated for Qt4 widgets and enables transparent
 * re-translation for text-based attributes, namely 'text' and 'tooltip'. It
 * utitlizes the standard accessor functions for these attributed, e.g.,
 * setToolTip().
 *
 * Combined with the empty 'TR' macro, the 'Translatable' class template
 * expects the text attribute to be set to untranslated text, called
 * translation tag. It keeps this tag internally, translates it into the
 * current language, and passes the translated text to the original widget. If
 * the language is changed, each 'Translatable' widget responds to the
 * corresponding 'LanguageChange' event by re-translating the stored tag. The
 * translation context is determined by traversing the parent-child hierarchy
 * upwards until an object inheriting 'Translation_context' is found.
 *
 * Dynamic text parts are supported by providing extended versions of the
 * text-set functions. The implementation of these functions accepts an
 * additional parameter, which is an optional list of arguments for string
 * marker substitutions.
 *
 * Consequently, using the 'Translatable' class template eliminates the need
 * for explicitly storing widget pointers of all to-be-translated widgets and
 * manually implementing 'retranslateUi'-like functionality.
 */
template <typename WIDGET>
class Translatable : public WIDGET
{
	private:

		QString     _text_tag;
		QStringList _text_args;
		QString     _tool_tip_tag;
		QStringList _tool_tip_args;

		QObject    *_context;

		/**
		 * Look for translation-context marker up the widget tree
		 */
		QObject * _lookup_context()
		{
			for (QObject *p = this; p; p = p->parent())
				if (p->inherits("Translation_context"))
					return p;
			return 0;
		}

		const char *_context_name()
		{
			_context = _lookup_context();
			if (_context)
				return _context->metaObject()->className();
			else
				return "Global";
		}

		QString _translate(QString const &tag, QStringList const &args = QStringList())
		{
			QString translated(QCoreApplication::translate(_context_name(), tag.toLatin1()));

			Q_FOREACH(QString arg, args)
				translated = translated.arg(arg);

			return translated;
		}

		void _on_event()
		{
			if (!_text_tag.isEmpty())
				setText(_text_tag, _text_args);
			if (!_tool_tip_tag.isEmpty())
				setToolTip(_tool_tip_tag, _tool_tip_args);
		}

	protected:

		void changeEvent(QEvent *event)
		{
			if (event->type() == QEvent::LanguageChange)
				_on_event();
			WIDGET::changeEvent(event);
		}

		void showEvent(QShowEvent *event)
		{
			if (_context != _lookup_context())
				_on_event();
			WIDGET::showEvent(event);
		}

	public:

		Translatable() : _context(0) { }
		Translatable(QString const & string) : _context(0) { setText(string); }
		Translatable(QObject *parent) : WIDGET(parent), _context(0) { }

		void setText(const QString &tag, QStringList const &args = QStringList())
		{
			_text_tag  = tag;
			_text_args = args;

			WIDGET::setText(_translate(tag, args));
		}

		void setToolTip(QString const &tag, QStringList const &args = QStringList())
		{
			_tool_tip_tag  = tag;
			_tool_tip_args = args;

			/*
			 * Before finally setting the translated text as tooltip, format it
			 * with a utility function. The format step must be the last,
			 * otherwise the translation mechanism breaks.
			 */
			WIDGET::setToolTip(format_tooltip(_translate(tag, args)));
		}

		QString text()    const { return _text_tag; }
		QString toolTip() const { return _tool_tip_tag; }
};

#endif /* _INCLUDE__QOOST__TRANSLATABLE_H_ */
