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

#ifndef _EJA_ROUTER_COMPONENT_H_
#define _EJA_ROUTER_COMPONENT_H_

#include <deque>
#include <memory>
#include <string>

#include "component/component.h"
#include "component/endpoint_component.h"

#include "entity/entity.h"
#include "object/router.h"
#include "system/type.h"

namespace eja
{
	class entity;

	class router_component final : public endpoint_component
	{
		make_factory(router_component);

	private:
		std::string m_password;

	public:
		router_component() { }

		// Operator
		bool operator==(const router_component& comp);
		bool operator!=(const router_component& comp) { return !operator==(comp); }

		// Has
		bool has_password() const { return !m_password.empty(); }

		// Set
		void set_password(const std::string& password) { m_password = password; }

		// Get
		const std::string& get_password() const { return m_password; }
	};

	class router_list_component final : public thread_safe_ex<component, std::deque<std::shared_ptr<entity>>>
	{
		make_factory(router_list_component);

	private:
		std::shared_ptr<entity> m_entity;

	public:
		router_list_component() { }

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return !empty(); }
		
		// Has
		bool has_entity() const { return m_entity != nullptr; }

		// Set		
		void set_entity(const std::shared_ptr<entity>& entity);
		void set_entity();

		// Get
		std::shared_ptr<entity> get_entity();
	};
}

#endif

