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

#ifndef _EJA_GENERAL_OPTION_WIDGET_H_
#define _EJA_GENERAL_OPTION_WIDGET_H_

#include "entity/entity_widget.h"
#include "widget/option/option_pane.h"

class QCheckBox;
class QColor;
class QGroupBox;
class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QVBoxLayout;

namespace eja
{
	class general_option_widget final : public option_pane
	{
		Q_OBJECT

	private:
		QVBoxLayout* m_layout = nullptr;

		// Client
		QColor m_color = nullptr;
		QGroupBox* m_client = nullptr;
		QLineEdit* m_name = nullptr;
		QPushButton* m_name_color = nullptr;
		QPushButton* m_random_color = nullptr;

		QCheckBox* m_chat_beep = nullptr;
		QCheckBox* m_chat_flash = nullptr;
		QCheckBox* m_chat_tab = nullptr;
		QCheckBox* m_chat_toast = nullptr;

		QCheckBox* m_pm_beep = nullptr;
		QCheckBox* m_pm_flash = nullptr;
		QCheckBox* m_pm_tab = nullptr;
		QCheckBox* m_pm_toast = nullptr;

		QCheckBox* m_room_beep = nullptr;
		QCheckBox* m_room_flash = nullptr;
		QCheckBox* m_room_tab = nullptr;
		QCheckBox* m_room_toast = nullptr;

		// File/Folder
		QLineEdit* m_save_path = nullptr;
		QPushButton* m_save_edit = nullptr;

	private:
		void layout();
		void signal();

		// Event
		virtual bool eventFilter(QObject* object, QEvent* event) override;

		// Set
		void set_client_color();
		void set_random_color();
		void set_style_color();

		// Get
		QColor get_client_color();
		QString get_random_name() const;

	public:
		general_option_widget(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr);

		// Utility
		virtual void help();
		virtual void reset();

	signals:
		void color_changed(QColor& color);
		void name_changed();

	};
}

#endif
