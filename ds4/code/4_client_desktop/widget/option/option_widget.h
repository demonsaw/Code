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

#ifndef _EJA_OPTION_WIDGET_H_
#define _EJA_OPTION_WIDGET_H_

#include <QColor>

#include "entity/entity_dock_widget.h"

class QAction;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QPoint;
class QPushButton;
class QVBoxLayout;

namespace eja
{
	class general_option_widget;
	class system_option_widget;
	class router_option_widget;
	class transfer_option_widget;

	class option_widget final : public entity_dock_widget
	{
		Q_OBJECT

	private:
		QVBoxLayout* m_layout = nullptr;
		QPushButton* m_help = nullptr;
		QPushButton* m_reset = nullptr;

		QAction* m_close_action = nullptr;
		
		general_option_widget* m_general = nullptr;
		system_option_widget* m_system = nullptr;
		router_option_widget* m_router = nullptr;
		transfer_option_widget* m_transfer = nullptr;

		QLabel* m_name_label = nullptr;		
		QPushButton* m_general_button = nullptr;
		QPushButton* m_system_button = nullptr;
		QPushButton* m_router_button = nullptr;
		QPushButton* m_transfer_button = nullptr;

		QColor m_button_background;
		QColor m_button_border;
		QColor m_button_hover;

	private:
		// Interface
		virtual void on_update() override;

		// Init		
		void section();
		void signal();

		// Utility
		void help();
		void reset();

		// Event
		void on_section(QPushButton* button);

		// Set
		void set_style_color();

		// Get
		QVBoxLayout* get_vbox_layout() const;
		QHBoxLayout* get_hbox_layout() const;

	private slots:
		void show_menu(const QPoint& point);

	public:
		option_widget(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr);
		void set_button_style();

	public slots:
		void update_color(const QColor& color);
	};
}

#endif
