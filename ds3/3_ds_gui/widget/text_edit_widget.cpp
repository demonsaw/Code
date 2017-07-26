//
// The MIT License(MIT)
//
// Copyright(c) 2014 Demonsaw LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cassert>

#include <QKeyEvent>
#include <QString>
#include <QWidget>

#include "thread/thread_info.h"
#include "widget/text_edit_widget.h"

namespace eja
{
	// Constructor
	text_edit_widget::text_edit_widget(QWidget* parent /*= 0*/) : QPlainTextEdit(parent)
	{
		setDocument(0);
	}

	text_edit_widget::text_edit_widget(const QString& text, QWidget* parent /*= 0*/) : QPlainTextEdit(parent)
	{
		setDocument(0);

		setPlainText(text);
	}

	// Event
	void text_edit_widget::keyPressEvent(QKeyEvent* event)
	{
		assert(thread_info::main());

		if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return) && !(event->modifiers() & Qt::ShiftModifier))
		{
			// Remove the newline character '\n' from our text as QPlainTextEdit::keyPressEvent will add it to our
			// text stream but since shift it not pressed we want to read the return key as a notification that our
			// text field is to be used as input
			QString text = toPlainText();
			if (text.endsWith('\n'))
			{
				text.chop(1);
				setPlainText(text);
			}

			// Only Use The 'submit_text' Callback If Shift Is NOT Also Pressed
			emit submit_text();
		}
		else
		{
			QPlainTextEdit::keyPressEvent(event);
		}
	}
}
