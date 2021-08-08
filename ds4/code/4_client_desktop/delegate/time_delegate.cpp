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

#include "component/client/client_component.h"
#include "component/time/time_component.h"

#include "delegate/time_delegate.h"
#include "entity/entity.h"
#include "resource/resource.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	void time_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			const auto owner = entity->get_owner();
			if (unlikely(!owner))
				return;

			// Painter
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			// Time
			char buff[32];
			const auto client = owner->get<client_component>();
			const auto time = entity->get<time_component>();
			const auto tm = localtime(&time->get_time());
			strftime(buff, 32, client->get_timestamp().c_str(), tm);

			QRect rect(option.rect.left(), option.rect.top() + resource::get_icon_padding(), option.rect.width() - resource::get_icon_padding(), option.rect.height() - (resource::get_icon_padding() * 2));
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignTop | Qt::AlignRight, QString(buff));

			painter->restore();
		}
	}

	QSize time_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		const auto variant = index.data(Qt::DisplayRole);
		const auto entity = variant.value<entity::ptr>();

		// Client
		const auto owner = entity->get_owner();
		const auto client = owner->get<client_component>();

		// Time
		char buff[32];
		const auto time = entity->get<time_component>();
		const auto tm = localtime(&time->get_time());
		strftime(buff, 32, client->get_timestamp().c_str(), tm);

		QFontMetrics metrics(option.font);
		const auto width = metrics.width(buff) + resource::get_icon_padding();

		return QSize(width, resource::get_row_height());
	}
}
