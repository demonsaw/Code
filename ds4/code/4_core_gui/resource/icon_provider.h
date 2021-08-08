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

#ifndef _EJA_ICON_PROVIDER_H_
#define _EJA_ICON_PROVIDER_H_

#include <string>
#include <boost/filesystem.hpp>

#include <QColor>
#include <QFileIconProvider>
#include <QIcon>
#include <QPixmap>
#include <QPixmapCache>

#include "font/font_awesome.h"

class QFileInfo;

namespace eja
{
	class entity;

	class icon_provider final
	{
	private:
		static QPixmapCache s_cache;
		static QFileIconProvider s_provider;

	private:
		icon_provider() = delete;
		~icon_provider() = delete;

	public:
		static QIcon get_file() { return s_provider.icon(QFileIconProvider::File); }
		static QIcon get_file(const boost::filesystem::path& path);
		static QIcon get_file(const QFileInfo& file_info);

		static QIcon get_file(const std::wstring& path);
		static QIcon get_file(const wchar_t* path);
		
		static QIcon get_file(const std::string& path);
		static QIcon get_file(const char* path);

		static QIcon get_folder() { return s_provider.icon(QFileIconProvider::Folder); }
		static QIcon get_drive() { return s_provider.icon(QFileIconProvider::Drive); }

		static QIcon get_icon(const size_t diameter, const int fa, const QColor& color) { return get_icon(diameter, fa, color.red(), color.green(), color.blue()); }
		static QIcon get_icon(const size_t diameter, const int fa, const int red, const int green, const int blue);
		static QIcon get_icon(const size_t diameter, const int fa, const u32 rgba);
		static QIcon get_icon(const size_t diameter, const int fa) { return get_icon(diameter, fa, 72, 72, 72); }

		static QIcon get_icon(const char* path, const size_t diameter, const std::shared_ptr<entity>& entity);
		static QIcon get_icon(const char* path, const size_t diameter, const QColor& color) { return get_icon(path, diameter, color.red(), color.green(), color.blue()); }
		static QIcon get_icon(const char* path, const size_t diameter, const int red, const int green, const int blue);
		static QIcon get_icon(const char* path, const size_t diameter, const u32 rgba);
		static QIcon get_icon(const char* path, const size_t diameter) { return get_icon(path, diameter, 255, 255, 255); }

		static QPixmap get_pixmap(const size_t diameter, const int fa, const QColor& color) { return get_pixmap(diameter, fa, color.red(), color.green(), color.blue()); }
		static QPixmap get_pixmap(const size_t diameter, const int fa, const int red, const int green, const int blue);
		static QPixmap get_pixmap(const size_t diameter, const int fa, const u32 rgba);
		static QPixmap get_pixmap(const size_t diameter, const int fa) { return get_pixmap(diameter, fa, 72, 72, 72); }

		static QPixmap get_pixmap(const char* path, const size_t diameter, const QColor& color) { return get_pixmap(path, diameter, color.red(), color.green(), color.blue()); }
		static QPixmap get_pixmap(const char* path, const size_t diameter, const int red, const int green, const int blue);
		static QPixmap get_pixmap(const char* path, const size_t diameter, const u32 rgba);
		static QPixmap get_pixmap(const char* path, const size_t diameter) { return get_pixmap(path, diameter, 72, 72, 72); }

		static QIcon get_circle(const size_t diameter, const QColor& color) { return get_circle(diameter, color.red(), color.green(), color.blue()); }
		static QIcon get_circle(const size_t diameter, const int red, const int green, const int blue);
		static QIcon get_circle(const size_t diameter, const u32 rgba);
	};
}

#endif
