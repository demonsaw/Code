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

#ifndef _EJA_CLIENT_STATUS_PANE_H_
#define _EJA_CLIENT_STATUS_PANE_H_

#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QString>

#include "entity/entity_callback.h"

class QTimer;

namespace eja
{
	class entity;

	class status_pane final : public QWidget
	{
	private:
		QPushButton* m_image = nullptr;
		QLabel* m_text = nullptr;
		QTimer* m_timer = nullptr;

	public:
		status_pane(QWidget* parent = 0);
		status_pane(const int image, const char* psz, QWidget* parent = 0);

		// Set
		void set(const int image, const char* psz);
		
		void set_image(const int image);
		void set_text(const char* psz) { m_text->setText(psz); }
		void set_text(const QString& str) { m_text->setText(str); }
		void set_tooltip(const char* psz) { m_image->setToolTip(psz); }
		void set_tooltip(const QString& str) { m_text->setText(str); }

		// Get
		QPushButton* get_image() const { return m_image; }
		QLabel* get_text() const { return m_text; }
		QTimer* get_timer() const { return m_timer; }
	};

	class client_status_pane final : public QStatusBar, public entity_callback
	{
		Q_OBJECT;

	private:
		status_pane** m_pane = nullptr;
		size_t m_size = 0;

	private:
		void on_clicked(const int image);
		void on_text(const std::shared_ptr<entity> entity);

	private slots:
		// Interface
		void on_add(const std::shared_ptr<entity> entity) { on_text(entity); }
		void on_remove(const std::shared_ptr<entity> entity) { on_text(entity); }
		void on_update(const std::shared_ptr<entity> entity);
		void on_clear(const std::shared_ptr<entity> entity);

	signals:
		void add(const std::shared_ptr<entity> entity); 
		void remove(const std::shared_ptr<entity> entity);
		void update(const std::shared_ptr<entity> entity);
		void clear(const std::shared_ptr<entity> entity);

	public:
		client_status_pane(const std::shared_ptr<entity> entity, QWidget* parent = 0);
		~client_status_pane();

		// Get
		size_t get_size() const { return m_size; }
		status_pane* get_pane(const size_t index) const { return (index < m_size) ? m_pane[index] : nullptr; }
	};
}

#endif