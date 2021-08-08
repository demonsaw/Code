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

#ifndef _EJA_MODE_H_
#define _EJA_MODE_H_

namespace eja
{
	template <typename T = size_t>
	class mode
	{
	public:
		enum bits : T { none };

	protected:
		T m_mode = bits::none;

	protected:
		mode() { }
		explicit mode(const T mode) : m_mode(mode) { }

		// Set
		virtual void set_mode(const T mode) { m_mode = mode; }
		virtual void set_mode(const T mode, const bool value) { m_mode = (value ? (m_mode | mode) : (m_mode & ~mode)); }

	public:
		// Has
		bool has_mode() const { return m_mode != bits::none; }

		// Is
		bool is_mode(const T mode) const { return (m_mode & mode) == mode; }
		bool is_none() const { return m_mode == bits::none; }

		// Set
		void set_mode() { set_none(); }
		void set_none() { m_mode = bits::none; }

		// Get
		virtual T get_mode() const { return m_mode; }
	};
}

#endif
