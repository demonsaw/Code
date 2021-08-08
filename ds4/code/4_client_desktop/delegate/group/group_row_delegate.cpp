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
#include <QCheckBox>
#include <QMouseEvent>
#include <QPainter>
#include <QSortFilterProxyModel>

#include "component/pixmap_component.h"
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/group/group_option_component.h"
#include "component/router/router_component.h"

#include "delegate/group/group_row_delegate.h"
#include "entity/entity.h"
#include "model/group_model.h"
#include "resource/resource.h"
#include "thread/thread_info.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	QWidget* group_row_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		assert(thread_info::main());

		return new QCheckBox(parent);
	}

	bool group_row_delegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
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
				bool modified = false;

				if (entity->disabled())
				{
					entity->enable();

					const auto group_option = entity->get<group_option_component>();
					if (group_option->has_entropy())
						modified = true;
				}
				else
				{
					entity->disable();

					const auto group_option = entity->get<group_option_component>();
					if (group_option->has_entropy())
					{
						const auto pixmap = entity->get<pixmap_component>();
						pixmap->clear();

						modified = true;
					}
				}

				if (modified)
				{
					const auto proxy = qobject_cast<QSortFilterProxyModel*>(model);
					qobject_cast<group_model*>(proxy->sourceModel())->set_modified();
				}
			}
		}

		return false;
	}

	void group_row_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		assert(thread_info::main());

		QCheckBox* widget = qobject_cast<QCheckBox*>(editor);
		widget->setMaximumWidth(option.rect.width());

		QRect rect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
		editor->setGeometry(rect);
	}

	// Utility
	void group_row_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
			QApplication::style()->drawControl(QStyle::CE_CheckBox, &button, painter);

			// Text
			variant = index.data(Qt::UserRole);
			rect = QRect(option.rect.left(), option.rect.top(), option.rect.width() - resource::get_icon_padding(), option.rect.height());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight, QString::number(variant.toUInt() + 1));

			painter->restore();
		}
	}

	QSize group_row_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(option.rect.width(), resource::get_row_height());
	}
}
