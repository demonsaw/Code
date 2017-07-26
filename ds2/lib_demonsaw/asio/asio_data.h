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

#ifndef _EJA_ASIO_DATA_
#define _EJA_ASIO_DATA_

#include <memory>
#include <string>

#include "system/type.h"

namespace eja
{
	class asio_data final
	{
	private:
		void* m_data = nullptr;
		size_t m_size = 0;

	public:
		using ptr = std::shared_ptr<asio_data>;

	public:
		asio_data() { }
		asio_data(const asio_data& data);		
		asio_data(const void* data, const size_t data_size); 
		asio_data(const std::string& data) : asio_data(data.c_str(), data.size() + 1) { }
		asio_data(const char* data) : asio_data(data, strlen(data) + 1) { }
		~asio_data();

		// Operator
		asio_data& operator=(const asio_data& data);
		asio_data& operator=(const std::string& data);
		asio_data& operator=(const char* data);

		operator const void*() const { return m_data; }
		operator void*() const { return m_data; }
		
		operator const char*() const { return static_cast<const char*>(m_data); }
		operator char*() const { return static_cast<char*>(m_data); }

		friend std::ostream& operator<<(std::ostream& os, const asio_data& data) { return os << static_cast<const char*>(data); }

		// Interface
		void clear();

		// Utility
		bool is_empty() const { return !m_size; }
		std::string str() const { return std::string(static_cast<const char*>(m_data), m_size); }

		// Mutator
		void set_data(const void* data, const size_t size); 
		void set_data(const std::string& data) { set_data(data.c_str(), data.size() + 1); }
		void set_data(const char* data) { set_data(data, strlen(data) + 1); }		

		// Accessor
		void* get_data() const { return m_data; }
		size_t get_size() const { return m_size; }

		// Static
		static ptr create() { return std::make_shared<asio_data>(); }
		static ptr create(const asio_data& data) { return std::make_shared<asio_data>(); }
		static ptr create(const void* data, const size_t data_size) { return std::make_shared<asio_data>(data, data_size); }
		static ptr create(const std::string& data) { return std::make_shared<asio_data>(data); }
		static ptr create(const char* data) { return std::make_shared<asio_data>(data); }
		
	};
}

#endif
