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

#include <string>

#include "data/data_type.h"
#include "object/object.h"

namespace eja
{
	class data;
	using data_list = std::list<std::shared_ptr<data>>;

	class data : public object
	{
	protected:
		data_type m_type = data_type::none;

	protected:
		data() { }
		data(const data& dat) : object(dat), m_type(dat.m_type) { }
		data(const data_type& type) : m_type(type) { }

		// Operator
		data& operator=(const data& dat);

	public:
		// Has
		bool has_type() const { return m_type != data_type::none; }

		// Set
		void set_type(const data_type& type) { m_type = type; }

		// Get
		data_type get_type() const { return m_type; }
	};

	// Operator
	inline data& data::operator=(const data& dat)
	{
		if (this != &dat)
		{
			object::operator=(dat);

			m_type = dat.m_type;
		}

		return *this;
	}
}

#endif
