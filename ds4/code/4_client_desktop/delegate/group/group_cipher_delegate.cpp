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

#include <QComboBox>
#include <QListView>
#include <QPainter>

#include "component/group/group_option_component.h"
#include "delegate/group/group_cipher_delegate.h"
#include "entity/entity.h"
#include "resource/resource.h"
#include "security/cipher/aes.h"
#include "security/cipher/mars.h"
#include "security/cipher/rc6.h"
#include "security/cipher/serpent.h"
#include "security/cipher/twofish.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Editor
	QWidget* group_cipher_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QComboBox* widget = new QComboBox(parent);
		widget->addItem(cipher_name::aes);
		widget->addItem(cipher_name::mars);
		widget->addItem(cipher_name::rc6);
		widget->addItem(cipher_name::serpent);
		widget->addItem(cipher_name::twofish);

		return widget;
	}

	void group_cipher_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		if (QComboBox* widget = qobject_cast<QComboBox*>(editor)) 
		{
			const auto data = index.data(Qt::EditRole).toString();
			widget->setCurrentText(data);
		}
		else 
		{
			QStyledItemDelegate::setEditorData(editor, index);
		}
	}

	void group_cipher_delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		if (QComboBox* widget = qobject_cast<QComboBox*>(editor))			
			model->setData(index, widget->currentText(), Qt::EditRole);
		else
			QStyledItemDelegate::setModelData(editor, model, index);
	}

	// Utility	
	void group_cipher_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

			// Name
			const auto security = entity->get<group_option_component>();
			const auto name = security->get_cipher();

			QRect rect = QRect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, name.c_str());
			
			painter->restore();
		}
	}

	QSize group_cipher_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(80 * resource::get_ratio(), resource::get_row_height());
	}
}
