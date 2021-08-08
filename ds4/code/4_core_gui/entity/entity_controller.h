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

#ifndef _EJA_ENTITY_CONTROLLER_H_
#define _EJA_ENTITY_CONTROLLER_H_

#include <list>

#include <QObject>

#include "entity/entity_object.h"
#include "entity/entity_dock_widget.h"

namespace eja
{
	class entity;

	class entity_controller : public entity_object
	{
		Q_OBJECT;

	protected:
		std::list<entity_dock_widget*> m_list;

	public:
		entity_controller(QObject* parent = nullptr) : entity_object(parent) { }
		entity_controller(const std::shared_ptr<entity>& entity, QObject* parent = nullptr) : entity_object(entity, parent) { }

		// Has
		using entity_object::has_entity;
		bool has_entity(const std::string& id) const;
		bool has_widget(const std::string& id) const;

		// Is
		bool is_visible() const;
		bool is_visible(const void* ptr) const;
		bool is_visible(const std::shared_ptr<entity>& entity) const;

		// Get
		template <typename T>
		const std::string& get_id(const std::shared_ptr<entity>& entity) const;
		virtual const std::string& get_id(const std::shared_ptr<entity>& entity) const = 0;

		using entity_object::get_entity;
		std::shared_ptr<entity> get_entity(const std::shared_ptr<entity>& entity) const;
		std::shared_ptr<entity> get_entity(const std::string& id) const;

		entity_dock_widget* get_widget(const std::shared_ptr<entity>& entity) const;
		entity_dock_widget* get_widget(const std::string& id) const;

		std::shared_ptr<entity> get_visible_entity() const;
		entity_dock_widget* get_visible_widget() const;
	};

	// Get
	template <typename T>
	const std::string& entity_controller::get_id(const std::shared_ptr<entity>& entity) const
	{
		assert(thread_info::main());

		const auto comp = entity->get<T>();
		return comp->get_id();
	}
}

#endif
