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

#include "component/spam_component.h"
#include "security/checksum/crc.h"
#include "utility/value.h"

namespace eja
{
	// Operator
	spam_component& spam_component::operator=(const spam_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_checksum = comp.m_checksum;
			m_request = comp.m_request;
			m_dupe = comp.m_dupe;
			m_max = comp.m_max;
		}

		return *this;
	}

	// Interface
	void spam_component::update()
	{
		component::update();

		// Input
		for (const auto& timeout : **m_request)
		{
			if (timeout->expired(request_timeout))
				m_request->pop_front();
		}

		// Dupe
		for (const auto& timeout : **m_dupe)
		{
			if (timeout->expired(dupe_timeout))
				m_dupe->pop_front();
		}

		// Max
		for (const auto& timeout : **m_max)
		{
			if (timeout->expired(max_timeout))
				m_max->pop_front();
		}
	}

	void spam_component::clear()
	{
		component::clear();

		m_checksum = 0;
		m_request->clear();
		m_dupe->clear();
		m_max->clear();
	}

	// Utility
	void spam_component::request()
	{
		const auto timeout = timeout_component::create();
		timeout->init();

		m_request->push_back(timeout);
	}

	void spam_component::chat(const char* psz)
	{
		if (strlen(psz) >= chat::max_text)
		{
			const auto timeout = timeout_component::create();
			timeout->init();

			m_max->push_back(timeout);
		}
		else
		{
			const auto checksum = crc32()(psz);

			if (checksum != m_checksum)
			{
				m_checksum = checksum;
				m_dupe->clear();
			}

			const auto timeout = timeout_component::create();
			timeout->init();

			m_dupe->push_back(timeout);
		}
	}
}
