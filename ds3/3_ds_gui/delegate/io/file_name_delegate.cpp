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

#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "delegate/io/file_name_delegate.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	QSize file_name_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize((option.decorationSize.width() / 2) + rect.width() + offset::name, offset::row);
	}

	void file_name_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;			
			
			QString name;
			QPixmap pixmap;

			if (entity->has<file_component>())
			{
				const auto file = entity->get<file_component>();
				name = QString::fromStdString(file->get_stem());
				const auto icon = icon_provider::get_file(file->get_path());
				pixmap = icon.pixmap(offset::icon, offset::icon);
			}
			else if (entity->has<folder_component>())
			{
				const auto folder = entity->get<folder_component>();
				name = QString::fromStdString(folder->drive() ? folder->get_path() : folder->get_name());
				const auto icon = folder->drive() ? icon_provider::get_drive() : icon_provider::get_folder();
				pixmap = icon.pixmap(offset::icon, offset::icon);
			}
			
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			// Pixmap
			const auto length = option.rect.bottom() - option.rect.top();
			QRect rect(option.rect.left() + offset::left, option.rect.top() + offset::top, offset::icon, offset::icon);
			painter->drawPixmap(rect, pixmap);

			// Name
			rect = QRect(option.rect.left() + offset::name, option.rect.top(), option.rect.width() - offset::name, offset::row);
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name);

			painter->restore();
		}
	}
}
