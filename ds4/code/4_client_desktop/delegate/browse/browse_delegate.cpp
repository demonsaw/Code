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
#include "component/io/file_component.h"
#include "component/io/folder_component.h"

#include "delegate/browse/browse_delegate.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "utility/value.h"
#include "window/main_window.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Constructor
	browse_delegate::browse_delegate(QObject* parent /*= nullptr*/) : QStyledItemDelegate(parent)
	{
		m_circle_font = QFont(software::font_awesome);
		m_circle_font.setPixelSize(resource::get_circle_size());
	}

	// Utility
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

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			if (entity->has<file_component>())
			{
				const auto file = entity->get<file_component>();
#if _WSTRING
				const auto name = QString::fromStdWString(file->get_wstem());
#else
				const auto name = QString::fromStdString(file->get_stem());
#endif
				// Name
				QRect rect(option.rect.left() + resource::get_row_height(), option.rect.top(), option.rect.width() - resource::get_row_height(), resource::get_row_height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name);

				// Icon
				const auto size = resource::get_icon_size();
#if _WSTRING
				const auto icon = icon_provider::get_file(file->get_wpath());
#else
				const auto icon = icon_provider::get_file(file->get_path());
#endif
				const auto pixmap = icon.pixmap(size, size);

				rect = QRect(option.rect.left() + resource::get_icon_padding(), option.rect.top() + (option.rect.height() - size) / 2, size, size);
				painter->drawPixmap(rect, pixmap);
			}
			else if (entity->has<folder_component>())
			{
				const auto folder = entity->get<folder_component>();
#if _WSTRING
				const auto name = QString::fromStdWString(!folder->is_drive() ? folder->get_wname() : folder->get_wpath());
#else
				const auto name = QString::fromStdString(!folder->is_drive() ? folder->get_name() : folder->get_path());
#endif
				// Name
				QRect rect(option.rect.left() + resource::get_row_height(), option.rect.top(), option.rect.width() - resource::get_row_height(), resource::get_row_height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name);

				// Icon
				const auto size = resource::get_icon_size();
				const auto icon = folder->is_drive() ? icon_provider::get_drive() : icon_provider::get_folder();
				const auto pixmap = icon.pixmap(size, size);

				rect = QRect(option.rect.left() + resource::get_icon_padding(), option.rect.top() + (option.rect.height() - size) / 2, size, size);
				painter->drawPixmap(rect, pixmap);
			}
			else
			{
				// Name
				const auto client = entity->find<client_component>();
				QString name = QString::fromStdString(client->get_name());
				if (!entity->has_owner())
				{
					if (client->is_troll())
						name += suffix::troll;
				}

				QRect rect(option.rect.left() + resource::get_row_height(), option.rect.top(), option.rect.width() - resource::get_row_height(), option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name);

				// Icon
				auto fa = fa::circle;

				if (client->is_mute())
					fa = fa::ban;
				else if (client->is_verified())
					fa = fa::check_circle;
				else if (client->is_troll())
					fa = fa::question_circle;

				painter->setFont(m_circle_font);

				const auto& app = main_window::get_app();
				if (app.is_user_colors())
					painter->setPen(QColor(client->get_color()));

				rect = QRect(option.rect.left(), option.rect.top(), resource::get_row_height(), resource::get_row_height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa));
			}

			painter->restore();
		}
	}

	QSize browse_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		const auto variant = index.data(Qt::EditRole);
		const auto qstr = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		const auto rect = metrics.boundingRect(qstr);
		return QSize(rect.width() + resource::get_row_height() + ((resource::get_row_height() - resource::get_icon_size()) / 2), resource::get_row_height());
	}
}
