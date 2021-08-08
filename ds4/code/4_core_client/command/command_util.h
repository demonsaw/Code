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

#ifndef _EJA_COMMAND_UTIL_H_
#define _EJA_COMMAND_UTIL_H_

#include <memory>
#include <string>
#include <vector>

namespace eja
{
	class client_data;
	class entity;
	class room_data;
	class router_data;

	class command_util final
	{
	private:
		command_util() = delete;
		~command_util() = delete;

	public:
		// Interface
		static void update(const std::shared_ptr<entity>& entity, const std::shared_ptr<client_data>& data);
		static void update(const std::shared_ptr<entity>& entity, const std::shared_ptr<room_data>& data);
		static void update(const std::shared_ptr<entity>& entity, const std::shared_ptr<router_data>& data);

		// Get
		static std::shared_ptr<entity> get_browse_entity(const std::shared_ptr<entity>& entity);
	};
}

#endif
