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

#include <QPainter>
#include <QTextDocument>

#include "rich_text_delegate.h"

namespace eja
{
	void rich_text_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		if (option.state & QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());

		painter->save();

		QTextDocument document;
		document.setTextWidth(option.rect.width());
		QVariant value = index.data(Qt::DisplayRole);

		if (value.isValid() && !value.isNull())
		{
			document.setHtml(value.toString());
			painter->translate(option.rect.topLeft());
			document.drawContents(painter);
		}

		painter->restore();
	}
}
