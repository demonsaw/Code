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

#include <QAction>
#include <QFileDialog>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSplitter>
#include <QScrollBar>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_io_component.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/search/keyword_component.h"
#include "delegate/io/file_name_delegate.h"
#include "delegate/search/search_weight_delegate.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_table_view.h"
#include "entity/entity_window.h"
#include "font/font_awesome.h"
#include "model/client/client_search_model.h"
#include "widget/text_edit_widget.h"
#include "widget/client/client_search_widget.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	client_search_widget::client_search_widget(const entity::ptr entity, const QString& text, QWidget* parent /*= 0*/) : entity_widget(entity, text, fa::search, callback_type::search, parent)
	{
		assert(thread_info::main());

		// Action
		m_download_action = make_action("Download", fa::arrowdown);
		m_download_to_action = make_action("Download To...", fa::folderopen);
		m_remove_action = make_action("Remove", fa::minus);
		m_update_action = make_action("Update", fa::bolt);

		// Menu
		add_button("Remove", fa::minus, [&]() { clear(); });
		add_button("Update", fa::bolt, [&]() { update(); });
		add_button("Popout", fa::clone, [&]() { popout(); });
		add_button("Close", fa::close, [&]() { close(); });

		// Text
		auto hlayout = new QHBoxLayout;
		hlayout->setContentsMargins(0, 0, 0, 0);
		hlayout->setSpacing(0);
		hlayout->setMargin(0);

		hlayout->addWidget(m_icon);
		hlayout->addWidget(m_title);
		hlayout->addStretch(1);
		hlayout->addWidget(m_toolbar);

		m_titlebar = new QWidget(this);
		m_titlebar->installEventFilter(this);
		m_titlebar->setObjectName("menubar");
		m_titlebar->setLayout(hlayout);
		setTitleBarWidget(m_titlebar);

		// Table
		m_table = new entity_table_view(m_entity, this);
		m_model = new client_search_model(m_entity, { "", "Name", "Type", "Checksum", "Size" }, { 24, 320, 72, 72, 72 }, this);
		init(m_table, m_model);

		// Style
		m_table->verticalScrollBar()->setObjectName("browse");

		// Header
		const auto hheader = m_table->horizontalHeader();
		hheader->setSectionResizeMode(client_search_model::column::weight, QHeaderView::Fixed);
		hheader->setSortIndicator(client_search_model::column::weight, Qt::SortOrder::DescendingOrder);

		// Text		
		m_text = new text_edit_widget(this);
		m_text->setObjectName("search");
		m_text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_text->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_text->setFocusPolicy(Qt::StrongFocus);
		m_text->setTabChangesFocus(false);
		m_text->setMinimumHeight(26);

		// Data
		const auto qtext = text.simplified();
		if (!qtext.isEmpty())
		{
			m_text->setPlainText(qtext);
			m_text->setFocus();
			m_text->selectAll();
		}
		else
		{
			m_title_text = "Search";
			m_title->setText(m_title_text);
		}

		// Callback
		add_callback(callback_action::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback_action::clear, [&](const entity::ptr entity) { m_model->clear(); });

		// Delegate
		m_table->setColumnWidth(client_search_model::column::weight, 24);
		m_table->setItemDelegateForColumn(client_search_model::column::weight, new search_weight_delegate(m_table)); 
		m_table->setItemDelegateForColumn(client_search_model::column::name, new file_name_delegate(m_table));

		// Pane
		m_enter_button = new QPushButton(this);
		const auto endpoint = m_entity->find<endpoint_component>();
		m_enter_button->setText(QString("%1:").arg(endpoint->get_name().c_str()));
		m_enter_button->setObjectName("search");
		m_enter_button->setToolTip("Search");
		const auto enabled = static_cast<size_t>(qtext.length()) >= search::min_text;
		m_enter_button->setEnabled(enabled);

		QHBoxLayout* input_layout = new QHBoxLayout;
		input_layout->setContentsMargins(0, 0, 0, 0);
		input_layout->setSpacing(0);
		input_layout->setMargin(0);

		input_layout->addWidget(m_enter_button, 0, Qt::AlignTop);
		input_layout->addWidget(m_text);

		QWidget* widget = new QWidget(this);
		widget->setObjectName("border");
		widget->setLayout(input_layout);

		QSplitter* splitter = new QSplitter(Qt::Vertical);
		splitter->setContentsMargins(0, 0, 0, 0);
		splitter->setChildrenCollapsible(false);
		splitter->addWidget(widget);
		splitter->setStretchFactor(0, 1);
		splitter->addWidget(m_table);
		splitter->setStretchFactor(1, 32);
		splitter->setHandleWidth(4);		

		// Signal
		connect(m_table, &entity_table_view::customContextMenuRequested, this, &client_search_widget::show_menu);
		connect(m_table, &entity_table_view::doubleClicked, m_table, &entity_table_view::double_click);
		connect(this, &client_search_widget::visibilityChanged, this, &client_search_widget::on_visibility_changed);

		connect(m_enter_button, &font_button::clicked, this, &client_search_widget::on_search);
		connect(m_text, &text_edit_widget::submit_text, this, &client_search_widget::on_search);
		connect(m_text, &text_edit_widget::textChanged, this, &client_search_widget::on_text_changed);

		connect(m_download_action, &QAction::triggered, this, &client_search_widget::on_download);
		connect(m_download_to_action, &QAction::triggered, this, &client_search_widget::on_download_to);
		connect(m_remove_action, &QAction::triggered, [&]() { m_table->remove(); });
		connect(m_update_action, &QAction::triggered, [&]() { update(); });

		// Sort
		m_table->sortByColumn(client_search_model::column::weight);

		setWidget(splitter);
	}

	// Interface
	void client_search_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (m_entity->get_id() == entity->get_id())
			m_model->add(entity);
	}

	void client_search_widget::on_clear()
	{
		assert(thread_info::main());

		// Id
		m_entity->set_id();

		m_title_text = "Search";
		m_title->setText(m_title_text);

		m_text->clear();
		m_text->setFocus();

		m_table->clear();
	}

	void client_search_widget::on_update()
	{
		assert(thread_info::main());

		// Spam
		auto button = get_button(fa::bolt);
		if (button)
		{
			button->setEnabled(false);
			QTimer::singleShot(ui::refresh, [&]() { set_enabled(fa::bolt); });
		}

		on_search();
	}

	void client_search_widget::on_download()
	{
		assert(thread_info::main());

		const auto parent = m_entity->get_parent();
		const auto service = parent->get<client_service_component>();
		if (service->invalid())
			return;

		m_table->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity && entity->has<file_component>())
				service->async_file(entity);
		});
	}

	void client_search_widget::on_download_to()
	{
		assert(thread_info::main());

		const auto parent = m_entity->get_parent();
		const auto service = parent->get<client_service_component>();
		if (service->invalid())
			return;

		QFileDialog dialog;
		dialog.setWindowTitle("Download To...");
		dialog.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
		dialog.setOption(QFileDialog::DontUseNativeDialog, true);
		dialog.setFileMode(QFileDialog::Directory);
		dialog.setViewMode(QFileDialog::ViewMode::List);
		
		const auto io = parent->get<client_io_component>();
		dialog.setDirectory(QString::fromStdString(io->get_path()));

		if (dialog.exec())
		{
			const auto qpaths = dialog.selectedFiles();
			const auto& qpath = qpaths.at(0);
			const auto path = qpath.toStdString();

			m_table->execute([&](const QModelIndex& index)
			{
				const auto entity = m_model->get_entity(index);
				if (entity && entity->has<file_component>())
					service->async_file(entity, path);
			});
		}
	}

	void client_search_widget::on_search()
	{
		assert(thread_info::main());

		// Spam
		if (!m_enabled || !m_enter_button->isEnabled())
			return;

		// Spam
		const auto qtext = m_text->toPlainText().simplified().toLower();
		const auto qlength = static_cast<size_t>(qtext.length() - qtext.count(' '));
		if (qlength < search::min_text)
			return;

		m_title_text = QString("Search: %1").arg(qtext);
		m_title->setText(m_title_text);

		m_model->clear();

		// Service
		const auto service = m_entity->find<client_service_component>();
		if (service->valid())
		{
			// Id
			m_entity->set_id();

			// Keyword
			const auto text = qtext.toStdString();
			const auto keyword = m_entity->get<keyword_component>();			
			keyword->set_text(text);

			// Async
			service->async_search(m_entity);
		}

		// Cursor
		m_text->setFocus();
		m_text->selectAll();

		// Spam
		m_enabled = false;
		m_enter_button->setEnabled(false);

		QTimer::singleShot(ui::enter, [this]()
		{
			m_enabled = true;
			const auto qtext = m_text->toPlainText().simplified();
			const auto enabled = static_cast<size_t>(qtext.length()) >= search::min_text;
			m_enter_button->setEnabled(enabled);
		});
	}

	// Event
	void client_search_widget::on_text_changed()
	{
		assert(thread_info::main());

		const auto qtext = m_text->toPlainText().simplified();
		const auto enabled = m_enabled && static_cast<size_t>(qtext.length()) >= search::min_text;
		m_enter_button->setEnabled(enabled);

		if (static_cast<size_t>(qtext.size()) > search::max_text)
		{
			const auto ltext = qtext.left(search::max_text);
			m_text->setPlainText(ltext);
		}
	}

	void client_search_widget::showEvent(QShowEvent* event)
	{
		if (!visibleRegion().isEmpty())
		{
			m_text->setFocus();
			m_text->selectAll();
		}

		// Event
		entity_widget::showEvent(event);
	}

	void client_search_widget::on_visibility_changed(bool visible)
	{
		if (visible && !visibleRegion().isEmpty())
		{
			m_text->setFocus();
			m_text->selectAll();
		}
	}

	void client_search_widget::closeEvent(QCloseEvent* event)
	{
		assert(thread_info::main());

		QDockWidget::closeEvent(event);

		// HACK: Defer deletion so we don't crash due to timer
		QTimer::singleShot(ui::enter, [this]() { delete this; });
	}

	// Menu
	void client_search_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		const auto index = m_table->indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);

		if (has_file())
		{
			menu.addAction(m_download_action);
			menu.addAction(m_download_to_action);
			menu.addSeparator();
		}			

		menu.addAction(m_remove_action);
		menu.addAction(m_update_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}

	bool client_search_widget::has_file() const
	{
		assert(thread_info::main());

		auto selected = m_table->selectionModel()->selectedRows();

		if (!selected.empty())
		{
			const auto proxy = m_table->get_proxy_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = m_model->get_entity(sindex);
						if (entity && entity->has<file_component>())
							return true;
					}
				}
			}
		}

		return false;
	}
}
