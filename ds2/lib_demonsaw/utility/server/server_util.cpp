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

#include "server_util.h"
#include "component/function_component.h"
#include "component/session_component.h"
#include "component/error/error_component.h"
#include "component/server/server_component.h"
#include "component/server/server_machine_component.h"
#include "component/server/server_option_component.h"
#include "component/server/server_security_component.h"

namespace eja
{
	// Static
	mutex server_util::m_mutex;

	// Utility
	entity::ptr server_util::create()
	{
		const auto entity = eja::entity::create();

		entity->add<server_component>();
		entity->add<server_option_component>();
		entity->add<server_security_component>();		
		entity->add<server_machine_component>();
		entity->add<session_component>();

		// Error
		entity->add<error_entity_vector_component>();

		return entity;
	}

	entity::ptr server_util::create(const entity::ptr parent)
	{
		const auto entity = create();
		entity->set_entity(parent);

		return entity;
	}
}