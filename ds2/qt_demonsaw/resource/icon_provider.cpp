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

#include <QPixmap>

#include "icon_provider.h"
#include "system/api.h"

#if _WIN32
// Should be in QtWinExtras soon, but for now let's import it manually
extern QPixmap qt_pixmapFromWinHICON(HICON icon);
#endif

namespace eja
{
	// Static
	QPixmapCache icon_provider::s_cache;
	QFileIconProvider icon_provider::s_provider;

	// Accessor
	QIcon icon_provider::get_file(const std::string& path)
	{
		if (path.empty())
			return QIcon();

		QFileInfo fileInfo(QString::fromStdString(path));

#if _WIN32
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
}
