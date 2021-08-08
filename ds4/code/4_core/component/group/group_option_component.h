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

#ifndef _EJA_GROUP_OPTION_COMPONENT_H_
#define _EJA_GROUP_OPTION_COMPONENT_H_

#include <string>

#include "component/security/security_component.h"
#include "system/type.h"

namespace eja
{
	class cipher;

	// Type
	enum class entropy_type { none, data, file, url };

	// Component
	class group_option_component final : public security_component
	{
		make_factory(group_option_component);

	private:		
		std::string m_entropy;
		std::string m_data;

		entropy_type m_type = entropy_type::none;
		double m_percent = 100.0;
		u64 m_size = 0;

	public:
		group_option_component();

		// Interface
		virtual void clear() override;

		// Utility
		std::shared_ptr<cipher> create_cipher() const;

		// Has
		bool has_data() const { return !m_data.empty(); }
		bool has_entropy() const { return !m_entropy.empty(); }		
		bool has_percent() const { return m_percent > 0.0; }
		bool has_size() const { return m_size > 0; }
		bool has_type() const { return m_type != entropy_type::none; }		

		// Set
		void set_entropy(const std::string& entropy) { m_entropy = entropy; }
		void set_percent(const double percent) { m_percent = percent; }
		void set_type(const entropy_type type) { m_type = type; }

		void set_data();
		void set_data(std::string&& data) { m_data = std::move(data); }
		void set_data(const std::string& data) { m_data = data; }		
		void set_size(const u64 size) { m_size = size; }

		// Get		
		const std::string& get_entropy() const { return m_entropy; }
		double get_percent() const { return m_percent; }
		entropy_type get_type() const { return m_type; }

		const std::string& get_data() const { return m_data; }
		u64 get_size() const { return m_size; }
	};
}

#endif
