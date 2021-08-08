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

#ifndef _EJA_IMAGE_WIDGET_H_
#define _EJA_IMAGE_WIDGET_H_

#include <QString>
#include <QPixmap>
#include <QWidget>

#include "system/type.h"

namespace eja
{
	class image_widget final : public QWidget
	{
		Q_OBJECT

	private:
		size_t m_width;
		QPixmap m_image;
		QPixmap m_unscaled_image;

	private:
		// Event
		virtual void paintEvent(QPaintEvent* event) override;

		void scale_image();

	public:
		image_widget(const QString& image_path, const size_t width, QWidget* parent);
		image_widget(const char* image_path, const size_t width, QWidget* parent) : image_widget(QString(image_path), width, parent) { }

	public slots:
		void parent_resized();
	};
}

#endif
