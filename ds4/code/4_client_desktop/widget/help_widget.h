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

#ifndef _EJA_HELP_WIDGET_H_
#define _EJA_HELP_WIDGET_H_

#include <QWidget>

class QFocusEvent;
class QVBoxLayout;
class QLabel;
class QMouseEvent;
class QPaintEvent;
class QPushButton;
class QPixmap;

namespace eja
{
	class font_button;

	class help_widget final : public QWidget
	{
		Q_OBJECT

	private:
		QVBoxLayout* m_layout = nullptr;
		QPushButton* m_activator_button = nullptr;

		// About
		QWidget* m_about_container = nullptr;
		QLabel* m_about_caption = nullptr;
		QLabel* m_about_icon = nullptr;
		QLabel* m_about_title = nullptr;

		// Site
		QWidget* m_site_container = nullptr;
		QLabel* m_site_caption = nullptr;
		QLabel* m_site_icon = nullptr;
		QLabel* m_site_title = nullptr;

		// Wiki
		QWidget* m_wiki_container = nullptr;
		QLabel* m_wiki_caption = nullptr;
		QLabel* m_wiki_icon = nullptr;
		QLabel* m_wiki_title = nullptr;

	private:
		font_button* make_button(const int icon, const char* tooltip);
		QWidget* make_container(QLabel* icon, QLabel* title, QLabel* caption);

		// Event
		virtual void focusOutEvent(QFocusEvent* event) override;
		virtual void paintEvent(QPaintEvent* event) override;
		virtual void mouseReleaseEvent(QMouseEvent* event) override;
		void resize();

	public:
		help_widget(QPushButton* button, QWidget* parent = nullptr);
		
	public slots:
		void toggle();
		void parent_resized();

	};
}

#endif
