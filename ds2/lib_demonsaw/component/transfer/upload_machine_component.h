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

#ifndef _EJA_UPLOAD_MACHINE_COMPONENT_
#define _EJA_UPLOAD_MACHINE_COMPONENT_

#include <memory>

#include "component/machine_component.h"
#include "http/http_socket.h"

namespace eja
{
	class upload_machine_component final : public machine_component, public std::enable_shared_from_this<upload_machine_component>
	{
	private:
		http_socket::ptr m_socket = http_socket::create();
		std::string m_data;

	public:
		using ptr = std::shared_ptr<upload_machine_component>;

	private:
		upload_machine_component() { }

		// Callback
		virtual bool on_none() override;
		virtual bool on_start() override;
		virtual bool on_restart() override;
		virtual bool on_stop() override;

		virtual bool on_handshake() override;
		virtual bool on_transfer() override;
		virtual bool on_upload() override;
		virtual bool on_quit() override;

		// Utility		
		void close();

	public:
		virtual ~upload_machine_component() override { close(); }

		// Interface
		virtual void init() override;
		virtual void shutdown() override;
		virtual void stop() override;

		// Mutator
		virtual void set_status(const eja::status status) override;

		// Static
		static ptr create() { return ptr(new upload_machine_component()); }
	};
}

#endif
