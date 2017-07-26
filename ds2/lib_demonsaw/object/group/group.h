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

#ifndef _EJA_GROUP_
#define _EJA_GROUP_

#include "object/object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	class group : public object
	{
	protected:
		std::string m_name;

	public:
		using ptr = std::shared_ptr<group>;

	private:
		void set_name();

	public:
		group() : m_name(default_group::name) { }
		group(const eja::group& group) : object(group), m_name(group.m_name) { }
		group(const std::string& id) : object(id) { set_name(); }
		group(const char* id) : object(id) { set_name(); }
		virtual ~group() override { }

		// Interface
		virtual void clear() override;

		// Utility
		bool has_name() const { return !m_name.empty(); }

		// Mutator
		virtual void set_id() override;
		virtual void set_id(const std::nullptr_t null) override;
		virtual void set_id(const byte* id, const size_t size) override;
		virtual void set_id(const std::string& id) override;
		virtual void set_id(const char* id) override;

		void set_name(const std::string& name) { m_name = name; }
		void set_name(const char* name) { m_name = name; }

		// Accessor
		const std::string& get_name() const { return m_name; }

		// Static
		static ptr create() { return std::make_shared<group>(); }
		static ptr create(const eja::group& group) { return std::make_shared<eja::group>(group); }
		static ptr create(const group::ptr group) { return std::make_shared<eja::group>(*group); }
		static ptr create(const std::string& id) { return std::make_shared<group>(id); }
		static ptr create(const char* id) { return std::make_shared<group>(id); }
	};

	// Container
	derived_type(group_list, mutex_list<group>);
	derived_type(group_map, mutex_map<std::string, group>);
	derived_type(group_queue, mutex_queue<group>);
	derived_type(group_vector, mutex_vector<group>);
}

#endif
