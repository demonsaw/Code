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

#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPoint>

#include "resource/resource.h"
#include "widget/image_widget.h"

namespace eja
{
	image_widget::image_widget(const QString& image_path, const size_t width, QWidget* parent) : m_width(width), QWidget(parent)
	{
		m_unscaled_image = QPixmap(image_path);
		scale_image();
		setAttribute(Qt::WA_TransparentForMouseEvents);
		setFixedSize(m_image.width(), m_image.height());
	}

	void image_widget::paintEvent(QPaintEvent* event)
	{
		QPoint center = QPoint(parentWidget()->width() / 2 - m_image.width() / 2, parentWidget()->height() / 2 - m_image.height() / 2 + resource::get_row_height());

		QPainter painter(this);
		painter.setOpacity(0.20);
		painter.drawPixmap(center, m_image);
	}

	void image_widget::parent_resized()
	{
		scale_image();
		setFixedSize(parentWidget()->width(), parentWidget()->height());
	}

	void image_widget::scale_image()
	{
		size_t width = parentWidget()->width();
		size_t height = parentWidget()->height();
		if (width > height)
		{
			size_t scaled = std::min(size_t(height * 0.60), m_width);
			m_image = m_unscaled_image.scaledToHeight(scaled, Qt::SmoothTransformation);
		}
		else
		{
			size_t scaled = std::min(size_t(width / 3), m_width);
			m_image = m_unscaled_image.scaledToWidth(scaled, Qt::SmoothTransformation);
		}
	}
}
