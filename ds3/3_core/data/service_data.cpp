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

#include "data/service_data.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	service_data::service_data()
	{
		m_retries = network::num_retries;
	}
	
	service_data::service_data(const service_data& data) : entity_type(data), m_type(data.m_type), m_action(data.m_action), m_retries(data.m_retries) 
	{ 
		m_retries = network::num_retries;
	}

	service_data::service_data(const service_type type) : m_type(type) 
	{ 
		m_retries = network::num_retries;
	}
	
	service_data::service_data(const service_type type, const service_action action) : m_type(type), m_action(action) 
	{
		m_retries = network::num_retries;
	}
	
	service_data::service_data(const service_type type, const service_action action, const std::shared_ptr<entity> entity) : entity_type(entity), m_type(type), m_action(action)
	{
		m_retries = network::num_retries;
	}
	
	service_data::service_data(const service_type type, const std::shared_ptr<entity> entity) : entity_type(entity), m_type(type)
	{ 
		m_retries = network::num_retries;
	}

	service_data::service_data(const service_action action) : m_action(action)
	{
		m_retries = network::num_retries;
	}

	service_data::service_data(const service_action action, const std::shared_ptr<entity> entity) : entity_type(entity), m_action(action)
	{
		m_retries = network::num_retries;
	}

	service_data::service_data(const std::shared_ptr<entity> entity) : entity_type(entity)
	{
		m_retries = network::num_retries;
	}

	// Operator
	service_data& service_data::operator=(const service_data& data)
	{
		if (this != &data)
		{
			entity_type::operator=(data);

			m_type = data.m_type;
			m_action = data.m_action;
			m_retries = data.m_retries;
		}

		return *this;
	}

	// Has
	bool service_data::has_retry(const bool update /*= false*/)
	{
		if (update)
		{
			if (m_retry < m_retries)
			{
				++m_retry;

				return true;
			}

			return false;
		}			

		return m_retry < m_retries; 
	}
}
