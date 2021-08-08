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

#include <QPlainTextEdit>
#include <QTextBlock>
#include <QTextEdit>

#include "delegate/delegate_util.h"

namespace eja
{
	size_t delegate_util::get_rows(const QFont& font, const QString& str, const size_t width)
	{
		assert(width);

		// Control
		QTextEdit edit;
		edit.setFont(font);
		edit.setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
		edit.setLineWrapMode(QTextEdit::FixedPixelWidth);
		edit.setLineWrapColumnOrWidth(width);
		edit.setFixedWidth(width);
		edit.setReadOnly(true);
		edit.setAcceptRichText(false);
		edit.setPlainText(str);

		QFontMetrics metrics(font);
		edit.setTabStopWidth(metrics.width(' '));
		
		size_t rows = 0;
		const auto doc = edit.document();

		for (size_t i = 0; i < doc->blockCount(); ++i)
		{
			const auto block = doc->findBlockByLineNumber(i);			
			const auto layout = block.layout();
			rows += std::max(1, layout->lineCount());
		}
		
		if (!rows)
			rows = doc->lineCount();

		return rows;
	}
}