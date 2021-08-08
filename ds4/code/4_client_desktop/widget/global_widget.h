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

#ifndef _EJA_GLOBAL_WIDGET_H_
#define _EJA_GLOBAL_WIDGET_H_

#include <QColor>
#include <QLineEdit>
#include <QPixmap>
#include <QWidget>

#include "system/type.h"

class QCheckBox;
class QLabel;
class QPaintEvent;
class QPushButton;
class QSlider;
class QSpinBox;
class QVBoxLayout;

namespace eja
{
	class global_widget final : public QWidget
	{
		Q_OBJECT;

	private:
		QPixmap m_background;
		QPushButton* m_help = nullptr;
		QPushButton* m_reset = nullptr;

		// Color
		QColor m_color;
		bool m_user_color = true;
		bool m_user_modified = false;
		bool m_start_connected = true;

		// Scaling
		QLabel* m_unsaved = nullptr;
		QLabel* m_scaling_label = nullptr;
		QSlider* m_scaling = nullptr;

		// Theme
		QLineEdit* m_theme = nullptr;
		QPushButton* m_theme_edit = nullptr;
		QPushButton* m_theme_update = nullptr;

		// Transfer
		QSpinBox* m_downloads = nullptr;
		QSpinBox* m_uploads = nullptr;

		QCheckBox* m_auto_open = nullptr;
		QCheckBox* m_statusbar = nullptr;
		QCheckBox* m_system_tray = nullptr;
		QCheckBox* m_timestamp = nullptr;
		QCheckBox* m_user_colors = nullptr;

		// Button
		QPushButton* m_start_button = nullptr;

	private:
		// Paint
		virtual void paintEvent(QPaintEvent *event) override;

		// Signal
		void signal();

		// Set
		void set_color();

		// Get
		QColor get_font_color(const QColor& background);
		QPushButton* get_help_button();

	signals:
		void done();

	public:
		global_widget(QWidget* parent = nullptr);
	};
}

#endif
