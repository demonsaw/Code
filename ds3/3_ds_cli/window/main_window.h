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

#ifndef _EJA_MAIN_WINDOW_H_
#define _EJA_MAIN_WINDOW_H_

#include <memory>

#include "config/application.h"
#include "system/type.h"

namespace eja
{
	class main_window final
	{
	private:
		bool m_active;
		application& m_app;

	public:
		using ptr = std::shared_ptr<main_window>;

	private:
		// Utility
		void credits();
		void menu();		
		void splash();

		void print(const char ch, const size_t num);
		void space(const size_t num);
		void dash(const size_t num);
		void endl();

		// Commands		
		void restart();
		void routers();

	public:
		main_window(const application& app) : m_app(const_cast<application&>(app)), m_active(true) { }

		// Interface
		void init();
		void update();
		void shutdown();
		void clear();

		// Utility
		bool active() const { return m_active; }
		bool inactive() const { return !active(); }

		// Get
		const application& get_application() const { return m_app; }
		application& get_application() { return m_app; }

		// Static
		static ptr create(const application& app) { return ptr(new main_window(app)); }
		static ptr create(const application::ptr& app) { return ptr(new main_window(*app)); }
	};
}

#endif
