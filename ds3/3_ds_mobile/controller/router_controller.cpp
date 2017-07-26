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

#include "component/endpoint_component.h"
#include "component/session/session_security_component.h"

#include "controller/router_controller.h"
#include "utility/value.h"

namespace eja
{
	// Set
	void router_controller::set_address(const QString& qaddress)
	{
		const auto message_list = m_entity->get<message_list_component>();
		assert(!message_list->empty());

		const auto entity = message_list->front();
		const auto endpoint = entity->get<endpoint_component>();
		
		const auto address = qaddress.trimmed().toStdString();
		endpoint->set_address(address);
		endpoint->set_name(address);

		emit addressChanged();
	}

	void router_controller::set_port(const u16 port)
	{
		const auto message_list = m_entity->get<message_list_component>();
		assert(!message_list->empty());

		const auto entity = message_list->front();
		const auto endpoint = entity->get<endpoint_component>();
		endpoint->set_port(port);

		emit portChanged();
	}

	void router_controller::set_prime_size(const size_t size)
	{
		const auto security = m_entity->get<session_security_component>();
		security->set_prime_size(size);

		emit prime_sizeChanged();
	}

	void router_controller::set_cipher(const QString& qcipher)
	{
		const auto security = m_entity->get<session_security_component>();
		const auto cipher = qcipher.trimmed().toStdString();
		security->set_cipher(cipher);

		emit cipherChanged();
	}

	void router_controller::set_key_size(const size_t key_size)
	{
		const auto security = m_entity->get<session_security_component>();
		security->set_key_size(key_size);

		emit key_sizeChanged();
	}

	void router_controller::set_hash(const QString& qhash)
	{
		const auto security = m_entity->get<session_security_component>();
		const auto hash = qhash.trimmed().toStdString();
		security->set_hash(hash);

		emit hashChanged();
	}

	void router_controller::set_iterations(const size_t iterations)
	{
		const auto security = m_entity->get<session_security_component>();
		security->set_iterations(iterations);

		emit iterationsChanged();
	}

	void router_controller::set_salt(const QString& qsalt)
	{
		const auto security = m_entity->get<session_security_component>();
		const auto salt = qsalt.trimmed().toStdString();
		security->set_salt(salt);

		emit saltChanged();
	}

	// Get
	entity::ptr router_controller::get_router() const
	{
		const auto message_list = m_entity->get<message_list_component>();
		assert(!message_list->empty());

		return message_list->front();
	}

	QString router_controller::get_address() const
	{
		const auto entity = get_router();
		const auto endpoint = entity->get<endpoint_component>();
		return QString::fromStdString(endpoint->get_address());
	}

	u16 router_controller::get_port() const
	{
		const auto entity = get_router();
		const auto endpoint = entity->get<endpoint_component>();
		return endpoint->get_port();
	}

	size_t router_controller::get_prime_size() const
	{
		const auto security = m_entity->get<session_security_component>();
		return security->get_prime_size();
	}

	QString router_controller::get_cipher() const
	{
		const auto security = m_entity->get<session_security_component>();
		return QString::fromStdString(security->get_cipher());
	}

	size_t router_controller::get_key_size() const
	{
		const auto security = m_entity->get<session_security_component>();
		return security->get_key_size();
	}

	QString router_controller::get_hash() const
	{
		const auto security = m_entity->get<session_security_component>();
		return QString::fromStdString(security->get_hash());
	}

	size_t router_controller::get_iterations() const
	{
		const auto security = m_entity->get<session_security_component>();
		return security->get_iterations();
	}

	QString router_controller::get_salt() const
	{
		const auto security = m_entity->get<session_security_component>();
		return QString::fromStdString(security->get_salt());
	}
}
