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

#ifndef _EJA_STATUSBAR_PANE_H_
#define _EJA_STATUSBAR_PANE_H_

#include <QLabel>
#include <QString>

namespace eja
{
	class statusbar_pane final : public QWidget
	{
	private:
		QLabel* m_image = nullptr;
		QLabel* m_text = nullptr;

	public:
		explicit statusbar_pane(QWidget* parent = nullptr);
		statusbar_pane(const int image, const char* psz, QWidget* parent = nullptr);

		// Has
		bool has_image() const { return m_image && !m_image->text().isEmpty(); }
		bool has_text() const { return m_text && !m_text->text().isEmpty(); }

		// Set
		void set(const int image, const char* psz);
		void set_image(const int image);
		void set_text(const char* psz) { m_text->setText(psz); }
		void set_text(const QString& str) { m_text->setText(str); }
		void set_tooltip(const char* psz) { m_image->setToolTip(psz); }
		void set_tooltip(const QString& str) { m_text->setText(str); }

		// Get
		QLabel* get_image() const { return m_image; }
		QLabel* get_text() const { return m_text; }
	};
}

#endif