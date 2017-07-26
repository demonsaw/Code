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

#include "delegate/row_delegate.h"
#include "entity/entity.h"

namespace eja
{
	// Utility
	QSize row_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QFontMetrics metrics(option.font);		
		const auto str = QString().sprintf("%02d", index.row() + 1);
		QRect rect = metrics.boundingRect(str);
		rect.setWidth(rect.width() + offset::padding);

		return rect.size();
	}

	void row_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			// Name
			const auto str = QString().sprintf("%02d", index.row() + 1);
			QRect rect = QRect(option.rect.left(), option.rect.top() + offset::top, option.rect.width() - offset::padding, option.rect.height() - offset::top);
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignTop | Qt::AlignRight, str);

			painter->restore();
		}
	}
}
