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

#include "component/io/file_component.h"
#include "component/io/transfer_component.h"
#include "component/status/status_component.h"
#include "delegate/io/file_progress_delegate.h"
#include "entity/entity.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	void file_progress_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			painter->save();
			//painter->setRenderHint(QPainter::Antialiasing, true);

			// Text
			const auto transfer = entity->get<transfer_component>();
			const auto percent = QString("%1%").arg(static_cast<size_t>(transfer->get_percent()));
			QRect rect(option.rect.left(), option.rect.top(), offset::left_text, option.rect.height());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignRight | Qt::AlignVCenter, percent);

			// Color
			const auto status = entity->get<status_component>();
			const auto color = QColor(status->get_color());
			auto outline = QColor(color.red(), color.green(), color.blue(), color.alpha() >> 1);
			painter->setPen(outline);
			
			auto width = option.rect.width() - (offset::left * 2) - offset::left_text - 1;
			rect = QRect(option.rect.left() + offset::left_outline, option.rect.top() + offset::top, width, offset::height_bar);
			painter->drawRect(rect);

			// Bar
			const auto file = entity->get<file_component>();

			if (!transfer->empty() || !file->has_size())
			{
				const auto bar = (width * clamp(0.0, transfer->get_ratio(), 1.0)) - 1;
				QRect rect(option.rect.left() + offset::left_bar, option.rect.top() + offset::top + 1, bar, offset::height_bar - 1);
				painter->fillRect(rect, color);
			}
			else
			{
				QRect rect(option.rect.left() + offset::left_bar, option.rect.top() + offset::top + 1, width - 1, offset::height_bar - 1);
				painter->fillRect(rect, color::white);
			}

			painter->restore();
		}
	}
}