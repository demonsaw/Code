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

#include "config/cli_application.h"

namespace eja
{
	class entity;

	class main_window final
	{
	public:
		using ptr = main_window*;

	private:
		static main_window* s_ptr;

		cli_application m_app;
		bool m_active = true;

	private:
		// Utility
		void credits() const;
		void dash(const size_t num) const;
		void menu() const;
		void splash() const;

		// Commands
		void info() const;
		void restart();
		void transfers() const;

		// Finished
		void on_finished_add(const std::shared_ptr<entity>& owner, const std::shared_ptr<entity>& entity) const;

		// Upload
		void on_upload_add(const std::shared_ptr<entity>& owner, const std::shared_ptr<entity>& entity) const;
		void on_upload_remove(const std::shared_ptr<entity>& owner, const std::shared_ptr<entity>& entity) const;
		void on_upload_update(const std::shared_ptr<entity>& owner) const;

	public:
		main_window(int& argc, char* argv[]);

		// Interface
		void init();
		void update();
		void shutdown();

		// Utility
		bool active() const { return m_active; }
		bool inactive() const { return !active(); }

		// Static
		static ptr get() { return s_ptr; }
		static cli_application& get_app() { return get()->m_app; }
	};
}

#endif
