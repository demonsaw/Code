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

#ifndef _EJA_ENTITY_WIDGET_H_
#define _EJA_ENTITY_WIDGET_H_

#include <cassert>
#include <map>

#include <QDockWidget>
#include <QSortFilterProxyModel>
#include <QToolBar>
#include <QToolButton>

#include "component/callback/callback_type.h"
#include "entity/entity_callback.h"
#include "font/font_button.h"
#include "utility/value.h"

class QCloseEvent;
class QLabel;
class QMenu;
class QString;

namespace eja
{
	class entity;
	class font_trigger;

	class entity_widget : public QDockWidget, public entity_callback
	{
		Q_OBJECT

	protected:
		QLabel* m_icon = nullptr; 
		QLabel* m_title = nullptr;
		QString m_title_text;

		QWidget* m_titlebar = nullptr;		
		QToolBar* m_toolbar = nullptr;
		font_trigger* m_trigger = nullptr;

		using font_button_map = std::map<int, font_button*>;
		font_button_map m_buttons;

	protected:
		// Interface
		template <typename T, typename M>
		void init(T* widget, M* model);

		// Utility
		template <typename T>
		void add_button(const char* tooltip, const int font, T t);

		QAction* make_action(const char* text, const char* tooltip = nullptr);
		QAction* make_action(const char* text, const int font, const char* tooltip = nullptr, const size_t size = 14);
		QAction* make_action(const char* text, const char* icon, const char* tooltip = nullptr);
		font_button* make_button(const char* tooltip, const int font);
		QMenu* make_menu(const char* text, const int font, const size_t size = 14);

		void popout();

		// Event
		virtual void closeEvent(QCloseEvent* event) override;
		virtual bool eventFilter(QObject* object, QEvent* event) override;

		// Set
		void set_modified(const bool value = true) const;

	protected slots:
		virtual void on_create() { on_create(nullptr); }
		virtual void on_create(const std::shared_ptr<entity> entity) { assert(false); }		

		virtual void on_destroy() { on_destroy(nullptr); }
		virtual void on_destroy(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_init() { on_init(nullptr); }
		virtual void on_init(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_update() { on_update(nullptr); }
		virtual void on_update(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_shutdown() { on_shutdown(nullptr); }
		virtual void on_shutdown(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_add() { on_add(nullptr); }
		virtual void on_add(const std::shared_ptr<entity> entity) { assert(false); }
		
		virtual void on_remove() { on_remove(nullptr); }
		virtual void on_remove(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_clear() { on_clear(nullptr); }
		virtual void on_clear(const std::shared_ptr<entity> entity) { assert(false); }

		void on_set_enabled(const int font);
		void on_top_level_changed(bool topLevel);

	signals:
		void create();
		void create(const std::shared_ptr<entity> entity);

		void destroy();
		void destroy(const std::shared_ptr<entity> entity);

		void init();
		void init(const std::shared_ptr<entity> entity);

		void update();
		void update(const std::shared_ptr<entity> entity);

		void shutdown();
		void shutdown(const std::shared_ptr<entity> entity);

		void add();
		void add(const std::shared_ptr<entity> entity);

		void remove();
		void remove(const std::shared_ptr<entity> entity);

		void clear();
		void clear(const std::shared_ptr<entity> entity);

		void set_enabled(const int font);

	public:
		entity_widget(const QString& title, const int fa, const callback_type type = callback_type::none, QWidget* parent = 0);
		entity_widget(const std::shared_ptr<entity> entity, const QString& title, const int fa, const callback_type type = callback_type::none, QWidget* parent = 0);

		// Has
		bool has_button(const int font) const;
		bool has_trigger() const { return m_trigger != nullptr; }

		// Set
		void set_trigger() { m_trigger = nullptr; }
		void set_trigger(const font_trigger* trigger) { m_trigger = const_cast<font_trigger*>(trigger); }

		// Get
		const font_button* get_button(const int font) const;
		font_button* get_button(const int font) { return const_cast<font_button*>(reinterpret_cast<const entity_widget*>(this)->get_button(font)); }

		const QToolBar* get_toolbar() const { return m_toolbar; }
		QToolBar* get_toolbar() { return m_toolbar; }

		const font_trigger* get_trigger() const { return m_trigger; }
		font_trigger* get_trigger() { return m_trigger; }
	};

	// Interface
	template <typename T, typename M>
	void entity_widget::init(T* widget, M* model)
	{
		QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
		proxy->setSortRole(Qt::UserRole);
		proxy->setSourceModel(model);
		widget->setModel(proxy);
		model->init();
	}

	// Utility
	template <typename T>
	void entity_widget::add_button(const char* tooltip, const int font, T t)
	{
		auto button = new font_button(font, this);
		button->setObjectName("menubar");
		button->setFont(QFont(demonsaw::font_awesome));
		button->setText(QString(font));
		button->setToolTip(tooltip);
		
		m_toolbar->addWidget(button);
		const auto pair = std::make_pair(font, button);
		m_buttons.insert(pair);

		// Signal
		connect(button, &QToolButton::clicked, this, t);
	}
}

#endif
