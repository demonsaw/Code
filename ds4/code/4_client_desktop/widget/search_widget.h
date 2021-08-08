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

#ifndef _EJA_SEARCH_WIDGET_H_
#define _EJA_SEARCH_WIDGET_H_

#include <set>
#include <string>
#include <boost/filesystem.hpp>

#include "component/message/message_component.h"
#include "entity/entity_dock_widget.h"

class QAction;
class QPoint;

namespace eja
{
	class search_model;
	class entity;
	class entity_tree_view_ex;
	class id_set;

	class search_widget final : public entity_dock_widget
	{
		Q_OBJECT

	private:
		QAction* m_download_action = nullptr;
		QAction* m_download_to_action = nullptr;
		QAction* m_clear_action = nullptr;
		QAction* m_refresh_action = nullptr;
		QAction* m_close_action = nullptr;

		std::set<std::string> m_messages;
		entity_tree_view_ex* m_tree = nullptr;
		search_model* m_model = nullptr;

	private:
		// Interface
		virtual void on_create() override;
		virtual void on_update() override;

		virtual void on_add(const std::shared_ptr<entity> entity) override;		
		virtual void on_clear() override;

		// Utility
		void reset();

		void download(const std::shared_ptr<entity>& entity, const boost::filesystem::path& path);		
		void open_tabs() const;

		// Event
		virtual void closeEvent(QCloseEvent* event) override;
		virtual bool eventFilter(QObject* object, QEvent* event) override;

	private slots:
		void on_double_click(const QModelIndex& index);		
		void show_menu(const QPoint& point);

		void on_download();
		void on_download_to();

	public:
		search_widget(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr);
	};
}

#endif
