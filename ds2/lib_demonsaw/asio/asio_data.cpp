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

#include <cassert>

#include "asio_data.h"

namespace eja
{
	// Constructor
	asio_data::asio_data(const asio_data& data)
	{
		if (!data.is_empty())
		{
			m_data = new byte[data.get_size()];
			memcpy(m_data, data.get_data(), data.get_size());
			m_size = data.get_size();
		}
	}

	asio_data::asio_data(const void* data, const size_t size)
	{
		assert(data && size);

		m_data = new byte[size];
		memcpy(m_data, data, size);
		m_size = size;
	}

	asio_data::~asio_data()
	{
		delete[] reinterpret_cast<byte*>(m_data);
	}

	// Operator
	asio_data& asio_data::operator=(const asio_data& data)
	{
		if (this != &data)
			set_data(data.get_data(), data.get_size());

		return *this;
	}

	asio_data& asio_data::operator=(const std::string& data)
	{
		set_data(data.c_str(), data.size() + 1);

		return *this;
	}

	asio_data& asio_data::operator=(const char* data)
	{
		set_data(data, strlen(data) + 1);

		return *this;
	}

	// Interface
	void asio_data::clear()
	{
		if (m_data)
		{
			delete[] reinterpret_cast<byte*>(m_data);
			m_data = nullptr;
			m_size = 0;
		}
	}

	// Mutator
	void asio_data::set_data(const void* data, const size_t size)
	{
		assert(data && size);

		clear();

		m_data = new byte[size];
		memcpy(m_data, data, size);
		m_size = size;
	}
}
