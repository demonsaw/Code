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

#ifndef _EJA_SESSIONT_COMPONENT_H_
#define _EJA_SESSIONT_COMPONENT_H_

#include <map>
#include <string>

#include "component/cipher/cipher_component.h"
#include "security/algorithm/diffie_hellman.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	make_thread_safe_component(session_map_component, std::map<std::string, std::shared_ptr<entity>>);

	class session_component final : public cipher_component
	{
		make_factory(session_component);

	private:
		diffie_hellman m_algorithm;
		std::string m_name;
		std::string m_password;

	public:
		session_component();
		session_component(const std::string& password);

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return cipher_component::valid() && has_id(); }

		void compute();
		void compute(const std::string& password);

		// Has
		bool has_name() const { return !m_name.empty(); }
		bool has_password() const { return !m_password.empty(); }

		// Set
		void set_name() { m_name.clear(); }
		void set_name(std::string&& name) { m_name = std::move(name); }
		void set_name(const std::string& name) { m_name = name; }

		void set_password() { m_password.clear(); }
		void set_password(std::string&& password) { m_password = std::move(password); }
		void set_password(const std::string& password) { m_password = password; }

		// Get
		const diffie_hellman& get_algorithm() const { return m_algorithm; }
		diffie_hellman& get_algorithm() { return m_algorithm; }

		const std::string& get_name() const { return m_name; }
		const std::string& get_password() const { return m_password; }

		// Static
		static ptr create(const std::string& password) { return std::make_shared<session_component>(password); }
	};
}

#endif
