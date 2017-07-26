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

#ifndef _EJA_ID_DATA_H_
#define _EJA_ID_DATA_H_

#include <string>

namespace eja
{
	class id_data
	{
	protected:
		std::string m_id;

	protected:
		id_data() { }
		id_data(const id_data& dat) : m_id(dat.m_id) { }

		// Operator
		id_data& operator=(const id_data& dat);

	public:
		// Has
		bool has_id() const { return !m_id.empty(); }

		// Set
		void set_id(const std::string& id) { m_id = id; }

		// Get
		const std::string& get_id() const { return m_id; }
	};

	// Operator
	inline id_data& id_data::operator=(const id_data& dat)
	{
		if (this != &dat)
			m_id = dat.m_id;

		return *this;
	}
}

#endif
