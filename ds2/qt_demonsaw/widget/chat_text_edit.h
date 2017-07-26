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

#ifndef _EJA_TEXT_INPUT_WINDOW_
#define _EJA_TEXT_INPUT_WINDOW_

#include <QString>
#include <QPlainTextEdit>

#include "system/type.h"

class QToolBar;
class QToolButton;
class QWidget;

namespace eja
{
	class chat_text_edit : public QPlainTextEdit
	{
		Q_OBJECT

	private:
		s32 m_max_length;		

	protected:
		// Event
		virtual void keyPressEvent(QKeyEvent *e) override;

	signals:
		// Only Trims the last '\n' key when callback is made
		// Leaves any other remaining whitespace letters at the beginning
		// and end of the string
		void submit_text();

	public:
		chat_text_edit(QWidget* parent = 0, s32 max_length = -1);
		chat_text_edit(const QString &text, QWidget *parent = 0, s32 max_length = -1);
		virtual ~chat_text_edit() { }
	};
}

#endif
