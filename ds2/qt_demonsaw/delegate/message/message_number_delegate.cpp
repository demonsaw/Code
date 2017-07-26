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
#include <QPixmap>

#include "message_number_delegate.h"
#include "component/chat_component.h"
#include "component/color_component.h"
#include "component/group/group_component.h"
#include "component/client/client_component.h"
#include "entity/entity.h"
#include "resource/resource.h"
#include "window/window.h"

namespace eja
{
	// Utility
	void message_number_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
		editor->setGeometry(rect);
	}

	QSize message_number_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize(rect.width() + offset::left, offset::row);
	}

	void message_number_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			// Component
			const auto chat = entity->get<chat_component>();
			if (!chat)
				return;

			const auto message_vector = entity->get<message_entity_vector_component>();
			if (!message_vector)
			return;

			painter->save();
			
			if (chat->has_unread())
			{
				// Message
				QPixmap message_pixmap(resource::icon);
				QRect rect(option.rect.left() + offset::left, option.rect.top() + offset::top - 1, 16, 16);
				painter->drawPixmap(rect, message_pixmap.scaled(16, 16, Qt::KeepAspectRatio));
			}
			else if (!message_vector->empty())
			{
				// Number
				QRect rect(option.rect);
				rect.setRight(option.rect.right() - offset::number);
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignRight | Qt::AlignVCenter, QString::number(message_vector->size()));
			}

			painter->restore();
		}
	}
}
