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

#ifndef _EJA_GROUP_SECURITY_COMPONENT_H_
#define _EJA_GROUP_SECURITY_COMPONENT_H_

#include <deque>
#include <map>
#include <string>
#include <vector>

#include "component/security/security_component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class cipher;

	// Type
	enum class group_security_type { none, data, file, url };

	make_thread_safe_component(group_security_list_component, std::vector<std::shared_ptr<entity>>);
	make_thread_safe_component(group_security_map_component, std::map<std::string, std::shared_ptr<entity>>);

	// Component
	class group_security_component final : public security_component
	{
		make_factory(group_security_component);

	private:		
		std::string m_entropy;
		group_security_type m_type = group_security_type::none;
		double m_percent = 100;

		std::string m_data;
		u64 m_size = 0;

	public:
		group_security_component();
		group_security_component(const group_security_component& comp) : security_component(comp), m_data(comp.m_data), m_entropy(comp.m_entropy), m_type(comp.m_type), m_percent(comp.m_percent), m_size(comp.m_size) { }

		// Operator
		group_security_component& operator=(const group_security_component& comp);

		// Interface
		virtual void clear() override;

		// Utility
		std::shared_ptr<cipher> create_cipher() const;

		// Has
		bool has_data() const { return !m_data.empty(); }
		bool has_entropy() const { return !m_entropy.empty(); }		
		bool has_percent() const { return m_percent > 0.0; }
		bool has_size() const { return m_size > 0; }
		bool has_type() const { return m_type != group_security_type::none; }		

		// Set
		void set_entropy(const std::string& entropy) { m_entropy = entropy; }
		void set_percent(const double percent) { m_percent = percent; }
		void set_type(const group_security_type type) { m_type = type; }

		void set_data();
		void set_data(std::string&& data) { m_data = std::move(data); }
		void set_data(const std::string& data) { m_data = data; }		
		void set_size(const u64 size) { m_size = size; }

		// Get		
		const std::string& get_entropy() const { return m_entropy; }
		double get_percent() const { return m_percent; }
		group_security_type get_type() const { return m_type; }

		const std::string& get_data() const { return m_data; }
		u64 get_size() const { return m_size; }
	};
}

#endif
