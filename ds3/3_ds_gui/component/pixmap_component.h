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

#ifndef _EJA_PIXMAP_COMPONENT_H_
#define _EJA_PIXMAP_COMPONENT_H_

#include <memory>
#include <string>

#include <QPixmap>
#include <QSize>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class pixmap_component final : public component
	{
		make_factory(pixmap_component);

	private:
		QSize m_size;
		QPixmap m_pixmap;

	public:
		pixmap_component() : m_size(16, 16) { }
		pixmap_component(const pixmap_component& comp) : component(comp), m_size(comp.m_size), m_pixmap(comp.m_pixmap) { }
		pixmap_component(const QSize& size) : m_size(size) { }
		pixmap_component(const size_t width, const size_t height) : m_size(width, height) { }
		pixmap_component(const size_t radius) : m_size(radius, radius) { }

		// Operator
		pixmap_component& operator=(const pixmap_component& comp);

		// Interface
		virtual void clear() override { set_path(); }

		// Utility
		virtual bool valid() const override { return has_pixmap(); }

		// Has
		bool has_pixmap() const { return !m_pixmap.isNull(); }

		// Set
		void set_path(const std::string& path, const std::string& data);
		void set_path(const char* path, const char* data);
		
		void set_path(const std::string& path) { set_path(path.c_str()); }
		void set_path(const char* path);

		void set_path() { m_pixmap = QPixmap(); }

		void set_size(const QSize& size) { m_size = size; }
		void set_size(const size_t width, const size_t height);

		// Get
		const QPixmap& get_pixmap() const { return m_pixmap; }
		const QSize& get_size() const { return m_size; }
		size_t get_width() const { return m_size.width(); }
		size_t get_height() const { return m_size.height(); }

		// Static
		static ptr create(const QSize& size) { return ptr(new pixmap_component(size)); }
		static ptr create(const size_t width, const size_t height) { return ptr(new pixmap_component(width, height)); }
		static ptr create(const size_t radius) { return ptr(new pixmap_component(radius)); }
	};
}

#endif
