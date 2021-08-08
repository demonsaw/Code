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

#include <QColor>
#include <QPainter>
#include <QTextEdit>

#include "component/error_component.h"

#include "delegate/delegate_util.h"
#include "delegate/error/error_delegate.h"
#include "entity/entity.h"
#include "resource/resource.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Editor
	QWidget* error_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Control
		QTextEdit* widget = new QTextEdit(parent);
		widget->setObjectName("error");
		widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		widget->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
		widget->setLineWrapMode(QTextEdit::WidgetWidth);
		widget->setReadOnly(true);

		return widget;
	}

	void error_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		if (QTextEdit* widget = qobject_cast<QTextEdit*>(editor))
		{
			const auto value = index.data(Qt::EditRole).toString();
			widget->setPlainText(value);
		}
		else
		{
			QStyledItemDelegate::setEditorData(editor, index);
		}
	}

	void error_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QTextEdit* widget = qobject_cast<QTextEdit*>(editor);
		QRect rect(option.rect.left() + resource::get_delegate_bar_width(), option.rect.top(), option.rect.width() - resource::get_delegate_bar_width(), option.rect.height());
		editor->setGeometry(rect);
	}

	// Utility	
	void error_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			// Painter
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			// Message			
			QFontMetrics metrics(option.font);
			auto flags = Qt::TextWordWrap;
			const auto width = option.rect.width() - resource::get_delegate_left();

			const auto error = entity->get<error_component>();
			const auto qtext = QString::fromStdString(error->get_text());
			const auto spaces = qtext.split(QRegExp("[ ]"), QString::SkipEmptyParts);

			for (const auto& token : spaces)
			{
				QRect rect = metrics.boundingRect(token);
				if (rect.width() > width)
				{
					flags = Qt::TextWrapAnywhere;
					break;
				}
			}

			// Text			
			QRect rect(option.rect.left() + resource::get_delegate_left(), option.rect.top() + resource::get_icon_padding(), option.rect.width() - resource::get_delegate_left(), option.rect.height() - (resource::get_icon_padding() * 2));
			painter->drawText(rect, flags, qtext);

			// Color
			QColor color(color::red);
			painter->setBrush(color);
			painter->setPen(Qt::NoPen);

			rect = QRect(option.rect.left(), option.rect.top() + resource::get_delegate_bar_margin(), resource::get_delegate_bar_width(), option.rect.height() - (resource::get_delegate_bar_margin() * 2));
			painter->drawRect(rect);

			painter->restore();
		}
	}

	QSize error_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		const auto variant = index.data(Qt::EditRole);
		const auto qstr = variant.toString();

		const size_t width = (option.rect.width() > resource::get_delegate_invalid()) ? option.rect.width() : resource::get_delegate_width();
		const auto rows = delegate_util::get_rows(option.font, qstr, width);

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(qstr);
		const auto height = (rows * metrics.lineSpacing()) + resource::get_icon_padding() + resource::get_icon_padding();

		return QSize(width, height);
	}

}
