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

#include <QLocale>
#include <QPainter>

#include "component/io/file_component.h"
#include "component/router/router_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"
#include "component/status/status_component.h"

#include "delegate/io/file_progress_delegate.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
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
			painter->setRenderHint(QPainter::TextAntialiasing, true);			

			// Percent
			QFontMetrics metrics(option.font);
			const auto transfer = entity->get<transfer_component>();
			const auto percent = QString("%1%").arg(static_cast<size_t>(transfer->get_percent()));
			const auto percent_width = metrics.width("100%");

			QRect rect(option.rect.left() + resource::get_icon_padding(), option.rect.top(), percent_width, option.rect.height());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignRight | Qt::AlignVCenter, percent);			

			// Seeds
			const auto seeds = QString::number(transfer->get_seeds());
			const auto seeds_left = metrics.width(seeds) + (resource::get_icon_padding() * 2);
			rect = QRect(option.rect.right() - seeds_left, option.rect.top(), seeds_left, option.rect.height());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, seeds);

			// Color
			const auto text_pen = painter->pen();
			const auto status = entity->get<status_component>();
			const auto color = QColor(status->get_color());

			QPen pen(color);
			const auto bar_border = 1;
			pen.setWidth(bar_border);
			painter->setPen(pen);

			// Outline
			const auto outline_left = percent_width + (resource::get_icon_padding() * 2);
			const auto bar_height = option.rect.height() - (resource::get_icon_padding() * 2) - 1;
			const auto bar_width = option.rect.width() - outline_left - seeds_left - 1;
			rect = QRect(option.rect.left() + outline_left, option.rect.top() + resource::get_icon_padding(), bar_width, bar_height);
			painter->drawRect(rect);

			if (!transfer->empty())
			{
				const auto left = option.rect.left() + outline_left + bar_border;
				const auto top = option.rect.top() + resource::get_icon_padding() + bar_border;

				const auto ratio = transfer->get_ratio();
				const auto width = (bar_width * ratio) - bar_border;
				const auto height = bar_height - bar_border;

				rect = QRect(left, top, width, height);
				painter->fillRect(rect, color);
			}

			// Chunks
			if (transfer->is_download() && transfer->has_chunks())
			{
				const auto chunks = QLocale::system().toString(static_cast<uint>(transfer->get_chunks()));
				const auto chunk_left = metrics.width(chunks) + (resource::get_icon_padding() * 2);
				rect = QRect(option.rect.right() - seeds_left - chunk_left - (resource::get_icon_padding() / 2), option.rect.top(), chunk_left, option.rect.height());

				painter->setPen(text_pen);
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignRight | Qt::AlignVCenter, chunks);

			}
			painter->restore();
		}
	}

	QSize file_progress_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(128 * resource::get_ratio(), resource::get_row_height());
	}
}