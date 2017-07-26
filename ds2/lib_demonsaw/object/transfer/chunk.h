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

#ifndef _EJA_CHUNK_
#define _EJA_CHUNK_

#include <memory>
#include <string>

#include "object/object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class chunk : public object
	{
	private:
		u64 m_size = 0;
		u64 m_offset = 0;
		bool m_ack = false;
		std::string m_data;

	public:
		using ptr = std::shared_ptr<chunk>;

	public:
		chunk() { }
		chunk(const eja::chunk& chunk) : object(chunk), m_size(chunk.m_size), m_offset(chunk.m_offset), m_data(chunk.m_data) { }
		chunk(const u64 size) : m_size(size) { }
		chunk(const u64 size, const u64 offset) : m_size(size), m_offset(offset) { }
		virtual ~chunk() override { }

		// Interface
		virtual void clear() override;
		void shrink();

		// Utility
		virtual bool valid() const override { return object::valid() && has_size(); }		

		bool has_ack() const { return m_ack; }
		bool has_size() const { return m_size > 0; }
		bool has_data() const { return !m_data.empty(); }		

		// Mutator
		void set_size(const u64 size) { m_size = size; }
		void set_offset(const u64 offset) { m_offset = offset; }
		void set_ack(const bool ack) { m_ack = ack; }
		void set_data(const std::string& data) { m_data = data; }
		void set_data() { m_data.clear(); }

		// Accessor
		const u64 get_size() const { return m_size; }
		const u64 get_offset() const { return m_offset; }		
		const std::string& get_data() const { return m_data; }

		// Static
		static ptr create() { return std::make_shared<chunk>(); }
		static ptr create(const eja::chunk& chunk) { return std::make_shared<eja::chunk>(chunk); }
		static ptr create(const chunk::ptr chunk) { return std::make_shared<eja::chunk>(*chunk); }
		static ptr create(const u64 size) { return std::make_shared<chunk>(size); }
		static ptr create(const u64 size, const u64 offset) { return std::make_shared<chunk>(size, offset); }
	};

	// Container
	derived_type(chunk_list, mutex_list<chunk>);
	derived_type(chunk_map, mutex_map<std::string, chunk>);
	derived_type(chunk_queue, mutex_queue<chunk>);
	derived_type(chunk_vector, mutex_vector<chunk>);
}

#endif
