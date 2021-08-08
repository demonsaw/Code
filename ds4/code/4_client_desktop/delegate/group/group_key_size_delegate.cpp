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

#include "delegate/group/group_key_size_delegate.h"
#include "resource/resource.h"

namespace eja
{
	// Editor
	QWidget* group_key_size_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QComboBox* widget = new QComboBox(parent);
		widget->addItem("128", 128);
		widget->addItem("192", 192);
		widget->addItem("256", 256);

		return widget;
	}

	void group_key_size_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
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

	void group_key_size_delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		if (QComboBox* widget = qobject_cast<QComboBox*>(editor))
			model->setData(index, widget->currentData(), Qt::EditRole);
		else
			QStyledItemDelegate::setModelData(editor, model, index);
	}

	// Utility
	QSize group_key_size_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(80 * resource::get_ratio(), resource::get_row_height());
	}
}
