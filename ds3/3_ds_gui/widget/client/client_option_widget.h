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

#ifndef _EJA_CLIENT_OPTION_WIDGET_H_
#define _EJA_CLIENT_OPTION_WIDGET_H_

#include "entity/entity_widget.h"

class QCheckBox;
class QComboBox;
class QGridLayout;
class QHBoxLayout;
class QLineEdit;
class QSpinBox;
class QTextEdit;
class QPushButton;
class QRadioButton;
class QVBoxLayout;

namespace eja
{
	class client_option_widget final : public entity_widget
	{
	private:
		QVBoxLayout* m_layout = nullptr;

		// Client
		QLineEdit* m_name = nullptr;
		QPushButton* m_name_color = nullptr;
		QPushButton* m_name_default = nullptr;

		// Theme
		QLineEdit* m_theme = nullptr;
		QPushButton* m_theme_edit = nullptr;
		QPushButton* m_theme_update = nullptr;
		QPushButton* m_theme_default = nullptr;

		// Option
		QCheckBox* m_option_beep = nullptr;
		QCheckBox* m_option_flash = nullptr;

		// File/Folder
		QLineEdit* m_save_path = nullptr;
		QPushButton* m_save_edit = nullptr;
		QPushButton* m_save_default = nullptr;

		QComboBox* m_io_hash = nullptr;
		QTextEdit* m_io_salt = nullptr;

		// Network
		QCheckBox* m_ghosting_browse = nullptr;
		QCheckBox* m_ghosting_chat = nullptr;
		QCheckBox* m_ghosting_pm = nullptr;
		QCheckBox* m_ghosting_search = nullptr;
		QCheckBox* m_ghosting_upload = nullptr;
		
		QSpinBox* m_network_chunk = nullptr;
		QSpinBox* m_network_downloads = nullptr;
		//QSpinBox* m_network_threads = nullptr;
		//QSpinBox* m_network_timeout = nullptr;
		QSpinBox* m_network_uploads = nullptr;
		QSpinBox* m_network_window = nullptr;

		//QRadioButton* m_network_none = nullptr;
		//QRadioButton* m_network_poll = nullptr;
		QRadioButton* m_network_tunnel = nullptr;
		QRadioButton* m_network_server = nullptr;

		QLineEdit* m_network_address = nullptr;
		QSpinBox* m_network_port = nullptr;
		QLineEdit* m_network_address_ext = nullptr;
		QSpinBox* m_network_port_ext = nullptr;
		/*QComboBox* m_network_interval = nullptr;*/
		//QSpinBox* m_network_sockets = nullptr;

		QRadioButton* m_trust_none = nullptr;
		QRadioButton* m_trust_router = nullptr;
		/*QRadioButton* m_trust_all = nullptr;*/

		// Security
		QSpinBox* m_prime_size = nullptr;
		/*QSpinBox* m_prime_buffer = nullptr;
		QSpinBox* m_prime_reuse = nullptr;
		QSpinBox* m_prime_threshold = nullptr;*/

		//QComboBox* m_session_alg = nullptr;		
		QComboBox* m_session_cipher = nullptr;
		QComboBox* m_session_key_size = nullptr;
		QComboBox* m_session_hash = nullptr;
		QSpinBox* m_session_iterations = nullptr;
		QTextEdit* m_session_salt = nullptr;

	private:
		// Interface
		virtual void on_update() override;

		// Utility		
		void client();
		void network();
		void security();
		void signal();
		void ui();

		// Set
		void set_color();

		// Get
		QHBoxLayout* get_hbox_layout() const;
		QVBoxLayout* get_vbox_layout() const;

	public:
		client_option_widget(const std::shared_ptr<entity> entity, QWidget* parent = 0);
	};
}

#endif
