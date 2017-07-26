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

#include <memory>

#include <QAbstractItemView>
#include <QPainter>
#include <QStyledItemDelegate>

#include "transfer_progress_delegate.h"
#include "component/transfer/download_thread_component.h"
#include "component/transfer/upload_thread_component.h"
#include "component/transfer/transfer_component.h"
#include "entity/entity.h"
#include "window/window.h"

namespace eja
{
	// Utility
	void transfer_progress_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			// Component
			auto status = eja::status::none;
			if (entity->has<download_thread_component>())
			{
				const auto thread = entity->get<download_thread_component>();
				if (thread)
					status = thread->get_status();
			}
			else if (entity->has<upload_thread_component>())
			{
				const auto thread = entity->get<upload_thread_component>();
				if (thread)
					status = thread->get_status();
			}

			const auto transfer = entity->get<transfer_component>();
			if (!transfer)
				return;

			// Status
			QBrush brush;

			switch (status)
			{
				case eja::status::success:
				case eja::status::none:
				{
					brush = QColor(0, 208, 135);
					break;
				}
				case eja::status::pending:
				{
					brush = QColor(208, 208, 208);
					break;
				}
				case eja::status::info:
				{
					brush = QColor(26, 158, 205);
					break;
				}
				case eja::status::warning:
				{
					brush = QColor(255, 195, 0);
					break;
				}
				case eja::status::error:
				{
					brush = QColor(229, 40, 29);
					break;
				}
				case eja::status::cancelled:
				{
					brush = QColor(0, 0, 0);
					break;
				}
				case eja::status::unknown:
				{
					brush = QColor(0, 0, 0);
					break;
				}
				default:
				{
					assert(false);
				}
			}

			const auto width = 100;// ((option.rect.right() - option.rect.left() - (offset::left * 2) - offset::bar + 2) * transfer->get_ratio());

			// Outline
			QRect rect = option.rect;
			rect.setTop(option.rect.top() + offset::top);
			rect.setHeight(14);
			rect.setLeft(option.rect.left() + offset::outline);
			rect.setWidth(width);

			painter->save();
			painter->setBrush(Qt::NoBrush);
			painter->setPen(QColor(176, 176, 176));
			painter->drawRect(rect);

			// Bar
			if (!transfer->empty())
			{
				rect.setTop(option.rect.top() + offset::top + 1);
				rect.setLeft(option.rect.left() + offset::bar);
				rect.setWidth((width * transfer->get_ratio()) - 1);
				painter->fillRect(rect, brush);
			}			

			// text
			QRect text_rect = option.rect;
			text_rect.setRight(option.rect.left() + offset::text);
			const auto percent = static_cast<size_t>(transfer->get_percent());

			painter->setPen(Qt::black);
			painter->drawText(text_rect, Qt::TextSingleLine | Qt::AlignRight | Qt::AlignVCenter, QString::number(percent) + "%");
			painter->restore();
		}
	}
}