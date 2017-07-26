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

#ifndef _EJA_CLIENT_OPTION_PANE_
#define _EJA_CLIENT_OPTION_PANE_

#include "entity/entity_pane.h"

class QLineEdit;
class QSpinBox;
class QToolButton;

namespace eja
{
	class client_performance_pane final : public entity_pane
	{
	private:		
		// Max
		QSpinBox* m_max_downloads = nullptr;
		QToolButton* m_max_downloads_refresh = nullptr;

		QSpinBox* m_max_errors = nullptr;
		QToolButton* m_max_errors_refresh = nullptr;

		QSpinBox* m_max_retries = nullptr;
		QToolButton* m_max_retries_refresh = nullptr;

		QSpinBox* m_max_searches = nullptr;
		QToolButton* m_max_searches_refresh = nullptr;
		
		QSpinBox* m_max_uploads = nullptr;
		QToolButton* m_max_uploads_refresh = nullptr;

		// Priority
		QLineEdit* m_priority_ext = nullptr;
		QToolButton* m_priority_ext_clear = nullptr;

		QSpinBox* m_priority_size = nullptr;
		QToolButton* m_priority_size_refresh = nullptr;

		// Timeout
		QSpinBox* m_download_timeout = nullptr;
		QToolButton* m_download_timeout_refresh = nullptr;

		QSpinBox* m_socket_timeout = nullptr;
		QToolButton* m_socket_timeout_refresh = nullptr;

		QSpinBox* m_upload_timeout = nullptr;
		QToolButton* m_upload_timeout_refresh = nullptr;

		QSpinBox* m_ping_timeout = nullptr;
		QToolButton* m_ping_timeout_refresh = nullptr;

		// Threads
		QSpinBox* m_download_threads = nullptr;
		QToolButton* m_download_threads_refresh = nullptr;

		QSpinBox* m_upload_threads = nullptr;
		QToolButton* m_upload_threads_refresh = nullptr;

		QSpinBox* m_thread_size = nullptr;
		QToolButton* m_thread_size_refresh = nullptr;

	private:
		// Interface
		virtual void create() override;
		virtual void layout() override;
		virtual void signal() override;

	public:
		client_performance_pane(entity::ptr entity, QWidget* parent = 0);
		virtual ~client_performance_pane() override { }
	};
}

#endif
