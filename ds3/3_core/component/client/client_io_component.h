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

#ifndef _EJA_CLIENT_IO_COMPONENT_H_
#define _EJA_CLIENT_IO_COMPONENT_H_

#include <string>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class client_io_component : public component
	{
		make_factory(client_io_component);

	private:
		// Network
		std::string m_path;		
		std::string m_hash;
		std::string m_salt;
		bool m_remove = true;

	public:
		client_io_component();
		client_io_component(const client_io_component& comp) : component(comp),
			m_path(comp.m_path), m_hash(comp.m_hash), m_salt(comp.m_salt), m_remove(comp.m_remove) { }

		// Operator
		client_io_component& operator=(const client_io_component& comp);

		// Interface
		virtual void clear() override;

		// Has		
		bool has_path() const { return !m_path.empty(); }				
		bool has_hash() const { return !m_hash.empty(); }
		bool has_salt() const { return !m_salt.empty(); }
		bool has_remove() const { return m_remove; }

		// Set		
		void set_path(const std::string& path) { m_path = path; }		
		void set_hash(const std::string& hash) { m_hash = hash; }
		void set_salt(const std::string& salt) { m_salt = salt; }
		void set_remove(const bool remove) { m_remove = remove; }

		// Get
		const std::string& get_path() const { return m_path; }
		const std::string& get_hash() const { return m_hash; }
		const std::string& get_salt() const { return m_salt; }
	};
}

#endif
