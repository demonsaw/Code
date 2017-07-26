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

#ifndef _EJA_COLOR_
#define _EJA_COLOR_

#include <memory>

#include "object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	class color : public object
	{
	protected:
		u8 m_red = 0;
		u8 m_green = 0;
		u8 m_blue = 0;

	public:
		using ptr = std::shared_ptr<color>;

	public:
		color() { }
		color(const eja::color& color) : object(color), m_red(color.m_red), m_green(color.m_green), m_blue(color.m_blue) { }
		color(const std::string& id) : object(id) { }
		color(const char* id) : object(id) { }
		color(const u8 red, const u8 green, const u8 blue) : m_red(red), m_green(green), m_blue(blue) { }
		virtual ~color() override { }

		// Interface
		virtual void clear() override;

		// Mutator
		void set_red(const u8 red) { m_red = red; }
		void set_green(const u8 green) { m_green = green; }
		void set_blue(const u8 blue) { m_blue = blue; }

		// Accessor
		u8 get_red() const { return m_red; }
		u8 get_green() const { return m_green; }
		u8 get_blue() const { return m_blue; }

		// Static
		static ptr create() { return std::make_shared<color>(); }
		static ptr create(const eja::color& color) { return std::make_shared<eja::color>(color); }
		static ptr create(const color::ptr color) { return std::make_shared<eja::color>(*color); }
		static ptr create(const std::string& id) { return std::make_shared<color>(id); }
		static ptr create(const char* id) { return std::make_shared<color>(id); }
		static ptr create(const u8 red, const u8 green, const u8 blue) { return std::make_shared<color>(red, green, blue); }
	};

	// Container
	derived_type(color_list, mutex_list<color>);
	derived_type(color_map, mutex_map<std::string, color>);
	derived_type(color_queue, mutex_queue<color>);
	derived_type(color_vector, mutex_vector<color>);
}

#endif
