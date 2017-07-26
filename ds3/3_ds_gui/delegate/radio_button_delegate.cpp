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

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QRadioButton>

#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"
#include "component/message/message_service_component.h"
#include "delegate/radio_button_delegate.h"
#include "entity/entity.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	QWidget* radio_button_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return new QRadioButton(parent);
	}

	void radio_button_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QRadioButton* widget = qobject_cast<QRadioButton*>(editor);
		widget->setMaximumWidth(option.rect.width());

		QRect rect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
		editor->setGeometry(rect);
	}

	void radio_button_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		if (option.state & QStyle::State_Selected)
		{
			painter->save();

			QBrush brush(option.palette.color(QPalette::Highlight));
			painter->fillRect(option.rect, brush);

			painter->restore();

		}

		// Entity
		const auto variant = index.data(Qt::DisplayRole);
		const auto entity = variant.value<entity::ptr>();
		if (!entity)
			return;
		
		// Dimensions
		const auto w = 16;
		const auto h = 16;
		const auto x = option.rect.left() + (option.rect.width() / 2) - (w / 2);
		const auto y = option.rect.top() + (option.rect.height() / 2) - (h / 2);

		QStyleOptionButton button;
		button.rect = QRect(x, y, w, h);
		button.state = QStyle::State_Enabled | (entity->enabled() ? QStyle::State_On : QStyle::State_Off);

		QApplication::style()->drawControl(QStyle::CE_RadioButton, &button, painter);
	}

	bool radio_button_delegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
	{
		if (event->type() == QEvent::MouseButtonRelease)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return false;

			QMouseEvent * e = (QMouseEvent *)event;
			const auto clickX = e->x();
			const auto clickY = e->y();

			// Dimensions			
			const auto w = 16;
			const auto h = 16;
			const auto x = option.rect.left() + (option.rect.width() / 2) - (w / 2);
			const auto y = option.rect.top() + (option.rect.height() / 2) - (h / 2);

			if (clickX > x && clickX < x + w)
			{
				if (clickY > y && clickY < y + h)
				{
					if (entity->disabled())
					{
						entity->enable();
						const auto parent = entity->get_parent();
						const auto message_list = parent->get<message_list_component>();

						for (const auto& e : *message_list)
						{
							if (e == entity)
								continue;
							
							const auto message_service = e->get<message_service_component>();
							if (message_service->valid())								
								message_service->async_quit();

							e->disable();
						}
						
						// Callback
						parent->call(callback_type::chat, callback_action::clear);
						parent->call(callback_type::client, callback_action::clear);
						parent->call(callback_type::browse, callback_action::clear);
						parent->call(callback_type::error, callback_action::clear);
						parent->call(callback_type::transfer, callback_action::clear);
						parent->call(callback_type::download, callback_action::clear);
						parent->call(callback_type::upload, callback_action::clear);						

						const auto client_service = parent->get<client_service_component>();						
						if (client_service->valid())
						{
							client_service->update();

							const auto message_service = entity->get<message_service_component>();
							message_service->start();
						}
					}
				}
			}
		}

		return false;
	}
}
