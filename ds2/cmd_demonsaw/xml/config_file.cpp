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

#include "config_file.h"
#include "component/client/client_option_component.h"
#include "component/error/error_component.h"
#include "component/server/server_component.h"
#include "entity/entity.h"
#include "system/mutex/mutex.h"

namespace eja
{
	// Interface
	bool config_file::read(const char* path)
	{
		const auto status = xml_config_file::read(path);

		if (status)
		{
			{
				// Router
				auto_lock_ref(m_routers);

				for (const auto& entity : m_routers)
				{
					entity->add<error_component>();
				}
			}

			{
				// Client
				auto_lock_ref(m_clients);

				for (const auto& entity : m_clients)
				{
					entity->add<error_component>();

					const auto option = entity->get<client_option_component>();
					if (option)
					{
						option->set_chat(false);
						option->set_message(false);
					}
				}					
			}
		}

		return status;
	}
}
