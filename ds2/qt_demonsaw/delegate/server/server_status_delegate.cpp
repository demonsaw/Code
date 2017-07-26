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

#include "server_status_delegate.h"
#include "component/server/server_component.h"
#include "component/server/server_machine_component.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "window/window.h"

namespace eja
{
	// Utility
	void server_status_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		if (QLineEdit* widget = qobject_cast<QLineEdit*>(editor))
		{
			QString currentText = index.data(Qt::EditRole).toString();
			widget->setText(currentText);
		}
		else
		{
			QStyledItemDelegate::setEditorData(editor, index);
		}
	}

	void server_status_delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		// Save the current text of the combo box as the current value of the item
		if (QLineEdit* widget = qobject_cast<QLineEdit*>(editor))
			model->setData(index, widget->text(), Qt::EditRole);
		else
			QStyledItemDelegate::setModelData(editor, model, index);
	}

	void server_status_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left() + offset::text, option.rect.top(), option.rect.width() - offset::text, option.rect.height());
		editor->setGeometry(rect);
	}

	QSize server_status_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize((option.decorationSize.width() / 2) + rect.width() + offset::text, 0);
	}

	void server_status_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			// Component
			const auto server = entity->get<server_component>();
			if (!server)
				return;

			const auto machine = entity->get<server_machine_component>();
			if (!machine)
				return;

			// Status
			const auto status = machine->get_status();
			QPixmap pixmap;

			switch (status)
			{
				case eja::status::none:
				{
					pixmap = QPixmap(resource::status::none);
					break;
				}
				case eja::status::success:
				{
					pixmap = QPixmap(resource::status::success);
					break;
				}
				case eja::status::warning:
				{
					pixmap = QPixmap(resource::status::warning);
					break;
				}
				case eja::status::error:
				{
					pixmap = QPixmap(resource::status::error);
					break;
				}
				case eja::status::info:
				{
					pixmap = QPixmap(resource::status::info);
					break;
				}
				case eja::status::pending:
				{
					pixmap = QPixmap(resource::status::pending);
					break;
				}
				case eja::status::cancelled:
				{
					pixmap = QPixmap(resource::status::cancelled);
					break;
				}
				case eja::status::unknown:
				{
					pixmap = QPixmap(resource::status::unknown);
					break;
				}
				default:
				{
					assert(false);
				}
			}

			// Status
			painter->save();
			QRect rect(option.rect.left() + offset::status, option.rect.top() + 4, 13, 13);
			painter->drawPixmap(rect, pixmap.scaled(QSize(13, 13), Qt::KeepAspectRatio));

			// Name
			rect = QRect(option.rect.left() + offset::text, option.rect.top() + 3, option.rect.width() - offset::text, option.rect.height() - 3);
			painter->drawText(rect, Qt::TextSingleLine, QString::fromStdString(server->get_name()));
			painter->restore();
		}
	}
}
