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

#ifndef _EJA_CHUNK_DATA_H_
#define _EJA_CHUNK_DATA_H_

#include <deque>
#include <memory>
#include <string>

#include "data/data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(chunk_data, 1);

	using chunk_data_list = std::deque<std::shared_ptr<chunk_data>>;

	class chunk_data : public data
	{
		make_factory(chunk_data);

	private:
		std::string m_data;
		u32 m_sequence = static_cast<u32>(~0);

	public:
		chunk_data() { }

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return data::valid() && has_data() & has_sequence(); }
		void reset();

		// Has
		bool has_data() const { return !m_data.empty(); }
		bool has_sequence() const { return m_sequence < static_cast<u32>(~0); }
		bool has_size() const { return has_data(); }

		// Set
		void set_data(std::string&& data) { m_data = std::move(data); }
		void set_data(const std::string& data) { m_data = data; }
		void set_sequence(const u32 sequence) { m_sequence = sequence; }

		// Get
		std::string& get_data() { return m_data; }
		const std::string& get_data() const { return m_data; }

		u32 get_sequence() const { return m_sequence; }
		size_t get_size() const { return m_data.size(); }

		MSGPACK_DEFINE(MSGPACK_BASE(data), m_data, m_sequence);
	};
}

#endif
