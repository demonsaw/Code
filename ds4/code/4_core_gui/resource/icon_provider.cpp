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
#include "component/status/status_component.h"
#include "entity/entity.h"
#include "resource/icon_provider.h"
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
	QIcon icon_provider::get_file(const boost::filesystem::path& path)
	{
#if _WIN32
		return get_file(path.wstring());
#else
		return get_file();
#endif
	}

	QIcon icon_provider::get_file(const std::wstring& path)
	{
#if _WIN32
		return !path.empty() ? get_file(QFileInfo(QString::fromStdWString(path))) : QIcon();
#else
	return get_file();
#endif
	}

	QIcon icon_provider::get_file(const wchar_t* path)
	{
#if _WIN32
		return path ? get_file(QFileInfo(QString::fromWCharArray(path))) : QIcon();
#else
		return get_file();
#endif
	}

	QIcon icon_provider::get_file(const std::string& path)
	{
#if _WIN32
		return !path.empty() ? get_file(QFileInfo(QString::fromStdString(path))) : QIcon();
#else
		return get_file();
#endif
	}

	QIcon icon_provider::get_file(const char* path)
	{
#if _WIN32
		return path ? get_file(QFileInfo(QString(path))) : QIcon();
#else
		return get_file();
#endif
	}

	QIcon icon_provider::get_file(const QFileInfo& info)
	{
#if _WIN32
		if (info.suffix().isEmpty())
			return s_provider.icon(QFileIconProvider::File);

		if ((info.suffix() == "exe" && info.exists()))
			return s_provider.icon(info);

		QPixmap pixmap;

		if (!s_cache.find(info.suffix(), &pixmap))
		{
			// Support for nonexistent file type icons, will re-implement it as custom icon provider later
			// We don't use the variable, but by storing it statically, we ensure CoInitialize is only called once.
			static HRESULT comInit = ::CoInitialize(NULL);
			Q_UNUSED(comInit);

			SHFILEINFOA shFileInfo;
			const auto ext = "." + info.suffix();
			const auto result = ::SHGetFileInfoA(ext.toUtf8().constData(), 0, &shFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);

			// Even if GetFileInfo returns a valid result, hIcon can be empty in some cases
			if (result)
			{
				if (shFileInfo.hIcon)
				{
					pixmap = qt_pixmapFromWinHICON(shFileInfo.hIcon);
					::DestroyIcon(shFileInfo.hIcon);

					if (!pixmap.isNull())
						s_cache.insert(info.suffix(), pixmap);
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
		return get_file();
#endif
	}

	// Icon
	QIcon icon_provider::get_icon(const size_t diameter, const int fa, const int red, const int green, const int blue)
	{
		QPixmap pixmap(diameter, diameter);
		pixmap.fill(Qt::transparent);

		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing, true);

		QFont font(software::font_awesome);
		font.setPixelSize(diameter);
		painter.setFont(font);

		QColor color(red, green, blue);
		painter.setPen(color);

		painter.drawText(0, 0, diameter, diameter, Qt::AlignHCenter | Qt::AlignVCenter, QString(fa));

		return QIcon(pixmap);
	}

	QIcon icon_provider::get_icon(const size_t diameter, const int fa, const u32 rgba)
	{
		QColor color;
		color.setRgba(rgba);

		return get_icon(diameter, fa, color);
	}

	QIcon icon_provider::get_icon(const char* path, const size_t diameter, const entity::ptr& entity)
	{
		QPixmap target(diameter, diameter);
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

		QRect rect(0, 0, diameter, diameter);
		painter.drawEllipse(rect);

		QIcon icon(path);
		const auto source = icon.pixmap(diameter, diameter);
		painter.drawPixmap(0, 0, source);

		return QIcon(target);
	}

	QIcon icon_provider::get_icon(const char* path, const size_t diameter, const int red, const int green, const int blue)
	{
		QPixmap target(diameter, diameter);
		target.fill(Qt::transparent);

		// Circle
		QColor color(red, green, blue);
		QBrush brush(color);

		QPainter painter(&target);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(brush);
		painter.setPen(Qt::NoPen);

		QRect rect(0 , 0, diameter, diameter);
		painter.drawEllipse(rect);

		QIcon icon(path);
		const auto source = icon.pixmap(diameter, diameter);
		painter.drawPixmap(0, 0, source);

		return QIcon(target);
	}

	QIcon icon_provider::get_icon(const char* path, const size_t diameter, const u32 rgba)
	{
		QColor color;
		color.setRgba(rgba);

		return get_icon(path, diameter, color);
	}

	// Circle
	QIcon icon_provider::get_circle(const size_t diameter, const int red, const int green, const int blue)
	{
		const auto padding = 4;
		QPixmap target(diameter + padding, diameter + padding);
		target.fill(Qt::transparent);

		// Circle
		QColor color(red, green, blue);
		QBrush brush(color);

		QPainter painter(&target);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(brush);
		painter.setPen(Qt::NoPen);

		QRect rect(0, padding, diameter, diameter);
		painter.drawEllipse(rect);

		return QIcon(target);
	}

	QIcon icon_provider::get_circle(const size_t diameter, const u32 rgba)
	{
		QColor color;
		color.setRgba(rgba);

		return get_circle(diameter, color);
	}

	// Pixmap
	QPixmap icon_provider::get_pixmap(const size_t diameter, const int fa, const int red, const int green, const int blue)
	{
		QPixmap pixmap(diameter, diameter);
		pixmap.fill(Qt::transparent);

		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing, true);

		QFont font(software::font_awesome);
		font.setPixelSize(diameter);
		painter.setFont(font);

		QColor color(red, green, blue);
		painter.setPen(color);

		painter.drawText(0, 0, diameter, diameter, Qt::AlignHCenter | Qt::AlignVCenter, QString(fa));

		return pixmap;
	}

	QPixmap icon_provider::get_pixmap(const size_t diameter, const int fa, const u32 rgba)
	{
		QColor color;
		color.setRgba(rgba);

		return get_pixmap(diameter, fa, color);
	}

	QPixmap icon_provider::get_pixmap(const char* path, const size_t diameter, const int red, const int green, const int blue)
	{
		QPixmap pixmap(diameter, diameter);
		pixmap.fill(Qt::transparent);

		// Circle
		QColor color(red, green, blue);
		QBrush brush(color);

		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(brush);
		painter.setPen(Qt::NoPen);

		// NOTE: Leave a 1 pixel border for the mask
		QRect rect(1, 1, diameter - 2, diameter - 2);
		painter.drawEllipse(rect);

		QIcon icon(path);
		const auto source = icon.pixmap(diameter, diameter);
		painter.drawPixmap(0, 0, source);

		return pixmap;
	}

	QPixmap icon_provider::get_pixmap(const char* path, const size_t diameter, const u32 rgba)
	{
		QColor color;
		color.setRgba(rgba);

		return get_pixmap(path, diameter, color);
	}
}
