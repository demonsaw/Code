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

#ifndef _EJA_DATA_H_
#define _EJA_DATA_H_

#include <deque>
#include <memory>
#include <string>

#include "data/data_type.h"
#include "object/mode.h"
#include "object/object.h"

namespace eja
{
	class data;

	using data_list = std::deque<std::shared_ptr<data>>;

	class data : public object, public mode<u8>
	{
	protected:
		data_type m_type = data_type::none;

	public:
		using ptr = std::shared_ptr<data>;

	protected:
		data() { }
		explicit data(const data& dat) : object(dat), mode<u8>(dat), m_type(dat.m_type) { }
		explicit data(const data_type& type) : m_type(type) { }
		explicit data(const data_type& type, const u8 bits) : mode<u8>(bits), m_type(type) { }
		explicit data(const u8 bits) : mode<u8>(bits) { }

		// Operator
		data& operator=(const data& dat);

	public:
		// Has
		bool has_type() const { return m_type != data_type::none; }

		// Set
		void set_type(const data_type type) { m_type = type; }

		// Get
		data_type get_type() const { return m_type; }

		MSGPACK_DEFINE(m_mode);
	};

	// Operator
	inline data& data::operator=(const data& dat)
	{
		if (this != &dat)
		{
			object::operator=(dat);
			mode<u8>::operator=(dat);

			m_type = dat.m_type;
		}

		return *this;
	}

	// Factory
	template <data_type T>
	class data_impl final : public data
	{
		make_factory(data_impl);

	public:
		data_impl() : data(T) { }

		MSGPACK_DEFINE(MSGPACK_BASE(data));
	};
}

#endif
