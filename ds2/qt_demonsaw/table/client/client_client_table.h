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

#ifndef _EJA_CLIENT_CLIENT_TABLE_
#define _EJA_CLIENT_CLIENT_TABLE_

#include <QWidget>

#include "model/client/client_client_model.h"
#include "window/table_window.h"

class QAction;

namespace eja
{
	class client_client_table final : public table_window
	{
	private:
		QAction* m_mute_action = nullptr;
		QAction* m_unmute_action = nullptr;

	private:
		// Interface
		virtual void create(QWidget* parent = 0) override;
		virtual void layout(QWidget* parent = 0) override;
		virtual void signal(QWidget* parent = 0) override;

		// Utility
		bool muteable() const { return muteable(selectionModel()->selectedIndexes()); }
		bool muteable(const QModelIndexList& selected) const;

		bool unmuteable() const { return unmuteable(selectionModel()->selectedIndexes()); }
		bool unmuteable(const QModelIndexList& selected) const;

	private slots:
		void show_menu(const QPoint& point);
		void mute();
		void unmute();

	public:
		client_client_table(QWidget* parent = 0);
		virtual ~client_client_table() override { }

		// Accessor
		virtual int get_sort_column() const override { return client_client_model::column::name; }
	};
}

#endif
