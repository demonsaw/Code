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

#include <memory>
#include <vector>
#include <boost/atomic.hpp>

#include "component/component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	// Component
	class transfer_component final : public component
	{
		make_factory(transfer_component);

	private:
		boost::atomic<u64> m_size;
		boost::atomic<u64> m_speed_size;
		boost::atomic<size_t> m_chunk;
		boost::atomic<size_t> m_shards;

		bool m_stopped = false;
		bool m_valid = true;

	public:
		transfer_component() : m_size(0), m_speed_size(0), m_chunk(0), m_shards(0) { }
		transfer_component(const transfer_component& comp);

		// Operator
		transfer_component& operator=(const transfer_component& comp);

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return m_valid; }

		void resume();
		void stop();

		// Utility
		bool empty() const { return !m_size; }
		bool progress() const { return m_speed_size > 0; }

		bool complete() const;
		bool incomplete() const { return !complete(); }
		bool stopped() const { return m_stopped; }

		void add_size(const u64 size) { m_size += size; m_speed_size += size; }
		void sub_size(const u64 size) { m_size -= size; m_speed_size -= size; }

		void add_shards(const size_t shards) { m_shards += shards; }
		void sub_shards(const size_t shards) { m_shards -= shards; }

		// Has		
		bool has_size() const { return m_size > 0; }
		bool has_chunk() const { return m_chunk > 0; }
		bool has_shards() const { return m_shards > 0; }

		// Set
		void set_size(const u64 size) { m_size = size; }
		void set_chunk(const size_t chunk) { m_chunk = chunk; }
		void set_shards(const size_t shards) { m_shards = shards; }

		// Get
		u64 get_size() const { return m_size; }
		size_t get_chunk() const { return m_chunk; }
		size_t get_shards() const { return m_shards; }

		size_t get_speed() const;
		std::string get_time() const;

		double get_percent() const { return get_ratio() * 100.0; }
		double get_ratio() const;
	};
}

#endif
