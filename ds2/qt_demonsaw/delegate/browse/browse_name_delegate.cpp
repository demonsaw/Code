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

#include "browse_name_delegate.h"
#include "component/color_component.h"
#include "component/empty_component.h"
#include "component/client/client_component.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "window/window.h"

namespace eja
{
	// Utility
	void browse_name_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
		editor->setGeometry(rect);
	}

	QSize browse_name_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize((option.decorationSize.width() / 2) + rect.width() + offset::name, offset::row);
	}

	void browse_name_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

			if (entity->has<file_component>(false))
			{
				const auto file = entity->get<file_component>();
				if (!file)
					return;

				size = offset::icon;
				name = QString::fromStdString(file->get_name());
				const auto icon = icon_provider::get_file(file->get_path());
				pixmap = icon.pixmap(offset::icon, offset::icon);
			}
			else if (entity->has<folder_component>(false))
			{
				const auto folder = entity->get<folder_component>();
				if (!folder)
					return;

				size = offset::icon;
				name = QString::fromStdString(folder->is_drive() ? folder->get_path() : folder->get_name());
				const auto icon = folder->is_drive() ? icon_provider::get_drive() : icon_provider::get_folder();
				pixmap = icon.pixmap(offset::icon, offset::icon);
			}
			else if (entity->has<client_component>(false))
			{
				// Component
				const auto client = entity->get<client_component>();
				if (!client)
					return;

				const auto color = entity->get<color_component>();
				if (!color)
					return;
				
				size = offset::square;
				name = QString::fromStdString(client->get_join_name());
				pixmap = color->get_pixmap();

				// Pixmap
				const auto length = option.rect.bottom() - option.rect.top();		

				painter->save();
				painter->setRenderHint(QPainter::Antialiasing, true);

				// Name
				QRect rect(option.rect.left() + offset::name, option.rect.top() + offset::top, option.rect.width() - offset::name, offset::row - offset::top);
				painter->drawText(rect, Qt::TextSingleLine, name);

				// Circle				
				QBrush brush(pixmap);
				painter->setBrush(brush);
				painter->setPen(Qt::NoPen);

				rect = QRect(option.rect.left() + offset::left, option.rect.top() + offset::top, size, size);
				painter->drawEllipse(rect);

				painter->restore();

				return;
			}
			else if (entity->has<empty_component>(false))
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
				name = QString::fromStdString(error->get_text());
				const auto icon = QIcon(resource::tab::none);
				pixmap = icon.pixmap(offset::icon, offset::icon);
			}
			
			painter->save();

			// Pixmap
			const auto length = option.rect.bottom() - option.rect.top();
			QRect rect(option.rect.left() + offset::left, option.rect.top() + offset::top, size, size);
			painter->drawPixmap(rect, pixmap);

			// Name
			rect = QRect(option.rect.left() + offset::name, option.rect.top() + offset::top, option.rect.width() - offset::name, offset::row - offset::top);
			painter->drawText(rect, Qt::TextSingleLine, name);

			painter->restore();
		}
	}
}
