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
#include <QTextEdit>
#include <QTextDocument>

#include "chat_message_delegate.h"
#include "component/chat_component.h"
#include "entity/entity.h"
#include "utility/boost.h"
#include "window/window.h"

namespace eja
{
	// Utility
	QWidget* chat_message_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QTextEdit* widget = new QTextEdit(parent);		
		widget->setObjectName("chat");
		widget->setReadOnly(true);
		widget->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextBrowserInteraction);
		widget->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

		widget->setLineWrapMode(QTextEdit::WidgetWidth);
		widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		return widget;
	}

	void chat_message_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
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

	void chat_message_delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		if (QTextEdit* widget = qobject_cast<QTextEdit*>(editor))
			model->setData(index, widget->toPlainText(), Qt::EditRole);
		else
			QStyledItemDelegate::setModelData(editor, model, index);
	}

	void chat_message_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QTextEdit* widget = qobject_cast<QTextEdit*>(editor);
		widget->setMaximumWidth(option.rect.width());

		QRect rect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
		editor->setGeometry(rect);
	}

	QSize chat_message_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString qstr = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(qstr);
		const int width = (option.rect.width() > offset::invalid) ? option.rect.width() : offset::width;

		size_t rows = qstr.count('\n');
		if (rows)
		{
			const auto str = qstr.toStdString();
			boost::string_tokenizer tokens(str, boost::char_separator<char>("\n"));

			for (const std::string& token : tokens)
			{
				QRect rect = metrics.boundingRect(QString::fromStdString(token));
				if (rect.width() > width)
				{
					rows += (rect.width() / width) - 1;
					if (rect.width() % width)
						rows++;
				}
			}

			rows++;
		}
		else
		{
			rows = rect.width() / width;
			if (rect.width() % width)
				rows++;
		}

		size_t height = (rows == 1) ? 23 : ((rows * rect.height()) + (offset::top * 2));

		return QSize((option.decorationSize.width() / 2) + rect.width(), height);
	}

	void chat_message_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		// Entity
		const auto variant = index.data(Qt::DisplayRole);
		const auto entity = variant.value<entity::ptr>();
		if (!entity)
			return;

		// Component
		const auto chat = entity->get<chat_component>();
		if (!chat)
			return;

		if (option.state & QStyle::State_Enabled)
		{
			// Message			
			auto flags = Qt::TextWordWrap;
			QFontMetrics metrics(option.font);
			boost::string_tokenizer tokens(chat->get_text(), boost::char_separator<char>(" "));
			
			for (const std::string& token : tokens)
			{
				const auto qstr = QString::fromStdString(token);
				QRect rect = metrics.boundingRect(qstr);
				if (rect.width() > option.rect.width())
				{
					flags = Qt::TextWrapAnywhere;
					break;
				}
			}

			QRect rect(option.rect.left() + offset::left, option.rect.top() + offset::top, option.rect.width() - (offset::left * 2), option.rect.height() - (offset::top * 2));

			painter->save();
			painter->drawText(rect, flags, QString::fromStdString(chat->get_text()));
			painter->restore();
		}
	}
}
