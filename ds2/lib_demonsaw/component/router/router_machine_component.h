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

#ifndef _EJA_ROUTER_MACHINE_COMPONENT_
#define _EJA_ROUTER_MACHINE_COMPONENT_

#include <memory>

#include "object/status.h"
#include "component/service/acceptor_service_component.h"
#include "system/type.h"
#include "system/function/function.h"
#include "system/function/function_type.h"

namespace eja
{
	class router_machine_component final : public acceptor_service_component, public std::enable_shared_from_this<router_machine_component>
	{
		make_factory(router_machine_component);

	private:
		function_list m_functions; 
		status m_status = status::none;

	private:
		// Utility
		void main();

		// Callback
		void on_main();
		void on_accept(const io_service_ptr service, const std::shared_ptr<http_socket> socket);

	public:
		router_machine_component();
		router_machine_component(const router_machine_component& comp) : acceptor_service_component(comp), m_status(comp.m_status) { }
		router_machine_component(const size_t threads) : acceptor_service_component(threads) { }

		// Operator
		router_machine_component& operator=(const router_machine_component& comp);

		// Interface
		virtual void clear() override;
		virtual void shutdown() override;

		virtual void start() override;
		virtual void stop() override;

		// Utility
		void add(const function::ptr ptr) { m_functions.add(ptr); }
		void remove(const function::ptr ptr) { m_functions.remove(ptr); }

		// Status
		bool none() const { return m_status == status::none; }
		bool success() const { return m_status == status::success; }
		bool warning() const { return m_status == status::warning; }
		bool error() const { return m_status == status::error; }
		bool info() const { return m_status == status::info; }
		bool pending() const { return m_status == status::pending; }
		bool cancelled() const { return m_status == status::cancelled; }
		bool unknown() const { return m_status == status::unknown; }

		// Set
		void set_status(const eja::status status);

		// Get
		function_list& get_functions() { return m_functions; }
		const status& get_status() const { return m_status; }
	};
}

#endif
