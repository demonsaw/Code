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

#include <QLineEdit>
#include <QPainter>

#include "group_file_delegate.h"
#include "component/pixmap_component.h"
#include "component/group/group_cipher_component.h"
#include "component/group/group_status_component.h"
#include "component/io/file_component.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "window/window.h"

namespace eja
{
	// Utility	
	void group_file_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
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

	void group_file_delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		// Save the current text of the combo box as the current value of the item
		if (QLineEdit* widget = qobject_cast<QLineEdit*>(editor))
			model->setData(index, widget->text(), Qt::EditRole);
		else
			QStyledItemDelegate::setModelData(editor, model, index);
	}

	void group_file_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left() + offset::text, option.rect.top(), option.rect.width() - offset::text, option.rect.height());
		editor->setGeometry(rect);
	}

	QSize group_file_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(!str.isEmpty() ? str : empty_path);
		return QSize((option.decorationSize.width() / 2) + rect.width() + offset::text, 0);
	}

	void group_file_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;
			
			// Component
			const auto status = entity->get<group_status_component>();
			if (!status)
				return;

			const auto file = entity->get<file_component>();
			if (!file)
				return;

			const auto cipher = entity->get<group_cipher_component>();
			if (!cipher)
				return;

			const auto group_pixmap = entity->get<pixmap_component>();
			if (!group_pixmap)
				return;

			// Status
			QPixmap pixmap;

			switch (status->get_status())
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
			QRect rect(option.rect.left() + offset::status, option.rect.top() + offset::top + 1, 13, 13);
			painter->drawPixmap(rect, pixmap.scaled(QSize(13, 13), Qt::KeepAspectRatio));

			// Data			
			if (cipher->has_data())
			{
				group_pixmap->set_path(file->get_path(), cipher->get_data());
				cipher->set_data();
			}		

			// Type			
			rect = QRect(option.rect.left() + offset::file, option.rect.top() + offset::top - 1, 16, 16);
			painter->drawPixmap(rect, file->has_path() ? group_pixmap->get_pixmap() : QIcon(resource::logo_black).pixmap(QSize(16, 16)));
			
			// Path			
			rect = QRect(option.rect.left() + offset::text, option.rect.top() + offset::top, option.rect.width() - offset::text, option.rect.height() - (offset::top * 2));
			painter->drawText(rect, Qt::TextSingleLine, file->has_path() ? QString::fromStdString(file->get_path()) : empty_path);
			painter->restore();
		}
	}
}
