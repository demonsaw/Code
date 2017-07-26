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
#include "component/search/search_component.h"
#include "component/status/status_component.h"
#include "delegate/search/search_weight_delegate.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	void search_weight_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left() + offset::text, option.rect.top(), option.rect.width() - offset::text, option.rect.height());
		editor->setGeometry(rect);
	}

	QSize search_weight_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		rect.setWidth(rect.width() + offset::text + offset::padding);

		return rect.size();
	}

	void search_weight_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			// Weight
			const auto search = entity->get<search_component>();
			const auto weight = QString("%1").arg(search->get_weight());

			variant = index.data(Qt::EditRole);
			const auto swarm_max = variant.toFloat();
			const auto ratio = ((search->get_swarm() * search->get_weight()) / swarm_max);

			auto color = option.palette.color(QPalette::WindowText);
			painter->setBrush(color);

			QColor outline = QColor(color.red(), color.green(), color.blue(), color.alpha() >> 1);
			painter->setPen(outline);

			const auto swarm_diameter = offset::circle * ratio;
			const auto left_padding = (option.rect.width() >> 1) - (swarm_diameter / 2.0);
			const auto top_padding = (offset::circle - swarm_diameter) / 2;
			QRect rect(option.rect.left() + left_padding, option.rect.top() + offset::top + top_padding, swarm_diameter, swarm_diameter);
			painter->drawEllipse(rect);
			
			// Font Awesome Version (not as awesome)
			/*QFont font(demonsaw::font_awesome);
			font.setPixelSize(offset::circle * ratio);
			painter->setFont(font);

			painter->drawText(option.rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa::circle));*/

			painter->restore();
		}
	}
}