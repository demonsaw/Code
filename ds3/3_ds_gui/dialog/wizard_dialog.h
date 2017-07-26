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

#ifndef _EJA_WIZARD_DIALOG_H_
#define _EJA_WIZARD_DIALOG_H_

#include <QDialog>

#include "pane/wizard/wizard_client_pane.h"
#include "pane/wizard/wizard_router_pane.h"
#include "pane/wizard/wizard_welcome_pane.h"

class QPushButton;
class QStackedWidget;

namespace eja
{
	enum class wizard_status { none, quick, done };

	class wizard_dialog final : public QDialog
	{
		Q_OBJECT

	private:
		wizard_status m_status = wizard_status::none;

		QWidget* m_widget = nullptr;
		QStackedWidget* m_stack = nullptr;		

		wizard_client_pane* m_client = nullptr;
		wizard_router_pane* m_router = nullptr;
		wizard_welcome_pane* m_welcome = nullptr;

	public:
		wizard_dialog(QWidget* parent = 0);

		// Utility
		void back();
		void next();
		void quick();
		void done();

		// Is
		bool is_client() const { return m_welcome->is_client(); }
		bool is_router() const { return m_welcome->is_router(); }

		// Get
		wizard_status get_status() const { return m_status; }

		u32 get_color() const { return is_client() ? m_client->get_color() : m_router->get_color(); }
		std::string get_name() const { return is_client() ? m_client->get_name() : m_router->get_name(); }
		std::string get_address() const { return is_client() ? m_client->get_address() : m_router->get_address(); }
		u16 get_port() const { return is_client() ? m_client->get_port() : m_router->get_port(); }
	};
}

#endif
