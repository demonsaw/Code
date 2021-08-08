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

#include <cassert>

#include "component/callback/callback.h"
#include "component/transfer/transfer_service_component.h"

#include "entity/entity.h"
#include "thread/thread_info.h"
#include "utility/value.h"

namespace eja
{
	// Utility	
	void transfer_service_component::start()
	{
		if (invalid() && set_state(state::starting, state::none))
		{
			service_component::start();

			// Commands
			async_timeout();

			set_state(state::running);
		}
	}

	void transfer_service_component::restart()
	{
		if (valid() && set_state(state::restarting, state::running))
		{
			service_component::stop();

			set_state(state::none);

			start();
		}
	}

	void transfer_service_component::stop()
	{
		if (valid() && set_state(state::stopping, state::running))
		{
			service_component::stop();

			set_state(state::none);
		}
	}

	// Timeout
	void transfer_service_component::async_timeout()
	{
		const auto self = shared_from_this();
		wait(transfer::queue, [self]() { self->timeout(); });
	}

	void transfer_service_component::timeout()
	{
		try
		{
			// Queue
			call(m_action | callback::update);
		}
		catch (const std::exception& ex)
		{
			const auto owner = get_owner();
			set_status(owner, eja::status::error);

			log(ex);
		}

		// Repeat
		async_timeout();
	}

	// Set
	void transfer_service_component::set_status(const entity::ptr& entity, const status value) const
	{
		service_component::set_status(entity, value);

		// Callback
		const auto owner = get_owner();
		async_call(m_action | callback::update, entity);
	}
}
