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

#ifndef _EJA_EMOJI_WIDGET_H_
#define _EJA_EMOJI_WIDGET_H_

#include <QFont>
#include <QSize>
#include <QWidget>

class QButtonGroup;
class QFocusEvent;
class QListWidgetItem;
class QListWidget;
class QPushButton;

namespace eja
{
	class emoji_widget final : public QWidget
	{
		Q_OBJECT;
	
	private:
		QFont m_font;
		QButtonGroup* m_button_group = nullptr;
		QListWidget* m_list = nullptr;
		QPushButton* m_activator_button = nullptr;
		QSize m_cell_size;

		// Emojis
		static const size_t s_num_smilies;
		static const size_t s_num_animals;
		static const size_t s_num_food;
		static const size_t s_num_sports;
		static const size_t s_num_places;
		static const size_t s_num_objects;
		static const size_t s_num_symbols;

	private:
		QPushButton* make_button(const char* emoji, const char* tooltip, const char* name = "emoji_category");
		void set_emojis(const char* emojis[], const size_t length);

		// Event
		virtual void focusOutEvent(QFocusEvent *event) override;

	public:
		emoji_widget(QWidget* parent = nullptr);

		// Set
		void set_activator_button(QPushButton* activator_button) { m_activator_button = activator_button; }
		void set_cell_size(QSize& size) { m_cell_size = size; }

	public slots:
		void toggle();

	signals:
		void emoji_clicked(QListWidgetItem* item);
		void closing();
	};
}

#endif
