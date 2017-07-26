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

#include "chunk_component.h"

namespace eja
{
	// Upload
	queue_component<chunk>::value_type chunk_upload_queue_component::push()
	{
		auto_lock();

		// Pop
		if (m_file_offset >= m_file_size)
			return !chunk_queue_component::empty() ? pop_push() : nullptr;
		
		// Pop, Push
		if (m_chunk_drift && (size() >= 2))
		{
			const auto pair = std::minmax(peek_front()->get_offset(), peek_back()->get_offset());
			const auto delta = pair.second - pair.first;
			if (delta >= m_chunk_drift)
				return pop_push();
		}
		
		// Create, Push
		const u64 size = std::min(m_chunk_size, m_file_size - m_file_offset);
		const auto value = chunk::create(size, m_file_offset);
		m_file_offset += size;
		push(value);
		
		return value;
	}

	chunk_queue_component::value_type chunk_upload_queue_component::pop_push()
	{
		auto_lock();
		const auto value = pop();
		push(value);

		return value;
	}

	// Download
	void chunk_download_queue_component::push(const value_type value, const bool init)
	{
		auto_lock();
		chunk_queue_component::push(value);
		m_chunk_usage += value->get_size();

		if (init)
		{
			m_file_offset += value->get_size();
			++m_chunk_size;
		}		
	}

	chunk_queue_component::value_type chunk_download_queue_component::pop()
	{
		auto_lock();
		const auto value = chunk_queue_component::pop();
		m_chunk_usage -= value ? value->get_size() : 0;

		return value;
	}

	chunk_queue_component::value_type chunk_download_queue_component::pop_push()
	{
		auto_lock();
		const auto value = pop();
		push(value);

		return value;
	}
}
