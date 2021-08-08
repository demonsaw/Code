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
#include "component/status/status_component.h"
#include "component/transfer/transfer_component.h"

#include "delegate/io/file_name_delegate.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Constructor
	file_name_delegate::file_name_delegate(QObject* parent /*= nullptr*/) : QStyledItemDelegate(parent)
	{
		m_icon_font = QFont(software::font_awesome);
		m_icon_font.setPixelSize(resource::get_icon_size());
	}

	// Utility
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

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

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
				QRect rect(option.rect.left() + resource::get_row_height(), option.rect.top(), option.rect.width() - resource::get_row_height(), option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name);

				// Icon
#if _WSTRING
				const auto icon = icon_provider::get_file(file->get_wpath());
#else
				const auto icon = icon_provider::get_file(file->get_path());
#endif
				const auto pixmap = icon.pixmap(size, size);

				// Pixmap
				rect = QRect(option.rect.left() + resource::get_icon_padding(), option.rect.top() + (option.rect.height() - size) / 2, size, size);
				painter->drawPixmap(rect, pixmap);
			}
			else //if (entity->has<folder_component>())
			{
				const auto folder = entity->get<folder_component>();
#if _WSTRING
				const auto name = QString::fromStdWString(folder->get_wname());
#else
				const auto name = QString::fromStdString(folder->get_name());
#endif
				// Name
				QRect rect(option.rect.left() + resource::get_row_height(), option.rect.top(), option.rect.width() - resource::get_row_height(), option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name);

				// Icon
				const auto icon = folder->is_drive() ? icon_provider::get_drive() : icon_provider::get_folder();
				const auto pixmap = icon.pixmap(size, size);

				// Pixmap
				rect = QRect(option.rect.left() + resource::get_icon_padding(), option.rect.top() + (option.rect.height() - size) / 2, size, size);
				painter->drawPixmap(rect, pixmap);
			}

			// Color
			if (entity->has<status_component>())
			{
				const auto status = entity->get<status_component>();
				const auto color = QColor(status->get_color());
				painter->setPen(color);

				// Arrow
				const auto transfer = entity->get<transfer_component>();
				const auto fa = transfer->is_download() ? fa::arrow_down : fa::arrow_up;
				QRect rect(option.rect.left(), option.rect.top(), option.rect.width() - resource::get_icon_padding(), option.rect.height());

				painter->setFont(m_icon_font);
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignRight | Qt::AlignVCenter, QString(fa));
			}

			painter->restore();
		}
	}

	QSize file_name_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
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
		}

		if (entity->has<status_component>())
			width += (resource::get_icon_size() + resource::get_icon_padding());

		return QSize(width + resource::get_row_height() + ((resource::get_row_height() - resource::get_icon_size()) / 2), resource::get_row_height());
	}
}
