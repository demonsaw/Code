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

#ifndef _EJA_SPAM_COMPONENT_H_
#define _EJA_SPAM_COMPONENT_H_

#include <list>
#include <map>

#include "component/component.h"
#include "component/time/timeout_component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

/*
* Router
	- Restrict clients to 2.5.1
	-> SHA-512 teh MAC and send to router (is this already there? or do I have to sent)
	-> remove chat delay from 3 sec to 500 ms/1sec, 3 chats within 5 sec is 15 min ban
	-> 1) 3 max consecitive msg within 30 secs mac 15 mins
	-> 2) 3 identical msgs within 10 secs gets mac ban of 15 mins
	-> 3) 3 msgs within 5 secs gets mac ban of 15 mins
	-> 4) msgs are still sent to clients with same mac addr
	-> ioctl use mac, hash in memory on router (if these doesn't work then we can send hashed mac on client app)
*/

namespace eja
{
	class entity;

	make_thread_safe_component(spam_list_component, std::list<std::shared_ptr<entity>>); 
	make_thread_safe_component(spam_map_component, std::map<std::string, std::shared_ptr<entity>>);

	class spam_component final : public component
	{
		make_factory(spam_component);

	private:
		using timeout_queue = thread_safe<std::list<timeout_component::ptr>>;

		const static size_t max_items = 3;
		const static size_t request_timeout = sec_to_ms(3);
		const static size_t dupe_timeout = sec_to_ms(30);
		const static size_t max_timeout = sec_to_ms(30);

		size_t m_checksum;
		timeout_queue m_request;
		timeout_queue m_dupe;
		timeout_queue m_max;

	public:
		spam_component() : m_checksum(0) { }
		spam_component(const spam_component& comp) : m_checksum(comp.m_checksum), m_request(comp.m_request), m_dupe(comp.m_dupe), m_max(comp.m_max) { }

		// Operator
		spam_component& operator=(const spam_component& comp);

		// Interface
		virtual void update() override;		
		virtual void clear() override;

		// Utility
		void request();
		void chat(const char* psz);

		virtual bool valid() const override { return (m_request->size() < max_items) && (m_dupe->size() < max_items) && (m_max->size() < max_items); }
	};
}

#endif
