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

#include "pixmap_component.h"
#include "resource/icon_provider.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Interface
	void pixmap_component::set_path(const std::string& path, const std::string& data)
	{
		QImage image;
		if (image.loadFromData(reinterpret_cast<const byte*>(data.c_str()), data.size()))
			m_pixmap = QPixmap::fromImage(image.scaled(m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}

	void pixmap_component::set_path(const char* path, const char* data)
	{
		if (!m_pixmap.loadFromData(reinterpret_cast<const byte*>(data), strlen(data)))
		{
			const auto icon = icon_provider::get_file(path);
			m_pixmap = icon.pixmap(m_size);
		}
	}

	void pixmap_component::set_path(const char* path)
	{
		if (file_util::exists(path))
		{
			QImage image;
			if (image.load(path))
			{
				m_pixmap = QPixmap::fromImage(image.scaled(m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
				return;
			}
		}

		const auto icon = icon_provider::get_file(path);
		m_pixmap = icon.pixmap(m_size);
	}

	// Mutator
	void pixmap_component::set_size(const size_t width, const size_t height)
	{ 
		m_size.setWidth(width);
		m_size.setHeight(height);
	}
}
