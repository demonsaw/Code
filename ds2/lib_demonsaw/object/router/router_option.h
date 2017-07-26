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

#ifndef _EJA_ROUTER_OPTION_
#define _EJA_ROUTER_OPTION_

#include <memory>
#include <string>

#include "object/option.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class router_option : public option
	{
	protected:
		// Max		
		size_t m_max_threads = default_router::num_threads;
		size_t m_max_transfers = default_router::num_transfers;

		// Timeout
		size_t m_client_timeout = default_timeout::router::num_client;
		size_t m_queue_timeout = default_timeout::router::num_queue;
		size_t m_spam_timeout = default_timeout::router::num_spam;
		size_t m_transfer_timeout = default_timeout::router::num_transfer;				

		// Transfer
		size_t m_chunk_size = default_chunk::num_size;
		size_t m_chunk_buffer = default_chunk::num_buffer;
		size_t m_chunk_drift = default_chunk::num_drift;

		// Feature
		bool m_message_router = default_router::action::message;
		bool m_public_router = default_router::action::public_group;
		bool m_transfer_router = default_router::action::transfer;

		// Options
		std::string m_message;
		std::string m_redirect;
		size_t m_thread_pool = default_router::thread_pool;

	public:
		using ptr = std::shared_ptr<router_option>;

	public:
		router_option() { }
		router_option(const eja::router_option& router_option) : option(router_option), m_max_threads(router_option.m_max_threads), m_max_transfers(router_option.m_max_transfers), m_client_timeout(router_option.m_client_timeout), m_transfer_timeout(router_option.m_transfer_timeout), m_chunk_size(router_option.m_chunk_size), m_chunk_buffer(router_option.m_chunk_buffer), m_chunk_drift(router_option.m_chunk_drift), m_message_router(router_option.m_message_router), m_transfer_router(router_option.m_transfer_router) { }
		router_option(const std::string& id) : option(id) { }
		router_option(const char* id) : option(id) { }
		virtual ~router_option() override { }

		// Interface
		virtual void clear() override;

		// Mutator
		void set_max_threads(const size_t max_threads) { m_max_threads = max_threads; }
		void set_max_transfers(const size_t max_transfers) { m_max_transfers = max_transfers; }

		void set_client_timeout(const size_t client_timeout) { m_client_timeout = client_timeout; }		
		void set_queue_timeout(const size_t queue_timeout) { m_queue_timeout = queue_timeout; }
		void set_spam_timeout(const size_t spam_timeout) { m_spam_timeout = spam_timeout; }
		void set_transfer_timeout(const size_t transfer_timeout) { m_transfer_timeout = transfer_timeout; }

		void set_chunk_size(const size_t chunk_size) { m_chunk_size = chunk_size; }
		void set_chunk_buffer(const size_t chunk_buffer) { m_chunk_buffer = chunk_buffer; }
		void set_chunk_drift(const size_t chunk_drift) { m_chunk_drift = chunk_drift; }
		
		void set_message_router(const bool value) { m_message_router = value; }
		void set_public_router(const bool value) { m_public_router = value; }
		void set_transfer_router(const bool value) { m_transfer_router = value; }
		
		void set_message(const std::string& message) { m_message = message; }
		void set_message() { m_message.clear(); }

		void set_redirect(const std::string& redirect) { m_redirect = redirect; }
		void set_redirect() { m_redirect.clear(); }

		void set_thread_pool(const size_t thread_pool) { m_thread_pool = thread_pool; }

		// Accessor
		size_t get_max_threads() const { return m_max_threads; }
		size_t get_max_transfers() const { return m_max_transfers; }

		size_t get_client_timeout() const { return m_client_timeout; }
		size_t get_queue_timeout() const { return m_queue_timeout; }
		size_t get_spam_timeout() const { return m_spam_timeout; }
		size_t get_transfer_timeout() const { return m_transfer_timeout; }

		size_t get_chunk_size() const { return m_chunk_size; }
		size_t get_chunk_buffer() const { return m_chunk_buffer; }
		size_t get_chunk_drift() const { return m_chunk_drift; }

		bool get_message_router() const { return m_message_router; }
		bool get_public_router() const { return m_public_router; }
		bool get_transfer_router() const { return m_transfer_router; }

		bool has_message() const { return !m_message.empty(); }
		const std::string& get_message() const { return m_message; }

		bool has_redirect() const { return !m_redirect.empty(); }
		const std::string& get_redirect() const { return m_redirect; }

		size_t get_thread_pool() const { return m_thread_pool; }

		// Static
		static ptr create() { return std::make_shared<router_option>(); }
		static ptr create(const eja::router_option& router_option) { return std::make_shared<eja::router_option>(router_option); }
		static ptr create(const router_option::ptr router_option) { return std::make_shared<eja::router_option>(*router_option); }
		static ptr create(const std::string& id) { return std::make_shared<router_option>(id); }
		static ptr create(const char* id) { return std::make_shared<router_option>(id); }
	};
}

#endif
