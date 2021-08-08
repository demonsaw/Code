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

#include "component/io/file_component.h"
#include "component/io/folder_component.h"

#include "delegate/share/share_delegate.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility	
	void share_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

			// Checkbox
			const auto checkbox = entity->has_parent() ? 0 : (20 * resource::get_ratio());
			const auto size = resource::get_icon_size();

			if (entity->has<file_component>())
			{
				const auto file = entity->get<file_component>();
#if _WSTRING
				const auto name = QString::fromStdWString(file->get_wstem());
#else
				const auto name = QString::fromStdString(file->get_stem());
#endif
				// Name
				QRect rect(option.rect.left() + resource::get_row_height() + checkbox, option.rect.top(), option.rect.width() - resource::get_row_height() - checkbox, option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name);

				// Icon
#if _WSTRING
				const auto icon = icon_provider::get_file(file->get_wpath());
#else
				const auto icon = icon_provider::get_file(file->get_path());
#endif
				const auto pixmap = icon.pixmap(size, size);

				// Pixmap
				rect = QRect(option.rect.left() + resource::get_icon_padding() + checkbox, option.rect.top() + (option.rect.height() - size) / 2, size, size);
				painter->drawPixmap(rect, pixmap);
			}
			else //if (entity->has<folder_component>())
			{
				const auto folder = entity->get<folder_component>();
#if _WSTRING
				const auto name = QString::fromStdWString(!folder->is_drive() ? folder->get_wname() : folder->get_wpath());
#else
				const auto name = QString::fromStdString(!folder->is_drive() ? folder->get_name() : folder->get_path());
#endif
				QRect rect(option.rect.left() + resource::get_row_height() + checkbox, option.rect.top(), option.rect.width() - resource::get_row_height() - checkbox, option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name);

				// Icon
				const auto icon = folder->is_drive() ? icon_provider::get_drive() : icon_provider::get_folder();
				const auto pixmap = icon.pixmap(size, size);

				// Pixmap
				rect = QRect(option.rect.left() + resource::get_icon_padding() + checkbox, option.rect.top() + (option.rect.height() - size) / 2, size, size);
				painter->drawPixmap(rect, pixmap);
			}	

			painter->restore();
		}
	}

	QSize share_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		const auto variant = index.data(Qt::DisplayRole);
		const auto entity = variant.value<entity::ptr>();

		QFontMetrics metrics(option.font);
		int width;

		if (entity->has<file_component>())
		{
			const auto file = entity->get<file_component>();
#if _WSTRING
			width = metrics.width(QString::fromStdWString(file->get_wstem()));
#else
			width = metrics.width(QString::fromStdString(file->get_stem()));
#endif
		}
		else //if (entity->has<folder_component>())
		{			
			const auto folder = entity->get<folder_component>();			
#if _WSTRING
			width = metrics.width(QString::fromStdWString(folder->is_drive() ? folder->get_wpath() : folder->get_wname()));
#else
			width = metrics.width(QString::fromStdString(folder->is_drive() ? folder->get_path() : folder->get_name()));
#endif
			if (!entity->has_parent())
				width += (20 * resource::get_ratio());
		}

		return QSize(width + resource::get_row_height() + ((resource::get_row_height() - resource::get_icon_size()) / 2), resource::get_row_height());
	}
}
