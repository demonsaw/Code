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

#include "client_name_delegate.h"
#include "component/color_component.h"
#include "component/client/client_component.h"
#include "entity/entity.h"
#include "resource/resource.h"
#include "window/window.h"

namespace eja
{
	// Utility
	void client_name_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
		editor->setGeometry(rect);
	}

	QSize client_name_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize((option.decorationSize.width() / 2) + rect.width() + offset::name, offset::row);
	}

	void client_name_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

			// Name
			QRect rect(option.rect.left() + offset::name, option.rect.top() + offset::top, option.rect.width() - offset::name, offset::row - offset::top);

			if (entity->has<client_component>())
			{
				const auto client = entity->get<client_component>();
				if (client)
					painter->drawText(rect, Qt::TextSingleLine, QString::fromStdString(client->get_join_name()));
			}
			else
			{
				const auto router = entity->get<router_component>();
				if (router)
					painter->drawText(rect, Qt::TextSingleLine, QString::fromStdString(router->get_address()));
			}

			// Pixmap
			if (entity->has<color_component>())
			{
				const auto color = entity->get<color_component>();
				const auto& pixmap = color->get_pixmap();				

				// Circle				
				QBrush brush(pixmap);
				painter->setBrush(brush);
				painter->setPen(Qt::NoPen);

				rect = QRect(option.rect.left() + offset::left, option.rect.top() + offset::top, offset::square, offset::square);
				painter->drawEllipse(rect);
			}
			else
			{
				// Logo
				QPixmap pixmap(resource::icon);
				QRect rect(option.rect.left() + offset::left, option.rect.top() + offset::top, offset::square, offset::square);
				painter->drawPixmap(rect, pixmap.scaled(QSize(offset::square, offset::square), Qt::KeepAspectRatio));
			}

			painter->restore();
		}
	}
}
