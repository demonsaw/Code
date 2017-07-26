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

#ifndef _EJA_BSTRING_H_
#define _EJA_BSTRING_H_

#include <memory>
#include <ostream>
#include <string>

#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class bstring final
	{
		make_factory(bstring);

	private:
		char* m_data;
		size_t m_size;
		size_t m_capacity;

	public:
		static const size_t npos = static_cast<size_t>(-1);

	public:
		bstring() : m_data(nullptr), m_size(0), m_capacity(0) { }
		bstring(bstring&& bstr);
		bstring(const bstring& bstr);
		explicit bstring(const size_t capacity);
		bstring(const void* data, const size_t size);
		bstring(const void* data, const size_t size, const size_t capacity);
		~bstring() { delete[] m_data; }

		// Operator
		bstring& operator=(bstring&& bstr);
		bstring& operator=(const bstring& bstr);
		bool operator==(const bstring& bstr) { return size() == bstr.size() && !std::memcmp(data(), bstr.data(), size()); }
		bool operator!=(const bstring& bstr) { return !(*this == bstr); }
		bstring& operator+=(const bstring& bstr);

		friend std::ostream& operator<<(std::ostream& os, const bstring& bstr) { return os << bstr.hex(); }

		// Utility
		void clear();
		bool empty() const { return !m_size; }
		std::string hex() const;
		std::string str() const;

		bool reserve(const size_t capacity);
		bool resize(const size_t size);
		
		void assign(const void* data, const size_t size);
		void assign(const void* data, const size_t size, const size_t capacity);

		// Required for msgpack buffer
		void write(const void* data, const size_t size) { append(data, size); }
		void append(const void* data, const size_t size);
		void swap(bstring& bstr);

		// Get
		const char* data() const { return m_data; }
		char* data() { return m_data; }

		size_t capacity() const { return m_capacity; }		
		size_t size() const { return m_size; }

		// Static
		static ptr create(bstring&& bstr) { return ptr(new bstring(bstr)); }
		static ptr create(const size_t capacity) { return ptr(new bstring(capacity)); }
		static ptr create(const void* data, const size_t size) { return ptr(new bstring(data, size)); }
		static ptr create(const void* data, const size_t size, const size_t capacity) { return ptr(new bstring(data, size, capacity)); }
	};
}

namespace msgpack
{
	MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
	{
		namespace adaptor
		{
			// Read
			template<>
			struct convert<eja::bstring>
			{
				msgpack::object const& operator()(msgpack::object const& o, eja::bstring& v) const
				{
					if (o.type != msgpack::type::BIN)
						msgpack::type_error();

					v.assign(o.via.bin.ptr, o.via.bin.size);

					return o;
				}
			};

			// Write
			template<>
			struct pack<eja::bstring>
			{
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, eja::bstring const& v) const
				{
					o.pack_bin(v.size());
					o.pack_bin_body(v.data(), v.size());

					return o;
				}
			};
		}
	}
}

#endif
