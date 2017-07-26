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

#include "component/endpoint_component.h"
#include "component/status/status_component.h"
#include "delegate/endpoint_status_delegate.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	QSize endpoint_status_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize((option.decorationSize.width() / 2) + rect.width(), offset::row);
	}

	void endpoint_status_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
			const auto endpoing = entity->get<endpoint_component>();
			QRect rect(option.rect.left() + offset::text, option.rect.top(), option.rect.width() - offset::text, option.rect.height());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, QString::fromStdString(endpoing->get_name()));

			// Color
			const auto status = entity->get<status_component>();
			const auto color = QColor(status->get_color());
			painter->setBrush(color);

			QColor outline = QColor(color.red(), color.green(), color.blue(), color.alpha() >> 1);
			painter->setPen(outline);

			rect = QRect(option.rect.left() + offset::left, option.rect.top() + offset::top, offset::circle, offset::circle);
			painter->drawEllipse(rect);

			painter->restore();
		}
	}
}