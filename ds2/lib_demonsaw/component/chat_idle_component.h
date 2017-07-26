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

#ifndef _EJA_CHAT_IDLE_COMPONENT_
#define _EJA_CHAT_IDLE_COMPONENT_

#include <memory>
#include <queue>

#include "component/idle_component.h"
#include "entity/entity.h"
#include "http/http_socket.h"
#include "object/chat.h"
#include "system/type.h"

namespace eja
{
	struct chat_data
	{
		eja::entity::ptr entity;
		std::string message;
		chat_type type;
	};

	class chat_idle_component final : public idle_component
	{		
	public:
		using ptr = std::shared_ptr<chat_idle_component>;

	private:
		static const size_t s_delay = sec_to_ms(3); 
		http_socket::ptr m_socket = http_socket::create();
		mutex_queue<chat_data> m_queue;

		// Callback		
		virtual bool on_run() override;
		virtual bool on_stop() override;

	private:
		chat_idle_component() : idle_component(default_timeout::client::chat){ }

		// Utility
		void open();
		void close();

	public:
		virtual ~chat_idle_component() override { }

		// Interface
		virtual void init() override;
		virtual void shutdown() override;

		// Utility
		void add(const std::string& message, const entity::ptr entity = nullptr);
		void add(const entity::ptr entity, const std::string& message, const chat_type type);

		// Static
		static ptr create() { return ptr(new chat_idle_component()); }
	};
}

#endif
