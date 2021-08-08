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

#include "component/status/status_component.h"
#include "component/transfer/transfer_component.h"

#include "delegate/io/file_row_delegate.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Constructor
	file_row_delegate::file_row_delegate(QObject* parent /*= nullptr*/) : QStyledItemDelegate(parent)
	{
		m_icon_font = QFont(software::font_awesome);
		m_icon_font.setPixelSize(resource::get_icon_size());
	}

	// Utility
	void file_row_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			const auto transfer = entity->get<transfer_component>();
			if (!transfer->is_paused())
			{
				const auto variant = index.data(Qt::UserRole);
				QRect rect(option.rect.left(), option.rect.top(), option.rect.width() - resource::get_icon_padding(), option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight, QString::number(variant.toUInt() + 1));
			}
			else
			{
				// Color
				const auto status = entity->get<status_component>();
				const auto color = QColor(status->get_color());
				painter->setPen(color);

				painter->setFont(m_icon_font);
				painter->drawText(option.rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa::pause));
			}			

			painter->restore();
		}
	}

	QSize file_row_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(option.rect.width(), resource::get_row_height());
	}
}