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

#include "component/endpoint_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "delegate/browse/browse_delegate.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	QSize browse_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize((option.decorationSize.width() >> 1) + rect.width() + offset::text, offset::row);
	}

	void browse_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;			
			
			size_t size;
			QString name;
			QPixmap pixmap;

			if (entity->has<file_component>())
			{
				size = offset::icon;

				const auto file = entity->get<file_component>();
				name = QString::fromStdString(file->get_stem());
				
				const auto icon = icon_provider::get_file(file->get_path());
				pixmap = icon.pixmap(offset::icon, offset::icon);
			}
			else if (entity->has<folder_component>())
			{
				size = offset::icon;

				const auto folder = entity->get<folder_component>();
				name = QString::fromStdString(folder->drive() ? folder->get_path() : folder->get_name());

				const auto icon = folder->drive() ? icon_provider::get_drive() : icon_provider::get_folder();
				pixmap = icon.pixmap(offset::icon, offset::icon);
			}
			else
			{
				painter->save();
				painter->setRenderHint(QPainter::Antialiasing, true);

				// Name
				const auto endpoint = entity->get<endpoint_component>();
				QRect rect(option.rect.left() + offset::text, option.rect.top(), option.rect.width() - offset::text, offset::row);
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, QString::fromStdString(endpoint->get_name()));

				// Circle
				QColor color;
				color.setRgba(endpoint->get_color());
				painter->setBrush(color);

				QColor outline = QColor(color.red(), color.green(), color.blue(), color.alpha() >> 1);
				painter->setPen(outline);

				rect = QRect(option.rect.left() + offset::padding + 1, option.rect.top() + offset::top + 1, offset::circle, offset::circle);
				painter->drawEllipse(rect);

				painter->restore();

				return;
			}
			/*else if (entity->has<empty_component>(false))
			{
				size = offset::icon;
				name = "Not sharing any files";
				const auto icon = QIcon(resource::tab::sad);
				pixmap = icon.pixmap(offset::icon, offset::icon);
			}
			else if (entity->has<error_component>(false))
			{
				const auto error = entity->get<error_component>();
				if (!error)
					return;

				size = offset::icon;
				name = QString::fromStdString(error->get_message());
				const auto icon = QIcon(resource::tab::none);
				pixmap = icon.pixmap(offset::icon, offset::icon);
			}*/
			
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			// Pixmap
			const auto length = option.rect.bottom() - option.rect.top();
			QRect rect(option.rect.left() + offset::padding, option.rect.top() + offset::top, size, size);
			painter->drawPixmap(rect, pixmap);

			// Name
			rect = QRect(option.rect.left() + offset::text, option.rect.top(), option.rect.width() - offset::text, offset::row);
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name);

			painter->restore();
		}
	}
}
