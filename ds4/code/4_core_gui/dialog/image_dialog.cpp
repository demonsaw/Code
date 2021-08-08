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

#include <QBitmap>
#include <QEvent>
#include <QPainter>

#include "dialog/image_dialog.h"

namespace eja
{
	// Constructor
	image_dialog::image_dialog(const char* image, QWidget* parent /*= nullptr*/) : QDialog(parent, Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint)
	{
		setAttribute(Qt::WA_TranslucentBackground);
		setFixedSize(QSize(448, 448));

		// Event
		installEventFilter(this);

		// Pixmap
		QPixmap pixmap(image);
		m_background = pixmap.scaledToWidth(448, Qt::SmoothTransformation);
		setMask(m_background.mask());
	}

	// Event
	bool image_dialog::eventFilter(QObject* obj, QEvent* e)
	{
		if ((e->type() == QEvent::MouseButtonRelease) || (e->type() == QEvent::KeyPress))
		{
			close();

			return true;
		}			

		return false;
	}

	// Paint
	void image_dialog::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		QPoint center(width() / 2 - m_background.width() / 2, 0);
		painter.drawPixmap(center, m_background);
	}
}
