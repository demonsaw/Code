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

#ifndef _EJA_CLIENT_IDLE_COMPONENT_
#define _EJA_CLIENT_IDLE_COMPONENT_

#include <memory>

#include "component/idle_component.h"
#include "http/http_socket.h"
#include "utility/default_value.h"

namespace eja
{
	class client_idle_component final : public idle_component
	{
	private:
		http_socket::ptr m_socket = http_socket::create();

	public:
		using ptr = std::shared_ptr<client_idle_component>;

	private:
		client_idle_component() : idle_component(default_timeout::client::num_ping) { }
		client_idle_component(const size_t delay) : idle_component(delay) { }

		// Callback
		virtual bool on_run() override;
		virtual bool on_stop() override;
		void on_ping();

		// Utility	
		void close();

	public:
		virtual ~client_idle_component() override { close(); }

		// Interface
		virtual void shutdown() override;		

		// Static
		static ptr create() { return ptr(new client_idle_component()); }
		static ptr create(const size_t delay) { return ptr(new client_idle_component(delay)); }
	};
}

#endif
