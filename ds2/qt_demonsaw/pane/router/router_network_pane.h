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

#ifndef _EJA_ROUTER_EDIT_PANE_
#define _EJA_ROUTER_EDIT_PANE_

#include "entity/entity_pane.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QSpinBox;
class QTextEdit;

namespace eja
{
	class router_network_pane final : public entity_pane
	{
	private:
		// Router
		QCheckBox* m_enabled = nullptr;
		QLineEdit* m_name = nullptr;
		QComboBox* m_address = nullptr;
		QSpinBox* m_port = nullptr;

		// Security
		QTextEdit* m_passphrase = nullptr;
		QToolButton* m_passphrase_clear = nullptr;

		// Action
		QCheckBox* m_message = nullptr;
		QCheckBox* m_public_group = nullptr;
		QCheckBox* m_transfer = nullptr;

		// Options
		QTextEdit* m_motd = nullptr;
		QToolButton* m_motd_clear = nullptr;

		QLineEdit* m_redirect = nullptr;
		QToolButton* m_redirect_clear = nullptr;

	protected:
		// Interface
		virtual void create() override;
		virtual void layout() override;
		virtual void signal() override;

	public:
		router_network_pane(entity::ptr entity, QWidget* parent = 0);
		virtual ~router_network_pane() override { }
	};
}

#endif
