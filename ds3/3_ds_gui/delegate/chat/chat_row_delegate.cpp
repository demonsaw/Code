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

#include "component/chat_component.h"
#include "delegate/chat/chat_row_delegate.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	QSize chat_row_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		const auto str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		rect.setWidth(rect.width() + offset::text + offset::padding);

		return rect.size();
	}

	void chat_row_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		// Entity
		const auto variant = index.data(Qt::DisplayRole);
		const auto entity = variant.value<entity::ptr>();
		if (!entity)
			return;

		if (option.state & QStyle::State_Enabled)
		{			
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			// Icon
			const auto chat = entity->get<chat_component>();
			if (chat->has_client())
			{
				QFont font(demonsaw::font_awesome);
				font.setPixelSize(14);
				painter->setFont(font);
				
				QRect rect(option.rect.left(), option.rect.top() + offset::top, option.rect.width(), option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignHCenter, QString(fa::envelope));
			}
			/*else if (chat->has_type())
			{
				QPixmap pixmap(resource::logo_rgb);
				QRect rect((option.rect.width() >> 1) - 8, option.rect.top() + offset::top - 1, 16, 16);
				painter->drawPixmap(rect, pixmap.scaled(QSize(16, 16), Qt::KeepAspectRatio, Qt::SmoothTransformation));
			}*/

			painter->restore();
		}
	}
}
