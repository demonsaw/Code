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

#ifndef _EJA_CHUNK_COMPONENT_
#define _EJA_CHUNK_COMPONENT_

#include <memory>
#include <string>

#include "component/component.h"
#include "entity/entity.h"
#include "object/transfer/chunk.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	// Component
	class chunk_component final : public chunk, public component
	{
	public:
		using ptr = std::shared_ptr<chunk_component>;

	private:
		chunk_component() { }
		chunk_component(const u64 size) : chunk(size) { }
		chunk_component(const u64 size, const u64 offset) : chunk(size, offset) { }

	public:		
		virtual ~chunk_component() override { }

		// Static
		static ptr create() { return ptr(new chunk_component()); }
		static ptr create(const u64 size) { return ptr(new chunk_component(size)); }
		static ptr create(const u64 size, const u64 offset) { return ptr(new chunk_component(size, offset)); }
	};

	// Container
	derived_type(chunk_list_component, list_component<chunk>);
	derived_type(chunk_map_component, map_component<std::string, chunk>);
	derived_type(chunk_set_component, set_component<chunk>);
	derived_type(chunk_vector_component, vector_component<chunk>);

	derived_type(chunk_entity_list_component, entity_list_component);
	derived_type(chunk_entity_map_component, entity_map_component);
	derived_type(chunk_entity_queue_component, entity_queue_component);
	derived_type(chunk_entity_set_component, entity_set_component);
	derived_type(chunk_entity_vector_component, entity_vector_component);

	derived_type(chunk_ack_component, map_component<size_t, chunk>);

	// Queue
	class chunk_queue_component : public queue_component<chunk>
	{
	protected:
		u64 m_file_size;
		u64 m_file_offset;		

	protected:
		chunk_queue_component(const u64 file_size, const u64 file_offset = 0) : m_file_size(file_size), m_file_offset(file_offset) { }

	protected:
		virtual ~chunk_queue_component() override { }

	public:
		// Utility
		virtual bool done() const { return empty() && (m_file_offset >= m_file_size); }

		// Accessor
		const u64 get_file_size() const { return m_file_size; }
		const u64 get_file_offset() const { return m_file_offset; }
	};

	class chunk_upload_queue_component final : public chunk_queue_component
	{
	private:		
		u64 m_chunk_size = default_chunk::num_size;
		u64 m_chunk_drift = default_chunk::num_drift;

	public:
		using ptr = std::shared_ptr<chunk_upload_queue_component>;

	protected:
		// Container
		using chunk_queue_component::push;
		value_type pop_push();

	private:
		chunk_upload_queue_component(const u64 file_size, const u64 file_offset = 0) : chunk_queue_component(file_size, file_offset) { }

	public:		
		virtual ~chunk_upload_queue_component() override { }

		// Container		
		value_type push();

		// Mutator
		void set_chunk_size(const u64 chunk_size) { m_chunk_size = chunk_size; }
		void set_chunk_drift(const u64 chunk_drift) { m_chunk_drift = chunk_drift; }

		// Accessor		
		const u64 get_chunk_size() const { return m_chunk_size; }
		const u64 get_chunk_drift() const { return m_chunk_drift; }

		// Static
		static ptr create(const u64 file_size, const u64 file_offset = 0) { return ptr(new chunk_upload_queue_component(file_size, file_offset)); }
	};

	class chunk_download_queue_component final : public chunk_queue_component
	{
	private:
		u64 m_chunk_buffer = default_chunk::num_buffer;
		u64 m_chunk_usage = 0;
		u32 m_chunk_size = 0;

	public:
		using ptr = std::shared_ptr<chunk_download_queue_component>;

	private:
		chunk_download_queue_component(const u64 file_size, const u64 file_offset = 0) : chunk_queue_component(file_size, file_offset) { }

	public:
		virtual ~chunk_download_queue_component() override { }

		// Container
		virtual void push(const value_type value) override { return push(value, true); }
		void push(const value_type value, const bool init);

		virtual value_type pop() override;
		template <typename T> value_type pop_if(T value);
		value_type pop_push();

		// Utility
		virtual bool done() const override { return chunk_queue_component::done(); }
		bool full() const { return m_chunk_usage >= m_chunk_buffer; }

		// Mutator		
		void set_chunk_buffer(const u64 chunk_buffer) { m_chunk_buffer = chunk_buffer; }

		// Accessor		
		const u64 get_chunk_size() const { return m_chunk_size; }
		const u64 get_chunk_buffer() const { return m_chunk_buffer; }
		const u64 get_chunk_usage() const { return m_chunk_usage; }		

		// Static
		static ptr create(const u64 file_size, const u64 file_offset = 0) { return ptr(new chunk_download_queue_component(file_size, file_offset)); }
	};

	template <typename T>
	typename chunk_download_queue_component::value_type chunk_download_queue_component::pop_if(T t)
	{
		auto_lock();
		auto it = std::find_if(m_queue.begin(), m_queue.end(), t);
		if (it != m_queue.end())
		{
			auto value = *it;
			m_chunk_usage -= value->get_size();
			m_queue.erase(it);
			return value;
		}

		return nullptr;
	}
}

#endif
