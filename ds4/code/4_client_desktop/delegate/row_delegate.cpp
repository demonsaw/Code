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
#include "resource/resource.h"

namespace eja
{
	// Utility
	void row_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			const auto variant = index.data(Qt::UserRole);
			QRect rect(option.rect.left(), option.rect.top(), option.rect.width() - resource::get_icon_padding(), option.rect.height());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight, QString::number(variant.toUInt() + 1));

			painter->restore();
		}
	}

	QSize row_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(option.rect.width(), resource::get_row_height());
	}
}