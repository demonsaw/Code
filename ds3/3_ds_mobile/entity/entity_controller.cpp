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

#include "entity/entity.h"
#include "entity/entity_controller.h"

namespace eja
{
	// Constructor
	entity_controller::entity_controller(const callback_type type /*= callback_type::none*/, QObject* parent /*= nullptr*/) : QObject(parent), entity_callback(type)
	{
		connect(this, static_cast<void (entity_controller::*)()>(&entity_controller::create), this, static_cast<void (entity_controller::*)()>(&entity_controller::on_create));
		connect(this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::create), this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::on_create));

		connect(this, static_cast<void (entity_controller::*)()>(&entity_controller::destroy), this, static_cast<void (entity_controller::*)()>(&entity_controller::on_destroy));
		connect(this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::destroy), this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::on_destroy));

		connect(this, static_cast<void (entity_controller::*)()>(&entity_controller::init), this, static_cast<void (entity_controller::*)()>(&entity_controller::on_init));
		connect(this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::init), this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::on_init));

		connect(this, static_cast<void (entity_controller::*)()>(&entity_controller::update), this, static_cast<void (entity_controller::*)()>(&entity_controller::on_update));
		connect(this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::update), this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::on_update));

		connect(this, static_cast<void (entity_controller::*)()>(&entity_controller::shutdown), this, static_cast<void (entity_controller::*)()>(&entity_controller::on_shutdown));
		connect(this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::shutdown), this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::on_shutdown));

		connect(this, static_cast<void (entity_controller::*)()>(&entity_controller::add), this, static_cast<void (entity_controller::*)()>(&entity_controller::on_add));
		connect(this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::add), this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::on_add));

		connect(this, static_cast<void (entity_controller::*)()>(&entity_controller::remove), this, static_cast<void (entity_controller::*)()>(&entity_controller::on_remove));
		connect(this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::remove), this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::on_remove));

		connect(this, static_cast<void (entity_controller::*)()>(&entity_controller::clear), this, static_cast<void (entity_controller::*)()>(&entity_controller::on_clear));
		connect(this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::clear), this, static_cast<void (entity_controller::*)(const entity::ptr)>(&entity_controller::on_clear));
	}

	entity_controller::entity_controller(const entity::ptr entity, const callback_type type /*= callback_type::none*/, QObject* parent /*= nullptr*/) : entity_controller(type, parent)
	{
		set_entity(entity);
	}
}
