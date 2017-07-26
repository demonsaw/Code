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
#include <QColor>
#include <QComboBox>
#include <QListView>
#include <QRect>
#include <QPainter>

#include "group_hash_delegate.h"
#include "component/group/group_security_component.h"
#include "object/group/group_security.h"
#include "entity/entity.h"
#include "window/window.h"

namespace eja
{
	// Utility
	QWidget* group_hash_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QComboBox* widget = new QComboBox(parent);
		widget->setObjectName("pane");

		QListView* view = new QListView(parent);
		view->setObjectName("pane");
		widget->setView(view);

		widget->addItem(group_hash_name::sha256, static_cast<int>(group_hash_type::sha256));
		widget->addItem(group_hash_name::sha384, static_cast<int>(group_hash_type::sha384));
		widget->addItem(group_hash_name::sha512, static_cast<int>(group_hash_type::sha512));
		widget->addItem(group_hash_name::sha3_256, static_cast<int>(group_hash_type::sha3_256));
		widget->addItem(group_hash_name::sha3_384, static_cast<int>(group_hash_type::sha3_384));
		widget->addItem(group_hash_name::sha3_512, static_cast<int>(group_hash_type::sha3_512));
		widget->addItem(group_hash_name::pbkdf_sha256, static_cast<int>(group_hash_type::pbkdf_sha256));
		widget->addItem(group_hash_name::pbkdf_sha384, static_cast<int>(group_hash_type::pbkdf_sha384));
		widget->addItem(group_hash_name::pbkdf_sha512, static_cast<int>(group_hash_type::pbkdf_sha512));

		return widget;
	}

	void group_hash_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		if (QComboBox* widget = qobject_cast<QComboBox*>(editor))
		{
			const auto data = index.data(Qt::EditRole).toUInt();
			const auto index = widget->findData(data);

			if (index >= 0)
				widget->setCurrentIndex(index);
		}
		else
		{
			QStyledItemDelegate::setEditorData(editor, index);
		}
	}

	void group_hash_delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		if (QComboBox* widget = qobject_cast<QComboBox*>(editor))
			model->setData(index, widget->currentData(), Qt::EditRole);
		else
			QStyledItemDelegate::setModelData(editor, model, index);
	}

	QSize group_hash_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize((option.decorationSize.width() / 2) + rect.width() + offset::text, 0);
	}

	void group_hash_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
			const auto security = entity->get<group_security_component>();
			if (!security)
				return;

			// Option			
			const auto name = security->get_hash_name();
			const auto qname = QString::fromStdString(name);

			// Name
			QRect rect = QRect(option.rect.left() + offset::text, option.rect.top() + offset::top, option.rect.width() - offset::text, option.rect.height() - (offset::top * 2));
			painter->save();
			painter->drawText(rect, Qt::TextSingleLine, qname);
			painter->restore();
		}
	}
}
