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

#include "command/troll_command.h"

#include "component/name_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"

#include "utility/utf.h"
#include "utility/value.h"

namespace eja
{
	// Add
	void troll_command::add(const entity::ptr& entity) const
	{
		// Client
		const auto client = entity->get<client_component>();
		auto display = utf::strip(client->get_name());
		display.erase(std::remove_if(display.begin(), display.end(), [&](const char value) { return !is_alpha_numeric(value); }), display.end());

		if (!display.empty())
		{
			boost::to_lower(display);
			client->set_display(std::move(display));
			client->set_troll(false);

			const auto group_entity = get_group(entity);
			if (likely(group_entity))
			{
				bool verified = false;
				const auto name_map = group_entity->get<name_map_component>();
				{
					const auto pair = std::make_pair(client->get_display(), entity);

					thread_lock(name_map);
					const auto result = name_map->insert(pair);

					if (!result.second)
					{
						// Dupe
						const auto& it = result.first;
						const auto& e = it->second;

						if (e != entity)
						{
							const auto c = e->get<client_component>();

							if (!client->is_verified())
							{
								if (!c->is_verified())
								{
									// Don't troll on crashes
									it->second = entity;
								}
								else
								{
									client->set_troll(true);
									client->set_display();
								}
							}
							else
							{
								if (!c->is_verified())
								{
									verified = true;

									// Do this here even though we should also catch it down below
									c->set_troll(true);
									c->set_display();
								}

								// Don't troll on refreshes
								it->second = entity;
							}
						}
					}
				}

				if (verified)
				{
					const auto client_map = group_entity->get<client_map_component>();
					{
						thread_lock(client_map);

						for (const auto& pair : *client_map)
						{
							const auto& e = pair.second;
							if (e != entity)
							{
								const auto c = e->get<client_component>();
								if (!c->is_verified() && (client->get_display() == c->get_display()))
								{
									c->set_troll(true);
									c->set_display();
								}
							}
						}
					}
				}
			}
		}
		else
		{
			client->set_display();

			// Consider all empty nicks trolls
			const auto group = entity->get<group_component>();
			if (!group->has_id())
				client->set_troll(true);
		}
	}

	// Remove
	void troll_command::remove(const entity::ptr& entity) const
	{
		const auto client = entity->get<client_component>();
		if (client->has_display())
		{
			const auto group_entity = get_group(entity);
			if (likely(group_entity))
			{
				const auto name_map = group_entity->get<name_map_component>();

				thread_lock(name_map);
				const auto it = name_map->find(client->get_display());

				if (it != name_map->end())
				{
					const auto& e = it->second;
					if (likely(e == entity))
						name_map->erase(it);
				}
			}

			client->set_display();
		}
	}
}
