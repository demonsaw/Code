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

#ifndef _EJA_TRANSFER_SERVICE_COMPONENT_H_
#define _EJA_TRANSFER_SERVICE_COMPONENT_H_

#include <memory>

#include "component/callback/callback.h"
#include "component/service/service_component.h"
#include "message/message_packer.h"
#include "object/transfer.h"
#include "system/type.h"

namespace eja
{
	class entity;

	class transfer_service_component : public service_component
	{
	protected:
		size_t m_action = callback::none;

	public:
		using ptr = std::shared_ptr<transfer_service_component>;

	protected:
		transfer_service_component(const size_t action) : transfer_service_component(action, transfer::num_threads) { }
		transfer_service_component(const size_t action, const size_t threads) : service_component(threads), m_action(action) { }

		// Interface
		ptr shared_from_this() { return shared_from_base<transfer_service_component>(); }
		const ptr shared_from_this() const { return std::const_pointer_cast<transfer_service_component>(shared_from_base<transfer_service_component>()); }

		// Commands
		void async_timeout();
		void timeout();
		
		// Set
		virtual void set_status(const std::shared_ptr<entity>& entity, const status value) const override;

	public:
		// Utility
		virtual void start() override;
		virtual void restart() override;
		virtual void stop() override;
	};
}

#endif
