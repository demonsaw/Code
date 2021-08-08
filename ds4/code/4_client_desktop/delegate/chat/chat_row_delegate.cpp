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
#include "component/router/router_component.h"

#include "delegate/chat/chat_row_delegate.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "utility/value.h"
#include "window/main_window.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Constructor
	chat_row_delegate::chat_row_delegate(QObject* parent /*= nullptr*/) : QStyledItemDelegate(parent)
	{
		m_circle_font = QFont(software::font_awesome);
		m_circle_font.setPixelSize(resource::get_circle_size());

		QImage image(resource::logo_rgb);
		m_logo = image.scaled(QSize(resource::get_circle_size(), resource::get_circle_size()), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	// Utility
	void chat_row_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

			if (!entity->has<router_component>())
			{
				const auto owner = entity->get_owner();
				const auto client = owner->get<client_component>();

				// Icon
				auto fa = fa::none;

				if (client->is_mute())
					fa = fa::ban;
				else if (client->is_verified())
					fa = fa::check_circle;
				else if (client->is_troll())
					fa = fa::question_circle;

				if (fa != fa::none)
				{
					painter->setFont(m_circle_font);

					const auto& app = main_window::get_app();
					if (app.is_user_colors())
						painter->setPen(QColor(client->get_color()));

					QRect rect(option.rect.left() + resource::get_icon_padding(), option.rect.top() + (resource::get_row_height() / 2) - (resource::get_circle_size() / 2), option.rect.width(), resource::get_circle_size());
					painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa));
				}
			}
			else
			{
				QRect rect(option.rect.center().x() - (resource::get_circle_size() / 2), option.rect.top() + (resource::get_row_height()/ 2) - (resource::get_circle_size() / 2), resource::get_circle_size(), resource::get_circle_size());
				painter->drawImage(rect, m_logo);
			}

			painter->restore();
		}
	}

	QSize chat_row_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(option.rect.width(), resource::get_row_height());
	}
}
