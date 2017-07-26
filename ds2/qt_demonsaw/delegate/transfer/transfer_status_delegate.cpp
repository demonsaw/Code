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

#include <QIcon>
#include <QPainter>

#include "transfer_status_delegate.h"
#include "component/io/file_component.h"
#include "component/transfer/download_thread_component.h"
#include "component/transfer/upload_thread_component.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "window/window.h"

namespace eja
{
	// Utility
	void transfer_status_delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
	{
		QRect rect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
		editor->setGeometry(rect);
	}

	QSize transfer_status_delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		// Data
		auto variant = index.data(Qt::EditRole);
		QString str = variant.toString();

		// Size
		QFontMetrics metrics(option.font);
		QRect rect = metrics.boundingRect(str);
		return QSize(rect.width(), 0);
	}

	void transfer_status_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);

		if (option.state & QStyle::State_Enabled)
		{
			// Entity
			const auto variant = index.data(Qt::DisplayRole);
			const auto entity = variant.value<entity::ptr>();
			if (!entity)
				return;

			// Component
			auto status = eja::status::none;
			if (entity->has<download_thread_component>())
			{
				const auto thread = entity->get<download_thread_component>();
				if (thread)
					status = thread->get_status();
			}
			else if (entity->has<upload_thread_component>())
			{
				const auto thread = entity->get<upload_thread_component>();
				if (thread)
					status = thread->get_status();
			}

			// Status
			QPixmap pixmap;

			switch (status)
			{
				case eja::status::none:
				{
					pixmap = QPixmap(resource::status::none);
					break;
				}
				case eja::status::success:
				{
					pixmap = QPixmap(resource::status::success);
					break;
				}
				case eja::status::warning:
				{
					pixmap = QPixmap(resource::status::warning);
					break;
				}
				case eja::status::error:
				{
					pixmap = QPixmap(resource::status::error);
					break;
				}
				case eja::status::info:
				{
					pixmap = QPixmap(resource::status::info);
					break;
				}
				case eja::status::pending:
				{
					pixmap = QPixmap(resource::status::pending);
					break;
				}
				case eja::status::cancelled:
				{
					pixmap = QPixmap(resource::status::cancelled);
					break;
				}
				case eja::status::unknown:
				{
					pixmap = QPixmap(resource::status::unknown);
					break;
				}
				default:
				{
					assert(false);
				}
			}

			// Status
			painter->save();
			QRect rect(option.rect.left() + offset::left, option.rect.top() + offset::top + 1, 13, 13);
			painter->drawPixmap(rect, pixmap.scaled(QSize(13, 13), Qt::KeepAspectRatio));
			painter->restore();
		}
	}
}
