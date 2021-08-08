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

#ifndef _EJA_ENTITY_DOCK_WIDGET_H_
#define _EJA_ENTITY_DOCK_WIDGET_H_

#include <cassert>
#include <list>
#include <map>

#include <QAbstractItemView>
#include <QAction>
#include <QDockWidget>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include "entity/entity_callback.h"
#include "entity/entity_model.h"
#include "font/font_button.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "utility/value.h"

class QCloseEvent;
class QLabel;
class QPoint;
class QString;
class QTableView;
class QTreeView;

namespace eja
{
	class entity;
	class entity_dock_widget;

	class entity_dock_widget : public QDockWidget, public entity_callback
	{
		Q_OBJECT

	protected:
		using column_names = std::vector<QString>;
		using column_indices = std::vector<size_t>;

		int m_fa = 0;
		QLabel* m_icon = nullptr;
		QLabel* m_title = nullptr;
		QString m_title_text;

		QWidget* m_titlebar = nullptr;
		QToolBar* m_toolbar = nullptr;

		using font_button_map = std::map<int, font_button*>;
		font_button_map m_buttons;

	protected:
		// Interface
		template <typename T>
		void init_ex(QAbstractItemView* widget, entity_model* model);
		void init_ex(QAbstractItemView* widget, entity_model* model);
		void init(QAbstractItemView* widget, entity_model* model);

		// Utility
		template <typename T>
		void add_button(const char* tooltip, const int icon, T t);

		QAction* make_action(const char* text, const int fa);
		QAction* make_action(const char* text, const char* icon, const char* tooltip = nullptr);
		font_button* make_button(const char* tooltip, const int icon);
		QMenu* make_menu(const char* text, const int fa);

		void popout();

		// Event
		virtual bool eventFilter(QObject* object, QEvent* event) override;

		template <typename T>
		void show_header_menu(T* view, const entity_model* model, const QPoint& pos) const;

		template <typename T>
		void show_header_menu(const column_names& names, const column_indices& indices, T* view, const entity_model* model, const QPoint& pos) const;

	protected slots:
		virtual void on_create() { on_create(nullptr); }
		virtual void on_create(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_destroy() { on_destroy(nullptr); }
		virtual void on_destroy(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_add() { on_add(nullptr); }
		virtual void on_add(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_remove() { on_remove(nullptr); }
		virtual void on_remove(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_update() { on_update(nullptr); }
		virtual void on_update(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_clear() { on_clear(nullptr); }
		virtual void on_clear(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_click(const QModelIndex& index) { }
		virtual void on_double_click(const QModelIndex& index) { }
		virtual void on_execute(const QModelIndex& index) { }

		void on_top_level_changed(bool topLevel);

	signals:
		void create();
		void create(const std::shared_ptr<entity> entity);

		void destroy();
		void destroy(const std::shared_ptr<entity> entity);

		void add();
		void add(const std::shared_ptr<entity> entity);

		void remove();
		void remove(const std::shared_ptr<entity> entity);

		void update();
		void update(const std::shared_ptr<entity> entity);

		void clear();
		void clear(const std::shared_ptr<entity> entity);

		void click(const QModelIndex& index);
		void double_click(const QModelIndex& index);
		void execute(const QModelIndex& index);

	public:
		entity_dock_widget(const int fa, QWidget* parent = nullptr) : entity_dock_widget("", fa, parent) { }
		entity_dock_widget(const QString& title, const int fa, QWidget* parent = nullptr);
		entity_dock_widget(const std::shared_ptr<entity>& entity, const int fa, QWidget* parent = nullptr) : entity_dock_widget(entity, "", fa, parent) { }
		entity_dock_widget(const std::shared_ptr<entity>& entity, const QString& title, const int fa, QWidget* parent = nullptr);

		// Has
		bool has_button(const int font) const;

		// Is
		bool is_visible() const { return !visibleRegion().isEmpty();}
		bool is_visible(const void* ptr) const { return ptr == this; }

		// Get
		const font_button* get_button(const int font) const;
		font_button* get_button(const int font) { return const_cast<font_button*>(reinterpret_cast<const entity_dock_widget*>(this)->get_button(font)); }

		int get_fa() const { return m_fa; }

		const QToolBar* get_toolbar() const { return m_toolbar; }
		QToolBar* get_toolbar() { return m_toolbar; }
	};

	// Interface
	template <typename T>
	void entity_dock_widget::init_ex(QAbstractItemView* widget, entity_model* model)
	{
		T* proxy = new T(this);
		proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
		proxy->setSortRole(Qt::UserRole);
		proxy->setSourceModel(model);
		widget->setModel(proxy);
	}

	// Utility
	template <typename T>
	void entity_dock_widget::add_button(const char* tooltip, const int icon, T t)
	{
		// Font
		QFont font(font_family::font_awesome);
		font.setPixelSize(resource::get_large_font_size());

		// Button
		auto button = new font_button(icon, resource::get_small_icon_size(), this);
		button->setObjectName("menubar");
		button->setToolTip(tooltip);
		button->setFont(font);

		m_toolbar->addWidget(button);
		const auto pair = std::make_pair(icon, button);
		m_buttons.insert(pair);

		// Signal
		connect(button, &QPushButton::clicked, this, t);
	}

	template <typename T>
	void entity_dock_widget::show_header_menu(T* view, const entity_model* model, const QPoint& pos) const
	{
		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

		const auto& names = model->get_column_names();

		for (int i = 0; i < model->columnCount(); ++i)
		{
			const auto action = new QAction(names[i]);
			const auto checked = !view->isColumnHidden(i);

			if (checked)
			{
				const auto icon = icon_provider::get_icon(resource::get_icon_size(), fa::check);
				action->setIcon(icon);
			}

			connect(action, &QAction::triggered, [&, i, checked]() { view->setColumnHidden(i, checked); });

			menu.addAction(action);
		}

		menu.exec(view->mapToGlobal(pos));
	}

	template <typename T>
	void entity_dock_widget::show_header_menu(const column_names& names, const column_indices& indices, T* view, const entity_model* model, const QPoint& pos) const
	{
		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

		const auto size = std::min(names.size(), indices.size());

		for (int i = 0; i < size; ++i)
		{
			const auto index = indices[i];
			const auto action = new QAction(names[i]);
			const auto checked = !view->isColumnHidden(index);

			if (checked)
			{
				const auto icon = icon_provider::get_icon(resource::get_icon_size(), fa::check);
				action->setIcon(icon);
			}

			connect(action, &QAction::triggered, [&, index, checked]() { view->setColumnHidden(index, checked); });

			menu.addAction(action);
		}

		menu.exec(view->mapToGlobal(pos));
	}
}

#endif
