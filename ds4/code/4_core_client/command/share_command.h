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

#ifndef _EJA_SHARE_COMMAND_H_
#define _EJA_SHARE_COMMAND_H_

#include "entity/entity_command.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class hash;

	class share_command final : public entity_command
	{
		make_factory(share_command);

	private:
		// Add
		void add(const std::string& name, const std::shared_ptr<entity>& entity) const;
		void add_file(const std::shared_ptr<entity>& entity) const;
		void add_folder(const std::shared_ptr<entity>& entity) const;

		// Remove
		void remove(const std::string& name, const std::shared_ptr<entity>& entity) const;

		// Get
		std::shared_ptr<hash> get_hash() const;

	public:
		share_command() { }
		share_command(const std::shared_ptr<entity>& entity) : entity_command(entity) { }

		// Add
		void add(const std::shared_ptr<entity>& entity) const;

		// Remove
		void remove(const std::shared_ptr<entity>& entity) const;

		// Static
		static ptr create(const std::shared_ptr<entity>& entity) { return std::make_shared<share_command>(entity); }
	};
}

#endif
