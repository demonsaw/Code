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
#include <QBrush>
#include <QPainter>
#include <QPalette>
#include <QPixmap>

#include "component/endpoint_component.h"
#include "component/notify_component.h"
#include "component/status/status_component.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "system/api.h"
#include "utility/value.h"

#if _WIN32
// Should be in QtWinExtras soon, but for now let's import it manually
extern QPixmap qt_pixmapFromWinHICON(HICON icon);
#endif

namespace eja
{
	// Static
	QPixmapCache icon_provider::s_cache;
	QFileIconProvider icon_provider::s_provider;

	// Get
	QIcon icon_provider::get_file(const std::string& path)
	{
		if (path.empty())
			return QIcon();

#if _WIN32
		QFileInfo fileInfo(QString::fromStdString(path));
		if (fileInfo.suffix().isEmpty())
			return s_provider.icon(QFileIconProvider::File);

		if ((fileInfo.suffix() == "exe" && fileInfo.exists()))
			return s_provider.icon(fileInfo);

		QPixmap pixmap;

		if (!s_cache.find(fileInfo.suffix(), &pixmap))
		{
			// Support for nonexistent file type icons, will re-implement it as custom icon provider later
			// We don't use the variable, but by storing it statically, we ensure CoInitialize is only called once.
			static HRESULT comInit = ::CoInitialize(NULL);
			Q_UNUSED(comInit);

			SHFILEINFOA shFileInfo;
			const auto ext = "." + fileInfo.suffix();
			const auto result = ::SHGetFileInfoA(ext.toUtf8().constData(), 0, &shFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);

			// Even if GetFileInfo returns a valid result, hIcon can be empty in some cases
			if (result)
			{
				if (shFileInfo.hIcon)
				{
					pixmap = qt_pixmapFromWinHICON(shFileInfo.hIcon);
					::DestroyIcon(shFileInfo.hIcon);

					if (!pixmap.isNull())
						s_cache.insert(fileInfo.suffix(), pixmap);
				}
				else
				{
					return s_provider.icon(QFileIconProvider::File);
				}
			}
			else
			{
				return s_provider.icon(QFileIconProvider::File);
			}
		}

		return QIcon(pixmap);
#else
		// Default icon for Linux and Mac OS X
		return s_provider.icon(QFileIconProvider::File);
#endif
	}

	// Logo
	QIcon icon_provider::get_logo(const size_t diameter, const entity::ptr entity)
	{		
		const auto padding = 32;
		QPixmap target(diameter + padding, diameter);
		target.fill(Qt::transparent);

		// Circle
		QColor color;
		const auto status = entity->get<status_component>();
		color.setRgba(status->get_color());
		QBrush brush(color);

		QPainter painter(&target);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(brush);
		painter.setPen(Qt::NoPen);

		QRect rect(0 + (padding / 2), 0, diameter, diameter);
		painter.drawEllipse(rect);

		QIcon icon(resource::logo_gray);
		const auto source = icon.pixmap(diameter, diameter);
		painter.drawPixmap((padding / 2), 0, source);

		// Notify
		const auto notify = entity->get<notify_component>();

		if (!notify->empty())
		{
			QFont font(demonsaw::font_awesome);
			font.setPixelSize(14);
			painter.setFont(font);

			const auto endpoint = entity->get<endpoint_component>();
			painter.setPen(endpoint->get_color());

			if (notify->has_chat())
				painter.drawText(0, 0, diameter + padding, diameter, Qt::AlignLeft | Qt::AlignTop, QString(fa::commento));

			if (notify->has_pm())
				painter.drawText(0, 0, diameter + padding, diameter, Qt::AlignRight | Qt::AlignTop, QString(fa::envelopeo));
		}

		return QIcon(target);
	}

	QIcon icon_provider::get_logo(const size_t diameter, const int red, const int green, const int blue)
	{
		const auto padding = 32;
		QPixmap target(diameter + padding, diameter);
		target.fill(Qt::transparent);

		// Circle
		QColor color(red, green, blue);
		QBrush brush(color);

		QPainter painter(&target);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(brush);
		painter.setPen(Qt::NoPen);

		QRect rect(0 + (padding / 2), 0, diameter, diameter);
		painter.drawEllipse(rect);

		QIcon icon(resource::logo_gray);
		const auto source = icon.pixmap(diameter, diameter);
		painter.drawPixmap((padding / 2), 0, source);

		return QIcon(target);
	}

	QIcon icon_provider::get_logo(const size_t diameter, const u32 rgba)
	{
		QColor color;
		color.setRgba(rgba);

		return get_logo(diameter, color);
	}

	// Icon
	QIcon icon_provider::get_icon(const size_t diameter, const int fa, const int red, const int green, const int blue)
	{
		assert(diameter <= 16);

		QPixmap pixmap(16, 16);
		pixmap.fill(Qt::transparent);

		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing, true);

		QFont font(demonsaw::font_awesome);
		font.setPixelSize(diameter);
		painter.setFont(font);
		
		QColor color(red, green, blue);
		painter.setPen(color);

		const auto offset = (16 - diameter) / 2;
		painter.drawText(offset, offset, diameter, diameter, Qt::AlignHCenter | Qt::AlignVCenter, QString(fa));

		return QIcon(pixmap);
	}

	QIcon icon_provider::get_icon(const size_t diameter, const int fa, const u32 rgba)
	{
		QColor color;
		color.setRgba(rgba);

		return get_icon(diameter, fa, color);
	}

	// Pixmap
	QPixmap icon_provider::get_pixmap(const size_t diameter, const int fa, const int red, const int green, const int blue)
	{
		assert(diameter <= 16);

		QPixmap pixmap(16, 16);
		pixmap.fill(Qt::transparent);

		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing, true);

		QFont font(demonsaw::font_awesome);
		font.setPixelSize(diameter);
		painter.setFont(font);
		
		QColor color(red, green, blue);
		painter.setPen(color);

		const auto offset = (16 - diameter) / 2;
		painter.drawText(offset, offset, diameter, diameter, Qt::AlignHCenter | Qt::AlignVCenter, QString(fa));

		return pixmap;
	}

	QPixmap icon_provider::get_pixmap(const size_t diameter, const int fa, const u32 rgba)
	{
		QColor color;
		color.setRgba(rgba);

		return get_pixmap(diameter, fa, color);
	}
}
 