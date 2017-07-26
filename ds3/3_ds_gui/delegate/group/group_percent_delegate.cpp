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

#include <QDoubleSpinBox>

#include "delegate/group/group_percent_delegate.h"

namespace eja
{
	// Utility
	QWidget* group_percent_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QDoubleSpinBox* widget = new QDoubleSpinBox(parent);
		widget->setRange(1.0, 100.0);		
		widget->setSingleStep(0.01);
		widget->setDecimals(3);

		return widget;
	}

	void group_percent_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		if (QDoubleSpinBox* widget = qobject_cast<QDoubleSpinBox*>(editor))
		{
			const auto value = index.data(Qt::EditRole).toDouble();
			widget->setValue(value);
		}
		else
		{
			QStyledItemDelegate::setEditorData(editor, index);
		}
	}

	void group_percent_delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		// Save the current text of the combo box as the current value of the item
		if (QDoubleSpinBox* widget = qobject_cast<QDoubleSpinBox*>(editor))
			model->setData(index, widget->value(), Qt::EditRole);
		else
			QStyledItemDelegate::setModelData(editor, model, index);
	}
}
