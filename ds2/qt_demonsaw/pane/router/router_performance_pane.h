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

#ifndef _EJA_ROUTER_OPTION_PANE_
#define _EJA_ROUTER_OPTION_PANE_

#include "entity/entity_pane.h"

class QLineEdit;
class QSpinBox;
class QToolButton;

namespace eja
{
	class router_performance_pane final : public entity_pane
	{
	private:
		// Timeout		
		QSpinBox* m_client_timeout = nullptr;
		QToolButton* m_client_timeout_refresh = nullptr;

		QSpinBox* m_queue_timeout = nullptr;
		QToolButton* m_queue_timeout_refresh = nullptr;

		QSpinBox* m_socket_timeout = nullptr;
		QToolButton* m_socket_timeout_refresh = nullptr;

		QSpinBox* m_spam_timeout = nullptr;
		QToolButton* m_spam_timeout_refresh = nullptr;

		QSpinBox* m_transfer_timeout = nullptr;
		QToolButton* m_transfer_timeout_refresh = nullptr;

		// Transfer
		QSpinBox* m_chunk_buffer = nullptr;
		QToolButton* m_chunk_buffer_refresh = nullptr;

		QSpinBox* m_chunk_size = nullptr;
		QToolButton* m_chunk_size_refresh = nullptr;

		QSpinBox* m_chunk_drift = nullptr;
		QToolButton* m_chunk_drift_refresh = nullptr;		

		// Max
		QSpinBox* m_max_errors = nullptr;
		QToolButton* m_max_errors_refresh = nullptr;

		QSpinBox* m_max_threads = nullptr;
		QToolButton* m_max_threads_refresh = nullptr;

		QSpinBox* m_max_transfers = nullptr;
		QToolButton* m_max_transfers_refresh = nullptr;

	protected:
		// Interface
		virtual void create() override;
		virtual void layout() override;
		virtual void signal() override;

	public:
		router_performance_pane(entity::ptr entity, QWidget* parent = 0);
		virtual ~router_performance_pane() override { }
	};
}

#endif
