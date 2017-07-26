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

#include "group_row_delegate.h"
#include "entity/entity.h"
#include "window/window.h"

namespace eja
{
	// Utility
	QSize group_row_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		const auto str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize((option.decorationSize.width() / 2) + rect.width() + offset::text, 0);
	}

	void group_row_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			const auto variant = index.data(Qt::EditRole);
			const auto str = variant.toString();

			// Name
			QRect rect = QRect(option.rect.left() + offset::text, option.rect.top() + offset::top, option.rect.width() - offset::text, option.rect.height() - (offset::top * 2));
			painter->save();
			painter->drawText(rect, Qt::TextSingleLine, str);
			painter->restore();
		}
	}
}
