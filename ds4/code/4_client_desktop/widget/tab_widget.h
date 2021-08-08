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

#ifndef _EJA_TAB_WIDGET_H_
#define _EJA_TAB_WIDGET_H_

#include <memory>

#include <QPaintEvent>
#include <QWidget>

#include "entity/entity_callback.h"

namespace eja
{
	class tab_widget final : public QWidget, public entity_callback
	{
		Q_OBJECT;

	private:
		std::shared_ptr<entity> m_chat;
		std::shared_ptr<entity> m_pm;
		std::shared_ptr<entity> m_room;

	private:
		virtual void paintEvent(QPaintEvent *event) override;

	private slots:
		void on_clear();
		void on_clear_chat();

		void on_clear_pm(const std::shared_ptr<entity>& entity);
		void on_clear_pm();
		
		void on_clear_room(const std::shared_ptr<entity>& entity);
		void on_clear_room();

		void on_chat(const std::shared_ptr<entity>& entity);
		void on_pm(const std::shared_ptr<entity>& entity);
		void on_room(const std::shared_ptr<entity>& entity);

	signals:
		void clear();
		void chat(const std::shared_ptr<entity>& entity);
		void pm(const std::shared_ptr<entity>& entity);
		void room(const std::shared_ptr<entity>& entity);

	public:
		tab_widget(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr);

		// Get
		std::shared_ptr<entity> get_chat_entity() const { return m_chat; }
		std::shared_ptr<entity> get_pm_entity() const { return m_pm; }
		std::shared_ptr<entity> get_room_entity() const { return m_room; }

	signals:
		void clear_chat();
		void clear_pm(const std::shared_ptr<entity>& entity);
		void clear_pm();
		void clear_room(const std::shared_ptr<entity>& entity);
		void clear_room();
	};
}

#endif
