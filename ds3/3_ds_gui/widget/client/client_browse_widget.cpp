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
#include "component/search/search_component.h"
#include "delegate/browse/browse_delegate.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_tree_view.h"
#include "entity/entity_window.h"
#include "font/font_awesome.h"
#include "model/client/client_browse_model.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "widget/text_edit_widget.h"
#include "widget/client/client_browse_widget.h"
#include "widget/client/client_search_widget.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	client_browse_widget::client_browse_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Browse", fa::sitemap, callback_type::browse, parent)
	{
		assert(thread_info::main());

		// Action
		m_chat_action = make_action("Message", fa::envelope);
		m_download_action = make_action("Multiple Clients", fa::users);	
		m_download_action_client = make_action("Single Client", fa::user);
		m_download_to_action = make_action("Multiple Clients", fa::users);
		m_download_to_client_action = make_action("Single Client", fa::user);
		m_mute_action = make_action("Mute", fa::ban);
		m_unmute_action = make_action("Unmute", fa::circleo);
		m_update_action = make_action("Update", fa::bolt);

		// Menu
		add_button("Update", fa::bolt, [&]() { update(); });
		add_button("Popout", fa::clone, [&]() { popout(); });
		add_button("Close", fa::close, [&]() { close(); });

		// Layout
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

		// Tree
		m_tree = new entity_tree_view(m_entity, this);
		m_model = new client_browse_model(m_entity, { "Name", "Type", "Checksum", "Size" }, { 38 + 320, 72, 72, 72 }, this);
		init(m_tree, m_model);

		// Style
		m_tree->verticalScrollBar()->setObjectName("browse");

		// Text
		m_text = new text_edit_widget(this);
		m_text->setObjectName("search"); 
		m_text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_text->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_text->setFocusPolicy(Qt::StrongFocus);
		m_text->setTabChangesFocus(false);
		m_text->setMinimumHeight(26);

		// Callback
		add_callback(callback_action::create, [&](const entity::ptr entity) { create(); });
		add_callback(callback_action::add, [&](const entity::ptr entity) { m_model->add(entity); }); 
		add_callback(callback_action::update, [&](const entity::ptr entity) { update(entity); });
		add_callback(callback_action::clear, [&](const entity::ptr entity) { m_model->clear(); });

		// Delegate
		m_tree->setItemDelegateForColumn(client_browse_model::column::name, new browse_delegate(m_tree));

		// Pane
		m_button = new QPushButton(this);
		const auto endpoint = m_entity->get<endpoint_component>();
		m_button->setText(QString("%1:").arg(endpoint->get_name().c_str()));
		m_button->setObjectName("search");
		m_button->setToolTip("Search");
		m_button->setEnabled(false);

		QHBoxLayout* input_layout = new QHBoxLayout;
		input_layout->setContentsMargins(0, 0, 0, 0);
		input_layout->setSpacing(0);
		input_layout->setMargin(0);

		input_layout->addWidget(m_button, 0, Qt::AlignTop);
		input_layout->addWidget(m_text);

		QWidget* widget = new QWidget(this);
		widget->setObjectName("search");
		widget->setLayout(input_layout);

		QSplitter* splitter = new QSplitter(Qt::Vertical);
		splitter->setContentsMargins(0, 0, 0, 0);
		splitter->setChildrenCollapsible(false);
		splitter->addWidget(widget); 
		splitter->setStretchFactor(0, 1);
		splitter->addWidget(m_tree);
		splitter->setStretchFactor(1, 32);
		splitter->setHandleWidth(4);

		// Signal		
		connect(m_tree, &entity_tree_view::customContextMenuRequested, this, &client_browse_widget::show_menu);
		connect(m_tree, &entity_tree_view::doubleClicked, m_tree, &entity_tree_view::double_click); 
		connect(this, &client_browse_widget::visibilityChanged, this, &client_browse_widget::on_visibility_changed);

		connect(m_button, &font_button::clicked, this, &client_browse_widget::on_search);
		connect(m_text, &text_edit_widget::submit_text, this, &client_browse_widget::on_search);
		connect(m_text, &text_edit_widget::textChanged, this, &client_browse_widget::on_text_changed);

		connect(m_chat_action, &QAction::triggered, this, &client_browse_widget::on_chat);
		connect(m_download_action, &QAction::triggered, this, &client_browse_widget::on_download);				
		connect(m_download_action_client, &QAction::triggered, this, &client_browse_widget::on_download_client);
		connect(m_download_to_action, &QAction::triggered, this, &client_browse_widget::on_download_to);
		connect(m_download_to_client_action, &QAction::triggered, this, &client_browse_widget::on_download_to_client);
		connect(m_mute_action, &QAction::triggered, [&]() { on_mute(true); });
		connect(m_unmute_action, &QAction::triggered, [&]() { on_mute(false); });
		connect(m_update_action, &QAction::triggered, [&]() { update(); });

		setWidget(splitter);
	}

	// Interface
	void client_browse_widget::on_create(const entity::ptr entity)
	{
		assert(thread_info::main());

		// HACK: We need this in a worker thread to avoid mutex re-entrancy
		QTimer::singleShot(0, [this]()
		{
			const auto main = main_window::get();
			const auto window = main->get_window();

			if (window)
			{
				const auto entity = entity_factory::create_client_search(m_entity);
				const auto widget = new client_search_widget(entity);

				widget->setAllowedAreas(Qt::TopDockWidgetArea);
				window->addDockWidget(Qt::TopDockWidgetArea, widget);
				window->tabifyDockWidget(this, widget);

				// HACK: Need to do this to raise the tab to foreground
				QTimer::singleShot(0, [widget]() { widget->raise(); });
			}
		});		
	}

	void client_browse_widget::on_update(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (!entity)
		{
			// Spam
			auto button = get_button(fa::bolt);
			if (button)
			{
				button->setEnabled(false);
				QTimer::singleShot(ui::refresh, [&]() { set_enabled(fa::bolt); });
			}

			// Name
			const auto endpoint = m_entity->get<endpoint_component>();
			m_button->setText(QString("%1:").arg(endpoint->get_name().c_str()));
		}

		// Model
		m_model->update(entity);
	}

	void client_browse_widget::on_chat()
	{
		assert(thread_info::main());

		m_tree->execute([&](const QModelIndex& index)
		{ 
			const auto entity = m_model->get_entity(index);
			if (entity)
				m_entity->call(callback_type::chat, callback_action::create, entity);
		});
	}

	void client_browse_widget::on_download()
	{
		assert(thread_info::main());

		const auto service = m_entity->get<client_service_component>();
		if (service->invalid())
			return;

		m_tree->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
				m_model->download(entity);
		});
	}

	void client_browse_widget::on_download_client()
	{
		assert(thread_info::main());

		const auto service = m_entity->get<client_service_component>();
		if (service->invalid())
			return;

		m_tree->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
				m_model->download_client(entity);
		});
	}

	void client_browse_widget::on_download_to()
	{
		assert(thread_info::main());

		const auto service = m_entity->get<client_service_component>();
		if (service->invalid())
			return;

		QFileDialog dialog;
		dialog.setWindowTitle("Download To...");
		dialog.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
		dialog.setOption(QFileDialog::DontUseNativeDialog, true);
		dialog.setFileMode(QFileDialog::Directory);
		dialog.setViewMode(QFileDialog::ViewMode::List);

		const auto io = m_entity->get<client_io_component>();
		dialog.setDirectory(QString::fromStdString(io->get_path()));

		if (dialog.exec())
		{
			const auto qpaths = dialog.selectedFiles();
			const auto& qpath = qpaths.at(0);
			const auto path = qpath.toStdString();

			m_tree->execute([&](const QModelIndex& index)
			{
				const auto entity = m_model->get_entity(index);
				if (entity)
					m_model->download_to(entity, path);
			});
		}
	}

	void client_browse_widget::on_download_to_client()
	{
		assert(thread_info::main());

		const auto service = m_entity->get<client_service_component>();
		if (service->invalid())
			return;

		QFileDialog dialog;
		dialog.setWindowTitle("Download To...");
		dialog.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
		dialog.setOption(QFileDialog::DontUseNativeDialog, true);
		dialog.setFileMode(QFileDialog::Directory);
		dialog.setViewMode(QFileDialog::ViewMode::List);

		const auto io = m_entity->get<client_io_component>();
		dialog.setDirectory(QString::fromStdString(io->get_path()));

		if (dialog.exec())
		{
			const auto qpaths = dialog.selectedFiles();
			const auto& qpath = qpaths.at(0);
			const auto path = qpath.toStdString();

			m_tree->execute([&](const QModelIndex& index)
			{
				const auto entity = m_model->get_entity(index);
				if (entity)
					m_model->download_to_client(entity, path);
			});
		}
	}

	// Utility
	void client_browse_widget::on_mute(const bool mute)
	{
		assert(thread_info::main());

		auto selected = m_tree->selectionModel()->selectedRows();

		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = m_model->get_entity(sindex);
						if (entity)
						{
							const auto endpoint = entity->find<endpoint_component>();
							endpoint->set_mute(mute);

							// HACK: Associate m_entity and the client list version
							//
							if (m_entity->equals<endpoint_component>(entity))
							{								
								const auto endpoint = m_entity->get<endpoint_component>();
								endpoint->set_mute(mute);
							}

							// Callback
							m_entity->call(callback_type::client, callback_action::update, entity);
						}
					}
				}
			}
		}
	}

	// Event
	void client_browse_widget::showEvent(QShowEvent* event)
	{
		if (!visibleRegion().isEmpty())
		{
			m_text->setFocus();
			m_text->selectAll();
		}

		// Event
		entity_widget::showEvent(event);
	}

	void client_browse_widget::on_visibility_changed(bool visible)
	{
		assert(thread_info::main());

		if (visible && !visibleRegion().isEmpty())
		{
			m_text->setFocus();
			m_text->selectAll();
		}
	}

	void client_browse_widget::on_search()
	{
		assert(thread_info::main());

		// Spam
		if (!m_enabled || !m_button->isEnabled())
			return;

		const auto qtext = m_text->toPlainText().simplified();
		const auto qlength = static_cast<size_t>(qtext.length() - qtext.count(' '));
		if (qlength < search::min_text)
			return;

		// Search
		const auto main = main_window::get();
		const auto window = main->get_window();

		if (window)
		{
			const auto entity = entity_factory::create_client_search(m_entity);
			const auto widget = new client_search_widget(entity, qtext);
			widget->search();

			widget->setAllowedAreas(Qt::TopDockWidgetArea);
			window->addDockWidget(Qt::TopDockWidgetArea, widget);
			window->tabifyDockWidget(this, widget);

			// HACK: Need to do this to raise the tab to foreground
			QTimer::singleShot(0, [widget]() { widget->raise(); });
		}

		// Cursor
		m_text->clear();
		m_text->setFocus();

		// Spam
		m_enabled = false;		
		m_button->setEnabled(false);

		QTimer::singleShot(ui::enter, [this]()
		{
			m_enabled = true;
			const auto qtext = m_text->toPlainText().simplified();
			const auto enabled = static_cast<size_t>(qtext.length()) >= search::min_text;
			m_button->setEnabled(enabled);
		});
	}
	
	void client_browse_widget::on_text_changed()
	{
		assert(thread_info::main());
		
		const auto qtext = m_text->toPlainText().simplified();
		const auto enabled = m_enabled && static_cast<size_t>(qtext.length()) >= search::min_text;
		m_button->setEnabled(enabled);

		if (static_cast<size_t>(qtext.size()) > search::max_text)
		{
			const auto ltext = qtext.left(search::max_text);
			m_text->setPlainText(ltext);
		}
	}

	// Menu
	void client_browse_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		const auto index = m_tree->indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);

		// Mute
		const auto state = get_mute();
		if ((state & mute_state::unmuted) == mute_state::unmuted)
			menu.addAction(m_mute_action);

		// Unmute
		if ((state & mute_state::muted) == mute_state::muted)
			menu.addAction(m_unmute_action);

		if (has_endpoint())
			menu.addAction(m_chat_action);

		if (has_file())
		{
			auto download = make_menu("Download", fa::arrowdown);
			download->addAction(m_download_action_client); 
			download->addAction(m_download_action);
			menu.addMenu(download);

			auto download_to = make_menu("Download To...", fa::folderopen);
			download_to->addAction(m_download_to_client_action); 
			download_to->addAction(m_download_to_action);
			menu.addMenu(download_to);
		}

		if (!menu.isEmpty())
			menu.addSeparator();

		menu.addAction(m_update_action);

		menu.exec(m_tree->viewport()->mapToGlobal(pos));
	}

	// Has
	bool client_browse_widget::has_endpoint() const
	{
		assert(thread_info::main());

		auto selected = m_tree->selectionModel()->selectedRows();

		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = m_model->get_entity(sindex);
						if (entity && entity->has<endpoint_component>())
							return true;
					}
				}
			}
		}

		return false;
	}

	bool client_browse_widget::has_file() const
	{
		assert(thread_info::main());

		auto selected = m_tree->selectionModel()->selectedRows();

		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = m_model->get_entity(sindex);
						if (entity && entity->has<file_component>() || entity->has<folder_component>())
							return true;
					}
				}
			}
		}

		return false;
	}

	// Get
	size_t client_browse_widget::get_mute()
	{
		assert(thread_info::main());

		size_t state = mute_state::none;

		auto selected = m_tree->selectionModel()->selectedRows();

		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = m_model->get_entity(sindex);
						if (entity && entity->has<endpoint_component>())
						{
							const auto endpoint = entity->get<endpoint_component>();
							state |= endpoint->unmuted() ? mute_state::unmuted : mute_state::muted;
						}
					}
				}
			}
		}

		return state;
	}
}
