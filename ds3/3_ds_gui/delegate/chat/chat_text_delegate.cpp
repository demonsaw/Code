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

#include "chat_text_delegate.h"
#include "component/chat_component.h"
#include "entity/entity.h"
#include "system/type.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	QWidget* chat_text_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

	void chat_text_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
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

	void chat_text_delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		if (QTextEdit* widget = qobject_cast<QTextEdit*>(editor))
			model->setData(index, widget->toPlainText(), Qt::EditRole);
		else
			QStyledItemDelegate::setModelData(editor, model, index);
	}

	void chat_text_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QTextEdit* widget = qobject_cast<QTextEdit*>(editor);
		widget->setMaximumWidth(option.rect.width());

		QRect rect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
		editor->setGeometry(rect);
	}

	QSize chat_text_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString qstr = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(qstr);
		const int width = (option.rect.width() > offset::invalid) ? option.rect.width() : offset::width;

		const auto str = qstr.toStdString();
		string_tokenizer newlines(str, boost::char_separator<char>("\n"));
		auto rows = qstr.count('\n');

		if (rows)
		{
			const auto str = qstr.toStdString();
			string_tokenizer newlines(str, boost::char_separator<char>("\n"));

			for (const std::string& token : newlines)
			{
				std::string line;
				string_tokenizer spaces(token, boost::char_separator<char>(" "));
				size_t num_spaces = qstr.count(' ');
				size_t num_words = 0;

				for (const std::string& token : spaces)
				{
					line += token;
					if (num_words < num_spaces)
						line += " ";

					QRect rect = metrics.boundingRect(QString::fromStdString(line));
					if (static_cast<size_t>(rect.width() + offset::left + 4) >= width)
					{
						line = token;
						if (num_words < num_spaces)
							line += " ";

						rows += ((rect.width() + offset::left + 4) / width);
					}

					++num_words;
				}
			}

			++rows;
		}
		else
		{
			std::string line;
			string_tokenizer spaces(str, boost::char_separator<char>(" "));
			size_t num_spaces = qstr.count(' ');
			size_t num_words = 0;

			for (const std::string& token : spaces)
			{
				line += token;
				if (num_words < num_spaces)
					line += " ";

				QRect rect = metrics.boundingRect(QString::fromStdString(line));
				if (static_cast<size_t>(rect.width() + offset::left + 4) >= width)
				{
					line = token;
					if (num_words < num_spaces)
						line += " ";

					rows += ((rect.width() + offset::left + 4) / width);
				}

				++num_words;
			}

			++rows;
		}

		const auto text_height = (rows == 1) ? offset::row : rect.height();
		const auto line_spacing = (rows - 1) * (option.fontMetrics.lineSpacing() - text_height);
		const auto height = (rows * text_height) + line_spacing + (offset::row - text_height);
		return QSize(rect.width(), height);

		//return QSize(rect.width(), (rows * text_height) + ((rows - 1) * (option.fontMetrics.lineSpacing() - text_height)) + (offset::row - text_height));
	}

	void chat_text_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
			auto flags = Qt::TextWordWrap | Qt::AlignVCenter;
			QFontMetrics metrics(option.font);
			string_tokenizer tokens(chat->get_text(), boost::char_separator<char>(" "));

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

			QRect rect(option.rect.left() + offset::left, option.rect.top(), option.rect.width() - (offset::left * 2), option.rect.height());

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			painter->drawText(rect, flags, QString::fromStdString(chat->get_text()));

			painter->restore();
		}
	}
}
