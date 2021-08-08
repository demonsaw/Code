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
#include <QStyleOption>
#include <QVBoxLayout>

#include "resource/resource.h"
#include "widget/help_dialog.h"

namespace eja
{
	help_dialog::help_dialog(QWidget* parent /*= nullptr*/) : QDialog(parent)
	{
		// Ratio
		const auto ratio = resource::get_ratio();

		// Widget
		setObjectName("dialog");
		setMinimumSize(QSize(448 * ratio, 320 * ratio));
		setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
		setWindowTitle("Help");

		// Variables
		m_text = new QTextEdit(this);
		m_text->setObjectName("help");
		m_text->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
		m_text->setLineWrapMode(QTextEdit::WidgetWidth);
		m_text->setReadOnly(true);

		// Layout
		const auto vlayout = resource::get_vbox_layout();
		vlayout->setContentsMargins(resource::get_top_margin(), resource::get_top_margin(), resource::get_top_margin(), resource::get_top_margin());
		vlayout->addWidget(m_text);

		setLayout(vlayout);
	}

	help_dialog::help_dialog(const QString& text, QWidget* parent /*= nullptr*/) : help_dialog(parent)
	{
		set_text(text);
	}
}
