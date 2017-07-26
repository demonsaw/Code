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

#ifndef _EJA_CONFIG_RUNTIME_
#define _EJA_CONFIG_RUNTIME_

#include "controller/console_controller.h"
#include "controller/router_controller.h"
#include "xml/config_file.h"

namespace eja
{
	class runtime_controller final : public console_controller
	{
	private:
		config_file m_config;

	private:
		// Callback
		void on_transfer_callback(const function_action action, const entity::ptr entity);
		void on_transfer_add(const entity::ptr entity);
		void on_transfer_remove(const entity::ptr entity);

		void on_upload_callback(const function_action action, const entity::ptr entity);
		void on_upload_add(const entity::ptr entity);
		void on_upload_remove(const entity::ptr entity);

	public:
		runtime_controller() { }
		virtual ~runtime_controller() override { }

		// Interface		
		virtual void init() override;
		virtual void shutdown() override;

		// Utility
		void restart();
		void refresh();

		bool load(const std::string& path) { return load(path.c_str()); }
		bool load(const char* path);

		void routers();
		void clients();
		void transfers();
	};
}

#endif
