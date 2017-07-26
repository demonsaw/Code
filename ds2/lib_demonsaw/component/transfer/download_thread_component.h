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

#ifndef _EJA_DOWNLOAD_THREAD_COMPONENT_
#define _EJA_DOWNLOAD_THREAD_COMPONENT_

#include <atomic>
#include <memory>
#include <set>

#include "component/machine_component.h"
#include "entity/entity.h"
#include "http/http_socket.h"
#include "system/mutex/mutex.h"
#include "system/type.h"

namespace eja
{
	class download_thread_component final : public machine_component, public std::enable_shared_from_this<download_thread_component>
	{
	private:
		size_t m_size = 0;
		size_t m_thread_size = 0;
		entity_vector m_entities;
		std::atomic<size_t> m_quit;

		mutex m_mutex;
		std::set<u64> m_chunks;

		http_socket::ptr m_socket = http_socket::create();				
		bool m_timeout = false;
		bool m_retry = false;

	public:
		using ptr = std::shared_ptr<download_thread_component>;

	private:
		download_thread_component(const size_t size = 1);

		// Utility
		void close();

		// Callback
		virtual bool on_start() override;
		virtual bool on_restart() override;
		virtual bool on_stop() override;
		virtual bool on_cancel() override;

		virtual bool on_handshake() override; 
		virtual bool on_transfer() override;
		virtual bool on_quit() override;

	public:
		virtual ~download_thread_component() override { }

		// Interface
		virtual void init() override;
		virtual void shutdown() override;

		virtual void start() override;
		virtual void stop() override;
		virtual bool timeout() const override;

		void quit();
		bool add_chunk(const u64& chunk);
		u64 get_next_chunk(const u64& chunk);

		// Utility
		bool running() const;
		bool stoppable() const;		
		bool resumable() const;		
		bool removeable() const;

		// Mutator
		virtual void set_status(const eja::status status) override;

		// Accessor
		size_t get_size() const { return m_thread_size; }

		// Static
		static ptr create(const size_t size = 1) { return ptr(new download_thread_component(size)); }
	};
}

#endif
