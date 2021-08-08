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

#ifndef _EJA_ENTITY_UTIL_H_
#define _EJA_ENTITY_UTIL_H_

#include <memory>
#include <string>
#include <vector>

#include "component/time/timeout_component.h"
#include "entity/entity.h"
#include "security/security.h"
#include "thread/thread_safe.h"
#include "utility/value.h"

namespace eja
{
	class entity_util final
	{
	private:
		entity_util() = delete;
		~entity_util() = delete;

	public:
		// Utility
		template <typename T>
		static std::string add(const std::shared_ptr<entity>& parent, const std::shared_ptr<entity>& entity);

		template <typename T>
		static void expire(const std::shared_ptr<entity>& parent);

		template <typename T>
		static std::vector<std::shared_ptr<entity>> transform(const std::shared_ptr<entity>& parent);
	};

	// Utility
	template <typename T>
	std::string entity_util::add(const std::shared_ptr<entity>& parent, const std::shared_ptr<entity>& entity)
	{
		std::string id;

		while (true)
		{
			id = security::get_id();
			const auto map = parent->get<T>();
			const auto pair = std::make_pair(id, entity);
			typename T::iterator it;

			thread_lock(map);
			//assert(map->size() < 32);

			it = map->find(id);
			if (it == map->end())
			{
				const auto result = map->insert(pair);
				if (!result.second)
				{
					// Replace
					const auto it = result.first;
					it->second = entity;
				}

				break;
			}
		}

		return id;
	}

	template <typename T>
	void entity_util::expire(const std::shared_ptr<entity>& parent)
	{
		const auto map = parent->get<T>();
		{
			thread_lock(map);

			for (auto it = map->begin(); it != map->end(); /*++it*/)
			{
				const std::shared_ptr<entity>& e = it->second;
				const auto timeout = e->get<timeout_component>();

				if (timeout->expired(milliseconds::timeout))
					map->erase(it++);
				else
					++it;
			}
		}
	}

	template <typename T>
	std::vector<std::shared_ptr<entity>> entity_util::transform(const std::shared_ptr<entity>& parent)
	{
		std::vector<std::shared_ptr<entity>> vector;
		const auto map = parent->get<T>();
		{
			thread_lock(map);

			vector.reserve(map->size());
			std::transform(map->begin(), map->end(), std::back_inserter(vector), [](typename T::value_type& pair) {return pair.second; });
		}

		return vector;
	}
}

#endif
