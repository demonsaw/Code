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

#include "utility/locale.h"

namespace eja
{
	namespace locale
	{
		/*QTextCodec::ConverterState state;
		QTextCodec* codec = QTextCodec::codecForName("UTF-8");
		const QString text = codec->toUnicode(array.constData(), array.size(), &state);
		return (state.invalidChars == 0) ? text : "!!!!!!";*/

		QString sanitize(const QString& input)
		{
			QString output;
			output.reserve(input.size());
			bool modified = false;

			for (int i = 0; i < input.size(); ++i)
			{
				const auto ch = input.at(i);
				if (ch.isSpace() || !ch.isPrint() || ch.isMark() ||
					ch.category() == QChar::Mark_NonSpacing ||
					ch.category() == QChar::Mark_SpacingCombining ||
					ch.category() == QChar::Mark_Enclosing ||
					//ch.category() == QChar::Number_Other ||
					ch.category() == QChar::Separator_Space ||
					ch.category() == QChar::Separator_Line ||
					ch.category() == QChar::Separator_Paragraph ||
					ch.category() == QChar::Other_Control ||
					ch.category() == QChar::Other_Format ||
					ch.category() == QChar::Other_Surrogate ||
					ch.category() == QChar::Other_PrivateUse ||
					ch.category() == QChar::Other_NotAssigned ||
					ch.category() == QChar::Symbol_Other)
				{
					if (ch != 0x20)
					{
						modified = true;
						continue;
					}					
				}

				output.append(ch);
			}

			return modified ? output.simplified() : output;
		}
	}
}