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
#include "component/room_component.h"
#include "component/client/client_component.h"

#include "delegate/client/client_delegate.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "system/type.h"
#include "utility/value.h"
#include "window/main_window.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Constructor
	client_delegate::client_delegate(QObject* parent /*= nullptr*/) : QStyledItemDelegate(parent)
	{
		m_circle_font = QFont(software::font_awesome);
		m_circle_font.setPixelSize(resource::get_circle_size());

		m_icon_font = QFont(software::font_awesome);
		m_icon_font.setPixelSize(resource::get_icon_size());
	}

	// Utility
	void client_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			const auto icon_size = resource::get_icon_size() + (resource::get_icon_padding() * 2);

			// Name
			if (entity->has<client_component>())
			{
				// HACK: Remove tree offset
				const_cast<QStyleOptionViewItem&>(option).rect.setLeft(option.rect.left() - resource::get_tree_view_indent());
				const_cast<QStyleOptionViewItem&>(option).rect.setRight(option.rect.right() + resource::get_tree_view_indent());

				QStyledItemDelegate::paint(painter, option, index);

				const auto client = entity->get<client_component>();
				QString name = QString::fromStdString(client->get_name());
				if (!entity->has_owner())
				{
					if (client->is_troll())
						name += suffix::troll;
				}

				QRect rect(option.rect.left() + resource::get_row_height(), option.rect.top(), option.rect.width() - resource::get_row_height(), option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name);

				// Size
				if (entity->has_owner() && entity->has<chat_list_component>())
				{
					const auto chat_list = entity->get<chat_list_component>();
					if (chat_list->has_rows())
					{
						// Text
						const auto number = QString::number(chat_list->get_rows());
						const auto width = option.rect.width() - resource::get_tree_view_indent() - resource::get_icon_size() - (resource::get_icon_padding() * 2);
						rect = QRect(option.rect.left(), option.rect.top(), width, option.rect.height());
						painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight, number);

						// Alpha
						int fa;
						if (chat_list->is_notify())
						{
							fa = fa::envelope;

							const auto& app = main_window::get_app();
							if (app.is_user_colors())
								painter->setPen(QColor(client->get_color()));
							else
								painter->setPen(option.palette.color(QPalette::WindowText));
						}
						else
						{
							fa = fa::envelope_o;
							auto color = option.palette.color(QPalette::WindowText);
							color.setAlpha(32);
							painter->setPen(color);
						}

						// Icon
						painter->setFont(m_icon_font);

						const auto left = option.rect.right() - resource::get_tree_view_indent() - icon_size;
						rect = QRect(left, option.rect.top(), option.rect.right() - left - resource::get_tree_view_indent(), option.rect.height());
						painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa));
						painter->setPen(option.palette.color(QPalette::WindowText));
					}
				}

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
			else
			{
				QStyledItemDelegate::paint(painter, option, index);

				// Name
				const auto room = entity->get<room_component>();
				QRect rect(option.rect.left() + icon_size, option.rect.top(), option.rect.width() - icon_size, option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, QString::fromStdString(room->get_name()));

				// Text
				const auto number = QString::number(room->get_size());
				rect = QRect(option.rect.left(), option.rect.top(), option.rect.width() - icon_size, option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight, number);

				// Alpha
				auto color = option.palette.color(QPalette::WindowText);
				color.setAlpha(32);
				painter->setPen(color);

				// Icon
				painter->setFont(m_icon_font);

				const auto left = option.rect.right() - icon_size;
				rect = QRect(left, option.rect.top(), option.rect.right() - left, option.rect.height());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa::user_o));
				painter->setPen(option.palette.color(QPalette::WindowText));

				// Icon
				painter->setFont(m_icon_font);

				if (room->has_color())
				{
					const auto& app = main_window::get_app();
					if (app.is_user_colors())
						painter->setPen(QColor(room->get_color()));
				}

				rect = QRect(option.rect.left() + resource::get_icon_padding(), option.rect.top() + (resource::get_row_height() / 2) - (resource::get_icon_size() / 2), resource::get_icon_size(), resource::get_icon_size());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa::hash_tag));
			}

			painter->restore();
		}
	}

	QSize client_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(option.rect.width(), resource::get_row_height());
	}
}
