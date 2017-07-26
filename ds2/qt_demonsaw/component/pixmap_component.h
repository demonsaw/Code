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

#ifndef _EJA_PIXMAP_COMPONENT_
#define _EJA_PIXMAP_COMPONENT_

#include <memory>
#include <string>

#include <QPixmap>
#include <QSize>

#include "component/component.h"

namespace eja
{
	class pixmap_component final : public component
	{
	private:
		QSize m_size;
		QPixmap m_pixmap;

	public:
		using ptr = std::shared_ptr<pixmap_component>;

	public:
		pixmap_component() : m_size(16, 16) { }
		pixmap_component(const QSize& size) : m_size(size) { }
		pixmap_component(const size_t width, const size_t height) : m_size(width, height) { }
		virtual ~pixmap_component() override { }

		// Utility
		bool valid() const { return !m_pixmap.isNull(); }
		bool invalid() const { return !valid(); }

		// Mutator
		void set_path(const std::string& path, const std::string& data);
		void set_path(const char* path, const char* data);
		
		void set_path(const std::string& path) { set_path(path.c_str()); }
		void set_path(const char* path);

		void set_path() { m_pixmap = QPixmap(); }

		void set_size(const QSize& size) { m_size = size; }
		void set_size(const size_t width, const size_t height);

		// Accessor
		const QPixmap& get_pixmap() const { return m_pixmap; }
		const QSize& get_size() const { return m_size; }
		const size_t get_width() const { return m_size.width(); }
		const size_t get_height() const { return m_size.height(); }

		// Static
		static ptr create() { return ptr(new pixmap_component()); }
		static ptr create(const QSize& size) { return ptr(new pixmap_component(size)); }
		static ptr create(const size_t width, const size_t height) { return ptr(new pixmap_component(width, height)); }
	};
}

#endif
