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

#include <boost/format.hpp>

#include "object/time/timeout.h"

namespace eja
{
	// Operator
	timeout& timeout::operator=(const timeout& obj)
	{
		if (this != &obj)
		{
			object::operator=(obj);

			m_start = obj.m_start;
		}

		return *this;
	}

	// Interface
	void timeout::stop()
	{
		std::chrono::seconds zero { 0 };
		std::chrono::system_clock::time_point epoch { zero };
		m_start = epoch;
	}

	std::string timeout::str() const
	{
		if (valid())
		{
			const auto ms = get_milliseconds();
			const auto seconds = (ms / 1000) % 60;
			const auto minutes = (ms / (1000 * 60)) % 60;
			const auto hours = (ms / (1000 * 60 * 60)) % 24;

			return boost::str(boost::format("%02d:%02d:%02d") % hours % minutes % seconds);
		}

		return "0";
	}
}
