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

#ifndef _EJA_VERIFIED_OPTION_COMPONENT_H_
#define _EJA_VERIFIED_OPTION_COMPONENT_H_

#include <string>

#include "component/component.h"

#include "system/type.h"

namespace eja
{
	class verified_option_component final : public component
	{
		make_factory(verified_option_component);

	protected:
		std::string m_hash; 
		std::string m_key;
		std::string m_salt;
		
		size_t m_key_size = 0;
		size_t m_iterations = 0;

	public:
		verified_option_component();

		// Utility
		virtual bool valid() const override { return component::valid() && has_key() && has_hash() && has_salt() && has_key_size() && has_iterations(); }

		// Interface
		virtual void clear() override;

		// Has
		bool has_hash() const { return !m_hash.empty(); }
		bool has_iterations() const { return m_iterations > 0; }
		bool has_key() const { return !m_key.empty(); }
		bool has_key_size() const { return m_key_size > 0; }		
		bool has_salt() const { return !m_salt.empty(); }

		// Set
		void set_hash(const std::string& hash) { m_hash = hash; }
		void set_iterations(const size_t iterations) { m_iterations = iterations; }
		void set_key(const std::string& key) { m_key = key; }
		void set_key_size(const size_t key_size) { m_key_size = key_size; }		
		void set_salt(const std::string& salt) { m_salt = salt; }

		// Get
		const std::string& get_hash() const { return m_hash; }
		size_t get_iterations() const { return m_iterations; }
		const std::string& get_key() const { return m_key; }
		size_t get_key_size() const { return m_key_size; }		
		const std::string& get_salt() const { return m_salt; }
	};
}

#endif
