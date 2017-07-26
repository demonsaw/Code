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

#include <stdexcept>

#include "security/filter/hex.h"
#include "system/bstring.h"

namespace eja
{
	// Constructor
	bstring::bstring(bstring&& bstr)
	{
		m_data = bstr.m_data;
		m_size = bstr.m_size;
		m_capacity = bstr.m_capacity;

		bstr.m_data = nullptr;
		bstr.m_size = 0;
		bstr.m_capacity = 0;
	}

	bstring::bstring(const bstring& bstr)
	{
		if (bstr.m_capacity != 0)
		{
			m_data = new char[bstr.m_capacity];

			if (bstr.m_size != 0)
				std::memcpy(m_data, bstr.m_data, bstr.m_size);
		}
		else
			m_data = nullptr;

		m_size = bstr.m_size;
		m_capacity = bstr.m_capacity;
	}

	bstring::bstring(const size_t capacity)
	{
		m_data = new char[capacity];
		m_capacity = capacity;
		m_size = 0;
	}

	bstring::bstring(const void* data, const size_t size)
	{
		if (size != 0)
		{
			m_data = new char[size];
			std::memcpy(m_data, data, size);
		}
		else
			m_data = nullptr;

		m_size = m_capacity = size;
	}

	bstring::bstring(const void* data, const size_t size, const size_t capacity)
	{
		if (size > capacity)
			throw std::invalid_argument("size greater than capacity");

		if (capacity != 0)
		{
			m_data = new char[capacity];

			if (size != 0)
				std::memcpy(m_data, data, size);
		}
		else
			m_data = nullptr;

		m_size = size;
		m_capacity = capacity;
	}

	// Operator
	bstring& bstring::operator=(bstring&& bstr)
	{
		if (this != &bstr)
		{
			delete [] m_data;

			m_data = bstr.m_data;
			m_size = bstr.m_size;
			m_capacity = bstr.m_capacity;

			bstr.m_data = nullptr;
			bstr.m_size = 0;
			bstr.m_capacity = 0;
		}

		return *this;
	}

	bstring& bstring::operator=(const bstring& bstr)
	{
		if (this != &bstr)
			assign(bstr.data(), bstr.size());

		return *this;
	}

	bstring& bstring::operator+=(const bstring& bstr)
	{
		if (this != &bstr)
			append(bstr.data(), bstr.size());

		return *this;
	}

	// Utility
	std::string bstring::hex() const
	{
		return hex::encode(m_data, m_size);
	}

	std::string bstring::str() const
	{
		return std::string(m_data, m_size);
	}

	void bstring::append(const void* data, const size_t size)
	{
		if (size != 0)
		{
			size_t new_size = m_size + size;

			if (m_capacity < new_size)
				reserve(new_size);

			std::memcpy(m_data + m_size, data, size);
			m_size = new_size;
		}
	}
	
	void bstring::assign(const void* data, const size_t size)
	{
		if (size > m_capacity)
		{
			delete [] m_data;
			m_data = new char[size];
			m_capacity = size;
		}

		if (size != 0)
			std::memcpy(m_data, data, size);

		m_size = size;
	}

	void bstring::assign(const void* data, const size_t size, const size_t capacity)
	{
		delete [] m_data;
		m_data = static_cast<char*>(const_cast<void*>(data));
		m_size = size;
		m_capacity = capacity;
	}

	void bstring::clear()
	{
		delete [] m_data;
		m_data = nullptr;
		m_size = 0;
		m_capacity = 0;
	}

	bool bstring::reserve(const size_t capacity)
	{
		// Ignore capacity decrease requests
		//
		if (m_capacity >= capacity)
			return false;

		char* data = new char[capacity];

		if (m_size != 0)
			std::memcpy(data, m_data, m_size);

		delete [] m_data;
		m_data = data;
		m_capacity = capacity;

		return true;
	}

	bool bstring::resize(const size_t size)
	{
		bool value = false;

		if (m_capacity < size)
			value = reserve(size);

		m_size = size;

		return value;
	}

	void bstring::swap(bstring& bstr)
	{
		char* data = bstr.m_data;
		size_t size = bstr.m_size;
		size_t capacity = bstr.m_capacity;

		bstr.m_data = m_data;
		bstr.m_size = m_size;
		bstr.m_capacity = m_capacity;

		m_data = data;
		m_size = size;
		m_capacity = capacity;
	}
}
