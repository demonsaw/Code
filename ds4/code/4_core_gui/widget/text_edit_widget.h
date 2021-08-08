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

#ifndef _EJA_TEXT_EDIT_WIDGET_H_
#define _EJA_TEXT_EDIT_WIDGET_H_

#include <QPlainTextEdit>

class QKeyEvent;
class QString;
class QWidget;

namespace eja
{
	class text_edit_widget final : public QPlainTextEdit
	{
		Q_OBJECT

	private:
		virtual void keyPressEvent(QKeyEvent* event) override;
		virtual void resizeEvent(QResizeEvent* event) override;

	signals:
		void submit_text();

	public:
		text_edit_widget(QWidget* parent = nullptr);
		text_edit_widget(const QString& text, QWidget* parent = nullptr);
	};
}

#endif
