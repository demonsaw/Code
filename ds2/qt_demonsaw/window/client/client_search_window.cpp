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
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QLocale>
#include <QSizePolicy>
#include <QSplitter>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "client_search_window.h"
#include "resource/resource.h"
#include "utility/default_value.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	client_search_window::client_search_window(const entity::ptr entity, QWidget* parent /*= 0*/) : widget_window(entity, parent)
	{
		create(parent);
		layout(parent);
		signal(parent);

		init();
	}

	// Interface
	void client_search_window::create(QWidget* parent /*= 0*/)
	{
		widget_window::create(parent);

		// Table
		m_search_table = new client_search_table(this);
		m_search_model = new client_search_model(m_entity, { "", "File", "Size", "Type" }, { 37, 411, 72, 72 }, this);
		init(m_search_table, m_search_model);

		// Button
		m_add_button = new QToolButton(this);
		m_add_button->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_add_button->setObjectName("chat");
		m_add_button->setIcon(QIcon(resource::tab::search));
		m_add_button->setToolTip("Search");
		m_add_button->setEnabled(false);

		// Keyword
		m_keyword = new chat_text_edit(this);
		m_keyword->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_keyword->setObjectName("chat");
		m_keyword->setMinimumHeight(24);
		m_keyword->setFocusPolicy(Qt::StrongFocus);
		m_keyword->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);		

		// Filter
		m_filter = new QComboBox(this);
		m_filter->setObjectName("search");
		m_filter->setMinimumHeight(24);
		m_filter->setFixedWidth(68);

		QListView* filter_view = new QListView(m_filter);
		filter_view->setObjectName("search");
		m_filter->setView(filter_view);

		m_filter->addItem("Any");
		m_filter->addItem("Audio");
		m_filter->addItem("Image");
		m_filter->addItem("Video");
	}

	void client_search_window::layout(QWidget* parent /*= 0*/)
	{
		widget_window::layout(parent);

		// Button
		add_button(widget_button::remove, false);
		add_button(widget_button::clear);

		// Layout		
		QWidget* hwidget = new QWidget;
		hwidget->setFixedHeight(24);
		hwidget->setObjectName("chat");

		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->setContentsMargins(0, 0, 0, 0);
		hlayout->setSpacing(0);
		hlayout->setMargin(0);
		
		hlayout->addSpacing(3);
		hlayout->addWidget(m_add_button, 0, Qt::AlignTop | Qt::AlignRight);
		hlayout->addWidget(m_keyword);	
		hlayout->addSpacing(4);
		hlayout->addWidget(m_filter);
		hwidget->setLayout(hlayout);

		// Splitter
		QSplitter* splitter = new QSplitter(Qt::Vertical);
		splitter->setContentsMargins(0, 0, 0, 0);
		splitter->setObjectName("chat");
		splitter->setChildrenCollapsible(false);
		splitter->addWidget(m_search_table);
		splitter->setStretchFactor(0, 20);
		splitter->addWidget(hwidget);
		splitter->setStretchFactor(1, 1);
		splitter->setHandleWidth(4);

		QVBoxLayout* vlayout = new QVBoxLayout(this);
		vlayout->setContentsMargins(0, 0, 0, 0);
		vlayout->setSpacing(0);
		vlayout->setMargin(0);

		vlayout->addWidget(splitter);

		setLayout(vlayout);
	}

	void client_search_window::signal(QWidget* parent /*= 0*/)
	{
		widget_window::signal(parent);

		// Signal
		connect(m_add_button, &QToolButton::clicked, [this]() { on_add(); });
		connect(m_keyword, &chat_text_edit::submit_text, this, &client_search_window::on_add);
		connect(m_keyword, &chat_text_edit::textChanged, this, &client_search_window::on_text_changed);
		connect(m_filter, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &client_search_window::on_activated);

		// Event
		m_search_table->installEventFilter(this);

		// Callback		
		auto& functions = m_search_table->get_functions();
		functions.add(table_callback::remove, function::create([this]() { on_remove(); }));
		functions.add(table_callback::clear, function::create([this]() { on_clear(); }));
		functions.add(table_callback::click, function::create([this]() { on_click(); }));
	}

	// Slot
	void client_search_window::on_add()
	{
		assert(thread::main());

		if (!m_enabled)
			return;

		const auto qkeyword = m_keyword->toPlainText().trimmed();
		const auto qlength = static_cast<size_t>(qkeyword.length() - qkeyword.count(' '));
		if (qlength < default_search::min_keywords)
			return;

		// Cursor
		m_keyword->setFocus();
		m_keyword->selectAll();

		m_enabled = false;
		m_add_button->setEnabled(false);

		QTimer::singleShot(default_search::timeout, [this]()
		{ 
			m_enabled = true;
			const auto qkeyword = m_keyword->toPlainText().trimmed();
			m_add_button->setEnabled(static_cast<size_t>(qkeyword.length()) >= default_search::min_keywords);
		});

		QApplication::setOverrideCursor(Qt::WaitCursor);

		QTimer::singleShot(default_timeout::client::refresh, [this]()
		{
			QApplication::restoreOverrideCursor();
		});

		const auto keyword = qkeyword.toLower().toStdString();
		const auto index = std::max(0, m_filter->currentIndex());
		const auto filter = static_cast<file_filter>(index);
		m_search_table->add(keyword, filter);
	}

	void client_search_window::on_remove()
	{ 
		assert(thread::main());

		m_keyword->setFocus();
		m_search_table->remove(); 
	}

	void client_search_window::on_clear()
	{
		assert(thread::main());

		m_keyword->clear();
		m_keyword->setFocus();
		m_add_button->setEnabled(false);
		m_remove_button->setEnabled(false);
		m_search_table->clear();
	}

	void client_search_window::on_click()
	{
		assert(thread::main());

		const auto enabled = m_search_table->selectionModel()->selectedRows().count() > 0;
		m_remove_button->setEnabled(enabled);
	}

	void client_search_window::on_text_changed()
	{
		assert(thread::main());

		if (!m_enabled)
			return;

		const auto qkeyword = m_keyword->toPlainText().trimmed();
		m_add_button->setEnabled(static_cast<size_t>(qkeyword.length()) >= default_search::min_keywords);

		if (static_cast<size_t>(qkeyword.size()) > default_search::max_keywords)
		{
			const auto text = qkeyword.left(default_search::max_keywords);
			m_keyword->setPlainText(text);
		}
	}

	void client_search_window::on_activated(int index)
	{
		assert(thread::main());

		m_keyword->setFocus();
	}

	// Event
	void client_search_window::showEvent(QShowEvent* event)
	{
		// Timer
		QTimer::singleShot(0, m_keyword, SLOT(setFocus()));
		QTimer::singleShot(0, m_keyword, SLOT(selectAll()));

		// Event
		QWidget::showEvent(event);
	}

	bool client_search_window::eventFilter(QObject* obj, QEvent* e)
	{
		if (e->type() == QEvent::KeyPress)
		{
			const auto ke = static_cast<QKeyEvent*>(e);

			switch (ke->key())
			{
				case Qt::Key_Up:
				case Qt::Key_Down:
				case Qt::Key_PageUp:
				case Qt::Key_PageDown:
				case Qt::Key_Shift:
				{
					return QObject::eventFilter(obj, e);
				}
			}

			if (!(ke->modifiers() & Qt::ControlModifier))
			{
				m_keyword->setFocus();
				return qApp->sendEvent(m_keyword, e);
			}
		}

		return QObject::eventFilter(obj, e);
	}
}
