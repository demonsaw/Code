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
#include "component/client/client_service_component.h"
#include "component/group/group_option_component.h"
#include "component/status/status_component.h"

#include "delegate/group/group_entropy_delegate.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Static
	const QString group_entropy_delegate::s_placeholder = "Enter a passphrase, URL, or right-click to select a file";

	// Constructor
	group_entropy_delegate::group_entropy_delegate(QObject* parent /*= nullptr*/) : QStyledItemDelegate(parent)
	{
		m_circle_font = QFont(software::font_awesome);
		m_circle_font.setPixelSize(resource::get_circle_size());
	}

	// Editor	
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
		QRect rect(option.rect.left() + resource::get_row_height(), option.rect.top(), option.rect.width() - resource::get_row_height(), option.rect.height());
		editor->setGeometry(rect);
	}

	// Utility	
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
			const auto group_option = entity->get<group_option_component>();
			const auto& qentropy = group_option->has_entropy() ? QString::fromStdString(group_option->get_entropy()) : s_placeholder;
			QRect rect(option.rect.left() + resource::get_row_height() + resource::get_icon_padding(), option.rect.top(), option.rect.width() - resource::get_row_height() - resource::get_icon_padding(), option.rect.height());
			painter->drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, qentropy);

			// Status
			const auto owner = entity->get_owner();
			const auto client_service = owner->get<client_service_component>();

			if (client_service->valid())
			{
				// Pixmap
				const auto pixmap = entity->get<pixmap_component>();
				if (group_option->has_data())
				{
					pixmap->set_path(group_option->get_entropy(), group_option->get_data());
					group_option->set_data();
				}

				const auto status = entity->get<status_component>();

				if ((entity->enabled() && status->is_none()) || (entity->disabled() && !status->is_none()))
				{
					painter->setFont(m_circle_font);

					rect = QRect(option.rect.left(), option.rect.top(), resource::get_row_height(), option.rect.height());
					painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa::refresh));
				}
				else if (status->is_success())
				{
					switch (group_option->get_type())
					{
						case entropy_type::file:
						case entropy_type::url:
						{
							if (pixmap->valid())
							{
								rect = QRect(option.rect.left() + resource::get_icon_padding(), option.rect.top() + resource::get_icon_padding(), resource::get_icon_size(), resource::get_icon_size());
								painter->drawPixmap(rect, pixmap->get_pixmap());
							}

							break;
						}
						case entropy_type::data:
						{
							painter->setFont(m_circle_font);

							rect = QRect(option.rect.left(), option.rect.top(), resource::get_row_height(), option.rect.height());
							painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa::key));

							break;
						}
						default:
						{
							painter->setFont(m_circle_font);
							painter->setPen(QColor(color::blue));

							rect = QRect(option.rect.left(), option.rect.top(), resource::get_row_height(), option.rect.height());
							painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa::refresh));

							break;
						}
					}
				}
				else
				{
					painter->setFont(m_circle_font);
					painter->setPen(QColor(status->get_color()));

					rect = QRect(option.rect.left(), option.rect.top(), resource::get_row_height(), option.rect.height());
					painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString(fa::circle));
				}
			}
						
			painter->restore();
		}
	}

	QSize group_entropy_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		auto qstr = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		auto rect = metrics.boundingRect(!qstr.isEmpty() ? qstr : s_placeholder);
		rect.setWidth((option.decorationSize.width() >> 1) + rect.width() + resource::get_row_height() + resource::get_small_padding());

		return rect.size();
	}
}
