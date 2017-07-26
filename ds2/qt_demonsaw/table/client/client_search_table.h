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

#ifndef _EJA_CLIENT_SEARCH_TABLE_
#define _EJA_CLIENT_SEARCH_TABLE_

#include "object/io/file.h"
#include "model/client/client_search_model.h"
#include "window/table_window.h"

class QAction;
class QPoint;

namespace eja
{
	class client_search_table final : public table_window
	{	
	private:
		QAction* m_download_action = nullptr;
		QAction* m_remove_action = nullptr;
		QAction* m_clear_action = nullptr;

	protected:
		// Interface
		virtual void create(QWidget* parent = 0) override;
		virtual void layout(QWidget* parent = 0) override;
		virtual void signal(QWidget* parent = 0) override;

	protected slots:		
		virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
		void show_menu(const QPoint& point);

	public:
		client_search_table(QWidget* parent = 0);
		virtual ~client_search_table() override { }

		// Interface
		using table_window::add;
		void add(const std::string& keyword, const file_filter& filter);

		// Accessor
		virtual int get_sort_column() const override { return client_search_model::column::name; }
	};
}

#endif
