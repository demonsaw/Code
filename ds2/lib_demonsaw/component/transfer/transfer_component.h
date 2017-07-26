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

#ifndef _EJA_TRANSFER_COMPONENT_
#define _EJA_TRANSFER_COMPONENT_

#include <memory>

#include "component/component.h"
#include "entity/entity.h"
#include "object/transfer/transfer.h"
#include "system/type.h"

namespace eja
{
	// Component
	class transfer_component : public component, public transfer
	{
	private:
		bool m_done = false;

	public:
		using ptr = std::shared_ptr<transfer_component>;

	protected:
		transfer_component() { }
		transfer_component(const transfer_type type) : transfer(type) { }

	public:
		virtual ~transfer_component() override { }

		// Using
		using component::update;
		using transfer::update;

		// Utility
		bool done() const;

		// Mutator
		void done(const bool value) { m_done = value; }		

		// Accessor
		std::string get_elapsed() const;
		std::string get_estimate() const;
		
		size_t get_speed() const;
		double get_percent() const { return get_ratio() * 100.0; }
		double get_ratio() const;

		// Static
		static ptr create() { return ptr(new transfer_component()); }
		static ptr create(const transfer_type type) { return  ptr(new transfer_component(type)); }
	};

	// Container
	derived_type(transfer_list_component, list_component<transfer>);
	derived_type(transfer_map_component, map_component<std::string, transfer>);
	derived_type(transfer_queue_component, queue_component<transfer>);
	derived_type(transfer_vector_component, vector_component<transfer>);

	derived_type(transfer_entity_list_component, entity_list_component);
	derived_type(transfer_entity_map_component, entity_map_component);
	derived_type(transfer_entity_queue_component, entity_queue_component);
	derived_type(transfer_entity_vector_component, entity_vector_component);
}

#endif
