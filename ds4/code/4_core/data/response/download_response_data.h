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

#ifndef _EJA_DOWNLOAD_RESPONSE_DATA_H_
#define _EJA_DOWNLOAD_RESPONSE_DATA_H_

#include <deque>
#include <memory>

#include "data/chunk_data.h"
#include "data/data.h"
#include "system/type.h"
#include "thread/thread_safe.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(download_response_data, 1);

	make_thread_safe_type(download_response_data_list, std::deque<std::shared_ptr<download_response_data>>);

	class download_response_data final : public data
	{
		make_factory(download_response_data);

	private:
		std::shared_ptr<chunk_data> m_chunk;

	public:
		download_response_data() : data(data_type::download_response) { }
		download_response_data(const std::shared_ptr<chunk_data>& chunk) : data(data_type::download_response), m_chunk(chunk) { }

		// Interface
		virtual void clear() override { m_chunk->clear(); }

		// Utility
		virtual bool valid() const override { return data::valid() && has_chunk() && m_chunk->valid(); }

		// Has
		bool has_chunk() const { return m_chunk != nullptr; }
		bool has_data() const { return m_chunk->has_data(); }
		bool has_sequence() const { return m_chunk->has_sequence(); }

		// Set		
		void set_chunk(const std::shared_ptr<chunk_data>& chunk) { m_chunk = chunk; }
		void set_data(std::string&& data) { m_chunk->set_data(std::move(data)); }
		void set_data(const std::string& data) { m_chunk->set_data(data); }
		void set_sequence(const u32 sequence) { m_chunk->set_sequence(sequence); }

		// Get
		std::shared_ptr<chunk_data> get_chunk() const { return m_chunk; }
		const std::string& get_data() const { return m_chunk->get_data(); }
		u32 get_sequence() const { return m_chunk->get_sequence(); }

		// Static
		static ptr create(const std::shared_ptr<chunk_data>& chunk) { return std::make_shared<download_response_data>(chunk); }

		MSGPACK_DEFINE(MSGPACK_BASE(data), m_chunk);
	};
}

#endif
