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

#include "uri.h"

namespace eja
{
	// Static
	const char HEX2DEC[256] =
	{
		/*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
		/* 0 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* 1 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* 2 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* 3 */  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,

		/* 4 */ -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* 5 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* 6 */ -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* 7 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

		/* 8 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* 9 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* A */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* B */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

		/* C */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* D */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* E */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		/* F */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};

	// Only alphanumeric is safe.
	const char SAFE[256] =
	{
		/*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
		/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 3 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,

		/* 4 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		/* 5 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
		/* 6 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		/* 7 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,

		/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 9 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* A */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* B */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		/* C */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* D */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* E */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* F */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	// Interface
	void uri::clear()
	{
		m_scheme = http::scheme;
		m_port = http::port;
		m_address.clear();
		m_resource = http::root;
	}

	bool uri::parse(const std::string& path)
	{
		clear();

		// Scheme
		auto address_pos = path.find(http::scheme_delimiter);
		if (address_pos != std::string::npos)
		{
			m_scheme = path.substr(0, address_pos);
			address_pos += strlen(http::scheme_delimiter);
			if (address_pos >= path.size())
				return false;
		}
		else
		{
			address_pos = 0;
		}

		//  Resource
		auto resource_pos = path.find(http::delimiter, address_pos);
		if (resource_pos != std::string::npos)
		{
			m_resource = path.substr(resource_pos, path.size() - resource_pos);
		}
		else
		{
			resource_pos = path.size();
			m_resource = http::root;
		}

		// Port
		auto port_pos = path.find(":", address_pos);
		if (port_pos != std::string::npos)
		{
			if (port_pos < resource_pos)
			{
				const auto str = path.substr(port_pos + strlen(":"), resource_pos - port_pos);
				m_port = boost::lexical_cast<u16>(str);
			}
		}
		else
		{
			port_pos = resource_pos;
		}

		// Address
		m_address = path.substr(address_pos, port_pos - address_pos);
		return !m_address.empty();
	}

	// Static
	std::string uri::encode(const byte* input, const size_t input_size)
	{
		const char DEC2HEX[16 + 1] = "0123456789ABCDEF";

		byte* const pStart = new byte[input_size * 3];
		byte* pEnd = pStart;
		const byte* const SRC_END = input + input_size;

		for (; input < SRC_END; ++input)
		{
			if (SAFE[*input])
				*pEnd++ = *input;
			else
			{
				// escape this char
				*pEnd++ = '%';
				*pEnd++ = DEC2HEX[*input >> 4];
				*pEnd++ = DEC2HEX[*input & 0x0F];
			}
		}

		std::string sResult((char *) pStart, (char *) pEnd);
		delete[] pStart;
		return sResult;
	}

	std::string uri::decode(const byte* input, const size_t input_size)
	{
		// Note from RFC1630: "Sequences which start with a percent
		// sign but are not followed by two hexadecimal characters
		// (0-9, A-F) are reserved for future extension"
		const byte * const SRC_END = input + input_size;

		// last decodable '%'
		const byte * const SRC_LAST_DEC = SRC_END - 2;

		char * const pStart = new char[input_size];
		char * pEnd = pStart;

		while (input < SRC_LAST_DEC)
		{
			if (*input == '%')
			{
				char dec1, dec2;
				if (-1 != (dec1 = HEX2DEC[*(input + 1)])
					&& -1 != (dec2 = HEX2DEC[*(input + 2)]))
				{
					*pEnd++ = (dec1 << 4) + dec2;
					input += 3;
					continue;
				}
			}

			*pEnd++ = *input++;
		}

		// the last 2- chars
		while (input < SRC_END)
			*pEnd++ = *input++;

		std::string sResult(pStart, pEnd);
		delete[] pStart;
		return sResult;
	}
}
