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
#include <QPainter>
#include <QTextEdit>

#include "component/chat_component.h"
#include "component/client/client_component.h"
#include "component/router/router_component.h"
#include "component/time/time_component.h"

#include "delegate/delegate_util.h"
#include "delegate/chat/chat_delegate.h"
#include "entity/entity.h"
#include "resource/resource.h"
#include "system/type.h"
#include "utility/value.h"
#include "window/main_window.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Constructor
	chat_delegate::chat_delegate(QObject* parent /*= nullptr*/) : QStyledItemDelegate(parent)
	{
		m_name_font = QApplication::font();
		m_name_font.setPixelSize(resource::get_font_size());
		m_name_font.setWeight(QFont::DemiBold);
	}

	// Editor
	QWidget* chat_delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Control
		QTextEdit* widget = new QTextEdit(parent);
		widget->setObjectName("chat");
		widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		widget->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
		widget->setLineWrapMode(QTextEdit::WidgetWidth);
		widget->setAcceptRichText(false);
		widget->setReadOnly(true);

		QFontMetrics metrics(option.font);
		widget->setTabStopWidth(metrics.width(' '));

		return widget;
	}

	void chat_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
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

	void chat_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left() + resource::get_delegate_left(), option.rect.top() + resource::get_row_height(), option.rect.width() - resource::get_delegate_left() - resource::get_delegate_right(), option.rect.height() - resource::get_row_height());
		editor->setGeometry(rect);
	}

	// Utility
	QSize chat_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		const auto qstr = variant.toString();

		const size_t width = (option.rect.width() > resource::get_delegate_invalid()) ? (option.rect.width() - resource::get_delegate_left() - resource::get_delegate_right()) : resource::get_delegate_width();
		const auto rows = delegate_util::get_rows(option.font, qstr, width);

		// Size
		QFontMetrics metrics(option.font);
		const auto rect = metrics.boundingRect(qstr);

		const auto height = ((rows + 1) * metrics.lineSpacing()) + (resource::get_delegate_top() * 2) + resource::get_delegate_bar_margin() + resource::get_delegate_bottom();
		return QSize(width, height);
	}

	void chat_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
			const auto width = option.rect.width() - resource::get_delegate_left() - resource::get_delegate_right();

			const auto chat = entity->get<chat_component>();
			const auto qtext = QString::fromStdString(chat->get_text());
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

			QRect rect(option.rect.left() + resource::get_delegate_left(), option.rect.top() + resource::get_row_height(), width, option.rect.height() - resource::get_row_height());
			painter->drawText(rect, flags, qtext);

			// Time
			const auto& app = main_window::get_app();
			const auto owner = entity->get_owner();
			const auto client = owner->get<client_component>();

			if (app.is_timestamps())
			{
				char buff[32];
				const auto time = entity->get<time_component>();
				const auto tm = localtime(&time->get_time());

				if (owner->has_owner())
				{
					const auto self = owner->get_owner();
					const auto self_client = self->get<client_component>();
					strftime(buff, 32, self_client->get_timestamp().c_str(), tm);
				}
				else
				{
					strftime(buff, 32, client->get_timestamp().c_str(), tm);
				}

				rect = QRect(option.rect.left(), option.rect.top() + resource::get_delegate_top(), option.rect.width() - resource::get_delegate_right(), option.rect.height() - resource::get_delegate_top());
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignTop | Qt::AlignRight, QString(buff));
			}

			// Rectangle
			QColor color;
			QString name;

			if (!entity->has<router_component>())
			{
				if (app.is_user_colors())
					color.setRgba(client->get_color());
				else
					color = option.palette.color(QPalette::WindowText);

				name = QString::fromStdString(client->get_name());
				if (!owner->has_owner())
				{
					if (client->is_troll())
						name += suffix::troll;
				}
			}
			else
			{
				color = option.palette.color(QPalette::WindowText);
				const auto router = entity->get<router_component>();
				name = QString::fromStdString(router->has_name() ? router->get_name() : router->get_address());
			}

			// Color
			painter->setBrush(color);
			painter->setPen(Qt::NoPen);

			rect = QRect(option.rect.left(), option.rect.top() + resource::get_delegate_bar_margin(), resource::get_delegate_bar_width(), option.rect.height() - resource::get_delegate_bar_margin() * 2);
			painter->drawRect(rect);

			// Name
			painter->setFont(m_name_font);
			painter->setPen(color);

			rect = QRect(option.rect.left() + resource::get_delegate_left(), option.rect.top() + resource::get_delegate_top(), option.rect.width() - resource::get_delegate_left(), option.rect.height() - resource::get_delegate_top());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignTop, name);

			painter->restore();
		}
	}
}
