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

#ifndef _EJA_ABOUT_DIALOG_H_
#define _EJA_ABOUT_DIALOG_H_

#include <QDialog>
#include <QPixmap>

class QFocusEvent;
class QPaintEvent;

namespace eja
{
	class about_dialog final : public QDialog
	{
		Q_OBJECT;

	private:
		QPixmap m_background;

	private:
		// Event
		virtual bool eventFilter(QObject* obj, QEvent* e) override;
		virtual void focusOutEvent(QFocusEvent* e) override { close(); }

		// Paint
		virtual void paintEvent(QPaintEvent* event) override;

	public:
		about_dialog(QWidget* parent = nullptr);
	};
}

#endif
