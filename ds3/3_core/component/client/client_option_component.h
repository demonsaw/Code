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

#ifndef _EJA_CLIENT_OPTION_COMPONENT_H_
#define _EJA_CLIENT_OPTION_COMPONENT_H_

#include <string>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class client_option_component : public component
	{
		make_factory(client_option_component);

	private:
		std::string m_timestamp;
		bool m_beep = false;
		bool m_flash = false;
		bool m_vibrate = false;

	public:
		client_option_component();
		client_option_component(const client_option_component& comp) : component(comp), m_timestamp(comp.m_timestamp), m_beep(comp.m_beep), m_flash(comp.m_flash), m_vibrate(comp.m_vibrate) { }

		// Operator
		client_option_component& operator=(const client_option_component& comp);

		// Interface
		virtual void clear() override;

		// Has
		bool has_beep() const { return m_beep; }
		bool has_flash() const { return m_flash; }
		bool has_timestamp() const { return !m_timestamp.empty(); }		
		bool has_vibrate() const { return m_vibrate; }

		// Set
		void set_beep(const bool beep) { m_beep = beep; }
		void set_flash(const bool flash) { m_flash = flash; }		
		void set_timestamp(const std::string& timestamp) { m_timestamp = timestamp; }		
		void set_vibrate(const bool vibrate) { m_vibrate = vibrate; }

		// Get		
		const std::string& get_timestamp() const { return m_timestamp; }
	};
}

#endif
