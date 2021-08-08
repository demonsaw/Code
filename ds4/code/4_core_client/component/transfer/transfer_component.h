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

#ifndef _EJA_TRANSFER_COMPONENT_H_
#define _EJA_TRANSFER_COMPONENT_H_

#include <deque>
#include <list>
#include <set>

#include "component/component.h"

#include "object/transfer.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	make_thread_safe_component(chunk_list_component, std::list<u32>);
	make_thread_safe_component(chunk_set_component, std::set<u32>);

	class transfer_component : public component
	{
		make_factory(transfer_component);

		enum bits : size_t { none, cancelled = bit(1), done = bit(2), errored = bit(3), paused = bit(4), running = bit(5) };

	private:
		size_t m_buffer_size = 0;
		size_t m_chunk_size = 0;
		size_t m_retry = 0; 
		size_t m_seeds = 0;
		size_t m_wait = 0;

		u64 m_progress = 0; 
		u64 m_size = 0;		

	public:
		transfer_component() { }

		// Interface
		void update(const u64 size);

		// Utility
		bool empty() const { return !has_size(); }

		void start();
		void stop();

		void pause();
		void reset();
		void resume();
		void retry() { ++m_retry; }
		void seed() { ++m_seeds; }
		void wait() { ++m_wait; }

		void cancel();
		void error();
		void finish();

		// Has
		bool has_buffer_size() const { return m_buffer_size > 0; }		
		bool has_chunk_size() const { return m_chunk_size > 0; }
		bool has_chunks() const;

		bool has_progress() const { return m_progress > 0; }
		bool has_retry() const { return m_retry > 0; }
		bool has_seeds() const { return m_seeds > 0; }
		bool has_size() const { return m_size > 0; }
		bool has_wait() const { return m_wait > 0; }

		// Is		
		bool is_cancelled() const { return is_mode(bits::cancelled); }
		bool is_done() const;
		bool is_errored() const { return is_mode(bits::errored); }
		bool is_paused() const { return is_mode(bits::paused); }
		bool is_running() const { return is_mode(bits::running); }

		bool is_download() const;
		bool is_upload() const { return !is_download(); }

		// Set
		void set_buffer_size(const size_t buffer_size) { m_buffer_size = buffer_size; }
		void set_chunk_size(const size_t chunk_size) { m_chunk_size = chunk_size; }
		void set_progress(const u64 progress) { m_progress = progress; }

		void set_retry(const size_t retry) { m_retry = retry; }
		void set_retry() { m_retry = 0; }

		void set_seeds(const size_t seed) { m_seeds = seed; }
		void set_seeds() { m_seeds = 0; }

		void set_wait(const size_t wait) { m_wait = wait; }
		void set_wait() { m_wait = 0; }

		void set_cancelled(const bool value) { set_mode(bits::cancelled, value); }
		void set_done(const bool value) { set_mode(bits::done, value); }
		void set_errored(const bool value) { set_mode(bits::errored, value); }
		void set_paused(const bool value) { set_mode(bits::paused, value); }
		void set_running(const bool value) { set_mode(bits::running, value); }

		// Get
		size_t get_buffer_size() const { return m_buffer_size; }
		size_t get_chunk_size() const { return m_chunk_size; }
		size_t get_chunks() const;
		size_t get_retry() const { return m_retry; }
		size_t get_seeds() const { return m_seeds; }
		size_t get_wait() const;

		u64 get_progress() const { return m_progress; }		
		u64 get_size() const { return m_size; }
		u64 get_remaining() const;

		size_t get_speed() const;
		std::string get_time() const;

		double get_percent() const { return get_ratio() * 100.0; }
		double get_ratio() const;
	};

	class transfer_list_component final : public thread_safe_ex<component, std::deque<std::shared_ptr<entity>>>
	{
		make_factory(transfer_list_component);

	private:
		size_t m_next = 0;

	public:
		transfer_list_component() { }

		// Utility
		virtual void clear() override;
		virtual bool valid() const override { return !empty(); }

		// Get
		std::shared_ptr<entity> get_entity();
	};
}

#endif
