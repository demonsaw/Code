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

#include <QIcon>
#include <QPainter>

#include "transfer_name_delegate.h"
#include "component/io/file_component.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "window/window.h"

namespace eja
{
	// Utility
	void transfer_name_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
		editor->setGeometry(rect);
	}

	QSize transfer_name_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize((option.decorationSize.width() / 2) + rect.width() + offset::name, 0);
	}

	void transfer_name_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
			const auto file = entity->get<file_component>();
			if (!file)
				return;

			// Type
			painter->save();
			
			const auto icon = icon_provider::get_file(file->get_path());
			QRect rect(option.rect.left() + offset::left, option.rect.top() + offset::top, offset::icon, offset::icon);
			QPixmap pixmap = icon.pixmap(QSize(offset::icon, offset::icon));
			painter->drawPixmap(rect, pixmap);

			// Name
			rect = QRect(option.rect.left() + offset::name, option.rect.top() + offset::top, option.rect.width() - offset::name, option.rect.height() - (offset::top * 2));
			painter->drawText(rect, Qt::TextSingleLine, QString::fromStdString(file->get_name()));
			painter->restore();
		}
	}
}
