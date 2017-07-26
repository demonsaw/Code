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

#ifndef _EJA_CLIENT_NETWORK_PANE_
#define _EJA_CLIENT_NETWORK_PANE_

#include "entity/entity_pane.h"
#include "resource/resource.h"

class QCheckBox;
class QLineEdit;
class QSpinBox;
class QTextEdit;
class QToolButton;

namespace eja
{
	class client_network_pane final : public entity_pane
	{
	private:
		// Client
		QCheckBox* m_enabled = nullptr;
		
		QLineEdit* m_name = nullptr;
		QToolButton* m_name_color = nullptr;

		QLineEdit* m_download_path = nullptr;
		QToolButton* m_download_edit = nullptr;
		QToolButton* m_download_refresh = nullptr;

		QCheckBox* m_partial = nullptr;

		// Router
		QLineEdit* m_address = nullptr;
		QToolButton* m_address_refresh = nullptr;

		QSpinBox* m_port = nullptr;
		QToolButton* m_port_refresh = nullptr;

		QTextEdit* m_passphrase = nullptr;
		QToolButton* m_passphrase_clear = nullptr;

		QCheckBox* m_motd = nullptr;

		// Type
		QCheckBox* m_browse = nullptr; 
		QCheckBox* m_chat = nullptr;
		QCheckBox* m_message = nullptr;
		QCheckBox* m_search = nullptr;
		QCheckBox* m_transfer = nullptr;		

	protected:
		// Interface
		virtual void create() override;
		virtual void layout() override;
		virtual void signal() override;

		// Utility
		void set_color();

	public:
		client_network_pane(const entity::ptr entity, QWidget* parent = 0);
		virtual ~client_network_pane() override { }
	};
}

#endif
