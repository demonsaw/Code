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

#include <cassert>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QRadioButton>

#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/router/router_component.h"

#include "delegate/radio_button_delegate.h"
#include "entity/entity.h"
#include "resource/resource.h"
#include "thread/thread_info.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	QWidget* radio_button_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		assert(thread_info::main());

		return new QRadioButton(parent);
	}

	bool radio_button_delegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
	{
		assert(thread_info::main());

		if (event->type() == QEvent::MouseButtonRelease)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return false;

			// Dimensions
			const auto e = static_cast<QMouseEvent*>(event);

			if (option.rect.contains(e->pos()))
			{
				if (entity->disabled())
				{
					// Router
					const auto owner = entity->get_owner();
					const auto router_list = owner->get<router_list_component>();
					for (const auto& e : *router_list)
						e->set_enabled(e == entity);

					router_list->set_entity(entity);

					// Update
					const auto client = owner->get<client_component>();
					client->set_update(true);

					// Callback					
					owner->async_call(callback::router | callback::update);
					owner->async_call(callback::window | callback::update);
				}
			}
		}

		return false;
	}

	void radio_button_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		assert(thread_info::main());

		QRadioButton* widget = qobject_cast<QRadioButton*>(editor);
		widget->setMaximumWidth(option.rect.width());

		QRect rect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
		editor->setGeometry(rect);
	}

	// Utility
	void radio_button_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			painter->save();

			if (option.state & QStyle::State_Selected)
			{				
				QBrush brush(option.palette.color(QPalette::Highlight));
				painter->fillRect(option.rect, brush);
			}

			// Dimensions
			const auto width = resource::get_circle_size();
			const auto height = resource::get_circle_size();
			const auto x = option.rect.left() + resource::get_icon_padding();
			const auto y = option.rect.top() + (option.rect.height() / 2) - (height / 2);
			QRect rect(x, y, width, height);

			// Control
			QStyleOptionButton button;
			button.rect = rect;
			button.state = QStyle::State_Enabled | (entity->enabled() ? QStyle::State_On : QStyle::State_Off);
			QApplication::style()->drawControl(QStyle::CE_RadioButton, &button, painter);

			// Text
			variant = index.data(Qt::UserRole);
			rect = QRect(option.rect.left(), option.rect.top(), option.rect.width() - resource::get_icon_padding(), option.rect.height());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight, QString::number(variant.toUInt() + 1));

			painter->restore();
		}
	}

	QSize radio_button_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(option.rect.width(), resource::get_row_height());
	}
}
