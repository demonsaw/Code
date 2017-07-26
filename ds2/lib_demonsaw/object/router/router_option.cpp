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

#include "router_option.h"

namespace eja
{
	// Interface
	void router_option::clear()
	{
		option::clear();
		
		// Max
		m_max_threads = default_router::num_threads;
		m_max_transfers = default_router::num_transfers;

		// Timeout
		m_client_timeout = default_timeout::router::num_client;
		m_queue_timeout = default_timeout::router::num_queue; 
		m_spam_timeout = default_timeout::router::num_spam;
		m_transfer_timeout = default_timeout::router::num_transfer;				

		// Transfer
		m_chunk_size = default_chunk::num_size;
		m_chunk_buffer = default_chunk::num_buffer;
		m_chunk_drift = default_chunk::num_drift;

		// API
		m_message_router = default_router::action::message;
		m_public_router = default_router::action::public_group;
		m_transfer_router = default_router::action::transfer;

		// Options
		m_message.clear();
		m_redirect.clear();
		m_thread_pool = default_router::thread_pool;
	}
}
