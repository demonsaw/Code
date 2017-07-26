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

#ifndef _EJA_STATUS_WINDOW_
#define _EJA_STATUS_WINDOW_

#include <QLabel>
#include <QWidget>

#include "entity/entity.h"

namespace eja
{
	class status_pane
	{
	private:
		QLabel* m_image = nullptr;
		QLabel* m_text = nullptr;

	public:
		status_pane(QWidget* parent = 0);

		// Mutator
		void set(const QImage& image, const QString& text) { set_image(image); set_text(text); }
		void set_tooltip(const QString& text) { m_image->setToolTip(text); }
		void set_image(const QImage& image) { m_image->setPixmap(QPixmap::fromImage(image.scaled(QSize(14, 14), Qt::KeepAspectRatio, Qt::SmoothTransformation))); }
		void set_text(const QString& text) { m_text->setText(text); }

		// Accessor
		QLabel* get_image() const { return m_image; }
		QLabel* get_text() const { return m_text; }
	};

	class status_window : public QWidget
	{
		Q_OBJECT

	private:
		size_t m_size = 0;
		status_pane** m_panes = nullptr;

	public:
		static const size_t max = 11;

	public:
		status_window(const size_t size, QWidget* parent = 0);
		virtual ~status_window();

		// Accessor
		size_t get_size() const { return m_size; }
		status_pane* get_pane(const size_t index) const { return (index < m_size) ? m_panes[index] : nullptr; }
	};
}

#endif
