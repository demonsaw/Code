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

#include "component/endpoint_component.h"
#include "delegate/endpoint_delegate.h"
#include "entity/entity.h"
#include "resource/resource.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	void endpoint_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left() + offset::text + m_indent, option.rect.top(), option.rect.width() - offset::text - m_indent, option.rect.height());
		editor->setGeometry(rect);
	}

	QSize endpoint_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize((option.decorationSize.width() >> 1) + rect.width() + offset::text + m_indent, offset::row);
	}

	void endpoint_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
			const auto endpoint = entity->find<endpoint_component>();

			if (endpoint->get_type() == endpoint_type::client)
			{
				QRect rect(option.rect.left() + offset::text + m_indent, option.rect.top() + offset::top, option.rect.width() - offset::text, option.rect.height() - offset::top);
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignTop, QString::fromStdString(endpoint->get_name()));

				// Color
				QColor color;
				color.setRgba(endpoint->get_color());
				painter->setBrush(color);

				QColor outline = QColor(color.red(), color.green(), color.blue(), color.alpha() >> 1);
				painter->setPen(outline);

				rect = QRect(option.rect.left() + offset::padding + m_indent + 1, option.rect.top() + offset::top, offset::circle, offset::circle);
				painter->drawEllipse(rect);
			}
			else
			{
				QRect rect(option.rect.left() + offset::text + m_indent, option.rect.top() + offset::top, option.rect.width() - offset::text, option.rect.height() - offset::top);
				painter->drawText(rect, Qt::TextSingleLine | Qt::AlignTop, QString::fromStdString(endpoint->has_name() ? endpoint->get_name() : endpoint->get_address()));

				// Logo
				QPixmap pixmap(resource::logo_rgb);
				rect = QRect(option.rect.left() + offset::padding + m_indent, option.rect.top() + offset::top - 1, 16, 16);
				painter->drawPixmap(rect, pixmap.scaled(QSize(16, 16), Qt::KeepAspectRatio, Qt::SmoothTransformation));
			}			

			painter->restore();
		}
	}
}
