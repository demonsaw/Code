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

#ifndef _EJA_ENTITY_WINDOW_
#define _EJA_ENTITY_WINDOW_

#include <QLabel>
#include <QSortFilterProxyModel>
#include <QStackedWidget>
#include <QTabWidget>
#include <QToolBox>
#include <QWidget>

#include "entity/entity_action.h"
#include "entity/entity.h"
#include "system/mutex/mutex_value.h"
#include "window/status_window.h"

class QTabWidget;

namespace eja
{
	class entity_window : public QWidget, public entity_value
	{
		Q_OBJECT

	protected:
		entity_action* m_action = nullptr;
		QAction* m_dashboard_action = nullptr;
		QAction* m_activity_action = nullptr;

		QTabWidget* m_main_tab = nullptr;
		QTabWidget* m_activity_tab = nullptr;
		QWidget* m_activity_widget = nullptr;
		QToolBox* m_toolbox = nullptr;		

		QStackedWidget* m_main_toolbar = nullptr;
		QStackedWidget* m_activity_toolbar = nullptr;
		status_window* m_statusbar = nullptr;

	protected:
		// Interface
		template <typename T, typename M> void init(T* t, M* m);

		// Utility
		virtual void create_tabs();
		virtual void create_dashboard();
		virtual void create_activity();		

		virtual void create_actions();
		virtual void create_toolbars();
		virtual void create_statusbar(QWidget* statusbar = nullptr) = 0;
		virtual void create_layouts() = 0;
		virtual void create_slots();

	protected slots:
		void main_tab_changed(int index) { m_main_toolbar->setCurrentIndex(index); }
		void activity_tab_changed(int index) { m_activity_toolbar->setCurrentIndex(index); }

	public:
		entity_window(const entity::ptr entity, QWidget* parent = 0, entity_action* action = nullptr) : entity_value(entity), QWidget(parent), m_action(action) { }
		virtual ~entity_window() { }

		// Interface
		virtual void init() { }
		virtual void shutdown() { }
		virtual void update() { }

		// Utility
		virtual void show() { }

		// Accessor
		entity_action* get_action() const { return m_action; }	

		QTabWidget* get_main_tab() const { return m_main_tab; }		
		bool is_main_active(const size_t index) { return m_main_tab->currentIndex() == index; }

		QTabWidget* get_activity_tab() const { return m_activity_tab; }
		bool is_activity_active(const size_t index) { return m_activity_tab->currentIndex() == index; }

		status_window* get_statusbar() const { return m_statusbar; }

	public slots:
		void show_options(bool value) { m_toolbox->setVisible(value); }
		void show_details(bool value) { m_activity_widget->setVisible(value); }
	};

	// Interface
	template <typename T, typename M>
	void entity_window::init(T* window, M* model)
	{
		// Proxy
		QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
		proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
		proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
		proxy->setSortRole(Qt::UserRole);
		proxy->setSourceModel(model);
		window->setModel(proxy);

		// Model
		model->init();
		window->init();
	}
}

#endif
