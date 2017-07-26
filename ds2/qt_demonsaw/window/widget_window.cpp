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

#include <QApplication>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include "widget_window.h"
#include "resource/resource.h"

namespace eja
{
	// Interface
	void widget_window::init()
	{
		// Signal
		connect(this, static_cast<void (widget_window::*)()>(&widget_window::added), this, static_cast<void (widget_window::*)()>(&widget_window::on_add)); 
		connect(this, static_cast<void (widget_window::*)(const entity::ptr)>(&widget_window::added), this, static_cast<void (widget_window::*)(const entity::ptr)>(&widget_window::on_add));

		connect(this, static_cast<void (widget_window::*)()>(&widget_window::removed), this, static_cast<void (widget_window::*)()>(&widget_window::on_remove)); 
		connect(this, static_cast<void (widget_window::*)(const entity::ptr)>(&widget_window::removed), this, static_cast<void (widget_window::*)(const entity::ptr)>(&widget_window::on_remove));
		
		connect(this, static_cast<void (widget_window::*)()>(&widget_window::refreshed), this, static_cast<void (widget_window::*)()>(&widget_window::on_refresh)); 
		connect(this, static_cast<void (widget_window::*)(const entity::ptr)>(&widget_window::refreshed), this, static_cast<void (widget_window::*)(const entity::ptr)>(&widget_window::on_refresh));
		
		connect(this, static_cast<void (widget_window::*)()>(&widget_window::cleared), this, static_cast<void (widget_window::*)()>(&widget_window::on_clear)); 
		connect(this, static_cast<void (widget_window::*)(const entity::ptr)>(&widget_window::cleared), this, static_cast<void (widget_window::*)(const entity::ptr)>(&widget_window::on_clear));
	}

	void widget_window::create(QWidget* parent /*= 0*/)
	{
		// Toolbar
		m_toolbar = new QToolBar(parent);
		m_toolbar->setObjectName("tab");
		m_toolbar->setCursor(Qt::ArrowCursor);
		m_toolbar->setIconSize(QSize(16, 16));

		// Spacer
		QWidget* spacer = new QWidget(m_toolbar);
		spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_toolbar->addWidget(spacer);
	}

	// Slot
	void widget_window::on_refresh()
	{
		assert(thread::main());

		if (m_refresh_button)
		{
			m_refresh_button->setEnabled(false);
			QApplication::setOverrideCursor(Qt::WaitCursor);

			QTimer::singleShot(default_timeout::client::refresh, [this]()
			{
				m_refresh_button->setEnabled(true);
				QApplication::restoreOverrideCursor();
			});
		}
	}

	void widget_window::on_refresh(const entity::ptr entity)
	{
		assert(thread::main());

		if (m_refresh_button)
		{
			m_refresh_button->setEnabled(false);
			QApplication::setOverrideCursor(Qt::WaitCursor);

			QTimer::singleShot(default_timeout::client::refresh, [this]()
			{
				m_refresh_button->setEnabled(true);
				QApplication::restoreOverrideCursor();
			});
		}
	}

	// Utility	
	void widget_window::add_button(widget_button button, const bool enabled /*= true*/)
	{
		switch (button)
		{
			case widget_button::add:
			{
				// Button
				m_add_button = new QToolButton(m_toolbar);
				m_add_button->setObjectName("tab");
				m_add_button->setIcon(QIcon(resource::menu::add));
				m_add_button->setCursor(Qt::ArrowCursor);
				m_add_button->setToolTip("Add");
				m_add_button->setEnabled(enabled);
				m_toolbar->addWidget(m_add_button);

				// Signal
				connect(m_add_button, &QToolButton::clicked, this, static_cast<void (widget_window::*)()>(&widget_window::add));

				break;
			}
			case widget_button::remove:
			{
				// Button
				m_remove_button = new QToolButton(m_toolbar);
				m_remove_button->setObjectName("tab");
				m_remove_button->setIcon(QIcon(resource::menu::remove));
				m_remove_button->setCursor(Qt::ArrowCursor);
				m_remove_button->setToolTip("Remove");
				m_remove_button->setEnabled(enabled);
				m_toolbar->addWidget(m_remove_button);

				// Signal
				connect(m_remove_button, &QToolButton::clicked, this, static_cast<void (widget_window::*)()>(&widget_window::remove));

				break;
			}
			case widget_button::clear:
			{
				// Button
				m_clear_button = new QToolButton(m_toolbar);
				m_clear_button->setObjectName("tab");
				m_clear_button->setIcon(QIcon(resource::menu::clear));
				m_clear_button->setCursor(Qt::ArrowCursor);
				m_clear_button->setToolTip("Clear");
				m_clear_button->setEnabled(enabled);
				m_toolbar->addWidget(m_clear_button);

				// Signal
				connect(m_clear_button, &QToolButton::clicked, this, static_cast<void (widget_window::*)()>(&widget_window::clear));

				break;
			}
			case widget_button::refresh:
			{
				// Button
				m_refresh_button = new QToolButton(m_toolbar);
				m_refresh_button->setObjectName("tab");
				m_refresh_button->setIcon(QIcon(resource::menu::refresh));
				m_refresh_button->setCursor(Qt::ArrowCursor);
				m_refresh_button->setToolTip("Refresh");
				m_refresh_button->setEnabled(enabled);
				m_toolbar->addWidget(m_refresh_button);

				// Signal
				connect(m_refresh_button, &QToolButton::clicked, this, static_cast<void (widget_window::*)()>(&widget_window::refresh));

				break;
			}
			default:
			{
				assert(false);
				break;
			}
		}
	}
}
