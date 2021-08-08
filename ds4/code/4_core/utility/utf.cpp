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

#if _WIN32
#include <codecvt>
#include <locale>
#endif

#include "utility/utf.h"

namespace eja
{
	namespace utf
	{
#if _WIN32
		std::string to_utf8(const std::wstring& input)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
			return convert.to_bytes(input);
		}

		std::wstring to_utf16(const std::string& input)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
			return convert.from_bytes(input);
		}
#endif
		/*
		https://stackoverflow.com/questions/28769452/qt-4-7-unicode-conversion-of-string

		┌────────┬────────┬─────────┬─────────┬─────────┬─────────┬─────────┬────────┐
		│Bits of │First   │Last     │Bytes in │Byte 1   │Byte 2   │Byte 3   │Byte 4  │
		│code    │code    │code     │sequence │         │         │         │        │
		│point   │point   │point    │         │         │         │         │        │
		├────────┼────────┼─────────┼─────────┼─────────┼─────────┼─────────┼────────┤
		│7       │U+0000  │U+007F   │1        │0xxxxxxx │         │         │        │
		├────────┼────────┼─────────┼─────────┼─────────┼─────────┼─────────┼────────┤
		│11      │U+0080  │U+07FF   │2        │110xxxxx │10xxxxxx │         │        │
		├────────┼────────┼─────────┼─────────┼─────────┼─────────┼─────────┼────────┤
		│16      │U+0800  │U+FFFF   │3        │1110xxxx │10xxxxxx │10xxxxxx │        │
		├────────┼────────┼─────────┼─────────┼─────────┼─────────┼─────────┼────────┤
		│21      │U+10000 │U+1FFFFF │4        │11110xxx │10xxxxxx │10xxxxxx │10xxxxxx│
		└────────┴────────┴─────────┴─────────┴─────────┴─────────┴─────────┴────────┘
		*/
		std::string strip(const std::string& input, const char value /*= '\0'*/)
		{
			size_t skip = 0;
			std::string output;

			for (auto i = 0; i < input.size(); ++i)
			{
				if (!skip)
				{
					const auto& ch = input[i];
					if (!(ch & 0x80))
					{
						output += ch;
					}
					else
					{
						if (value != '\0')
							output += value;

						// 2 byte
						if (ch & 0x40)
						{
							++skip;

							// 3 byte
							if (ch & 0x20)
							{
								++skip;

								// 4 byte
								if (ch & 0x10)
									++skip;
							}
						}
					}
				}
				else
				{
					--skip;
				}
			}

			return output;
		}
	}	
}
