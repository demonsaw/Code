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

#ifndef _EJA_CALLBACK_SERVICE_COMPONENT_H_
#define _EJA_CALLBACK_SERVICE_COMPONENT_H_

#include <map>

#include "component/service/service_component.h"
#include "entity/entity_callback.h"
#include "object/function.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class http_request;
	class message_packer;

	class callback_service_component final : public service_component
	{
		make_factory(callback_service_component);

	private:
		function_map m_map;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<callback_service_component>(); }
		const ptr shared_from_this() const { return std::const_pointer_cast<callback_service_component>(shared_from_base<callback_service_component>()); }

		// Quit
		void quit(const std::string& address, const u16 port, const std::shared_ptr<http_request> request);

	public:
		callback_service_component() { }
		callback_service_component(const size_t threads) : service_component(threads) { }

		// Interface
		virtual void clear() override;

		// Utility
		virtual void start() override;
		virtual void restart() override;
		virtual void stop() override;

		// Call
		template <typename ... T>
		void async_call(const size_t key, const T& ... args) const;
		void async_call(const size_t key) const;

		// Call
		template <typename ... T>
		void call(const size_t key, const T& ... args) const;
		void call(const size_t key) const;

		// Quit
		void async_quit(const std::string& address, const u16 port, const std::shared_ptr<http_request> request);

		// Add
		template <typename T>
		function::ptr add(const size_t key, const T& t);
		void add(const size_t key, const function::ptr& func);

		// Remove
		void remove(const size_t key);
		void remove(const size_t key, const function::ptr& func);
		void remove(const function::ptr& func);

		// Create
		static ptr create(const size_t threads) { return std::make_shared<callback_service_component>(threads); }
	};

	// Command
	template <typename ... T>
	void callback_service_component::async_call(const size_t key, const T& ... args) const
	{
		const auto self = shared_from_this();
		post([self, key, args...]()
		{
			self->call(key, args...);
		});
	}

	template <typename ... T>
	void callback_service_component::call(const size_t key, const T& ... args) const
	{
		thread_lock(m_map);

		const auto range = m_map.equal_range(key);
		//assert(range.first != range.second);

		for (auto it = range.first; it != range.second; ++it)
		{
			const auto& func = it->second;
			func->call(args...);
		}
	}

	// Add
	template <typename T>
	function::ptr callback_service_component::add(const size_t key, const T& t)
	{
		const auto func = function::create(t);
		add(key, func);

		return func;
	}
}

#endif
