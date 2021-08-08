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

#include <cassert>

#include "component/component_object.h"
#include "entity/entity.h"

namespace eja
{
	// Constructor
	component_object::component_object(QObject* parent /*= nullptr*/) : QObject(parent)
	{
		connect(this, static_cast<void (component_object::*)()>(&component_object::create), this, static_cast<void (component_object::*)()>(&component_object::on_create));
		connect(this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::create), this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::on_create));

		connect(this, static_cast<void (component_object::*)()>(&component_object::destroy), this, static_cast<void (component_object::*)()>(&component_object::on_destroy));
		connect(this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::destroy), this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::on_destroy));

		connect(this, static_cast<void (component_object::*)()>(&component_object::add), this, static_cast<void (component_object::*)()>(&component_object::on_add));
		connect(this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::add), this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::on_add));

		connect(this, static_cast<void (component_object::*)()>(&component_object::remove), this, static_cast<void (component_object::*)()>(&component_object::on_remove));
		connect(this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::remove), this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::on_remove));

		connect(this, static_cast<void (component_object::*)()>(&component_object::update), this, static_cast<void (component_object::*)()>(&component_object::on_update));
		connect(this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::update), this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::on_update));

		connect(this, static_cast<void (component_object::*)()>(&component_object::clear), this, static_cast<void (component_object::*)()>(&component_object::on_clear));
		connect(this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::clear), this, static_cast<void (component_object::*)(const entity::ptr entity)>(&component_object::on_clear));

		connect(this, static_cast<void (component_object::*)(const QModelIndex& index)>(&component_object::click), this, static_cast<void (component_object::*)(const QModelIndex& index)>(&component_object::on_click));
		connect(this, static_cast<void (component_object::*)(const QModelIndex& index)>(&component_object::double_click), this, static_cast<void (component_object::*)(const QModelIndex& index)>(&component_object::on_double_click));
		connect(this, static_cast<void (component_object::*)(const QModelIndex& index)>(&component_object::execute), this, static_cast<void (component_object::*)(const QModelIndex& index)>(&component_object::on_execute));
	}
}
