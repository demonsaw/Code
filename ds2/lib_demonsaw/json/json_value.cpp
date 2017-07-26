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

#include "json_value.h"

namespace eja
{
	// Operator		
	std::ostream& operator<<(std::ostream& os, const json_value& value)
	{
		Json::StyledWriter writer;

		return os << writer.write(value.get_value());
	}

	// Interface
	std::string json_value::str() const
	{
		const auto& value = get_value();
		if (!value.empty())
		{
			Json::FastWriter writer;
			writer.omitEndingLineFeed();
			return writer.write(value);
		}
		
		return "";
	}

	// Utility
	void json_value::parse(const std::string& input)
	{
		Json::Reader reader;
		if (!reader.parse(input, get_value()))
			throw std::runtime_error(reader.getFormattedErrorMessages());
	}

	void json_value::parse(const char* input)
	{
		Json::Reader reader;
		if (!reader.parse(input, get_value()))
			throw std::runtime_error(reader.getFormattedErrorMessages());
	}
}
