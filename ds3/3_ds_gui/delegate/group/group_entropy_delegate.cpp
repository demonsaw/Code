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

#include <QLineEdit>
#include <QPainter>

#include "component/pixmap_component.h"
#include "component/group/group_security_component.h"
#include "component/status/status_component.h"
#include "delegate/group/group_entropy_delegate.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility	
	void group_entropy_delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		if (QLineEdit* widget = qobject_cast<QLineEdit*>(editor))
		{
			QString currentText = index.data(Qt::EditRole).toString();
			widget->setText(currentText);
		}
		else
		{
			QStyledItemDelegate::setEditorData(editor, index);
		}
	}

	void group_entropy_delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		// Save the current text of the combo box as the current value of the item
		if (QLineEdit* widget = qobject_cast<QLineEdit*>(editor))
			model->setData(index, widget->text(), Qt::EditRole);
		else
			QStyledItemDelegate::setModelData(editor, model, index);
	}

	void group_entropy_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left() + offset::text, option.rect.top(), option.rect.width() - offset::text, option.rect.height());
		editor->setGeometry(rect);
	}

	QSize group_entropy_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		auto str = variant.toString();
			
		// Size
		QFontMetrics metrics(option.font);
		QRect rect = rect = metrics.boundingRect(!str.isEmpty() ? str : no_entropy);
		rect.setWidth((option.decorationSize.width() >> 1) + rect.width() + offset::text + offset::padding);

		return rect.size();
	}

	void group_entropy_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

			// Text
			const auto security = entity->get<group_security_component>();
			const auto& qentropy = security->has_entropy() ? QString::fromStdString(security->get_entropy()) : no_entropy;
			QRect rect(option.rect.left() + offset::text, option.rect.top(), option.rect.width() - offset::text, option.rect.height());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, qentropy);

			// Pixmap
			const auto pixmap = entity->get<pixmap_component>();			
			if (security->has_data())
			{
				pixmap->set_path(security->get_entropy(), security->get_data());
				security->set_data();
			}

			// Status			
			const auto status = entity->get<status_component>();

			if (status->is_success())
			{
				if (pixmap->valid())
				{
					rect = QRect(option.rect.left() + offset::status, option.rect.top() + offset::top, 16, 16);
					painter->drawPixmap(rect, pixmap->get_pixmap());
				}
				else
				{
					const auto& type = security->get_type();
					const auto fa = (type == group_security_type::data) ? fa::key : fa::question;

					QFont font(demonsaw::font_awesome);
					font.setPixelSize(14);
					painter->setFont(font);

					rect = QRect(option.rect.left() + offset::status + 1, option.rect.top(), 14, option.rect.height());
					painter->drawText(rect, Qt::TextSingleLine | Qt::AlignHCenter | Qt::AlignVCenter, QString(fa));
				}
			}
			else
			{
				const auto color = QColor(status->get_color());
				painter->setBrush(color);

				QColor outline = QColor(color.red(), color.green(), color.blue(), color.alpha() >> 1);
				painter->setPen(outline);

				rect = QRect(option.rect.left() + offset::status + 1, option.rect.top() + offset::top + 1, offset::circle, offset::circle);
				painter->drawEllipse(rect);
			}
						
			painter->restore();
		}
	}
}
