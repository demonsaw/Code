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

#include <QTimer>

#include "component/endpoint_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/group/group_security_component.h"

#include "controller/application_controller.h"
#include "controller/group_controller.h"
#include "controller/message_controller.h"
#include "entity/entity.h"
#include "model/group_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "system/type.h"
#include "thread/thread_info.h"
#include "utility/locale.h"
#include "utility/utf8.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	group_controller::group_controller(QObject* parent /*= nullptr*/) : entity_object(parent)
	{
		m_model = new group_model(this);
	}

	// Interface
	void group_controller::add(const entity::ptr entity)
	{
		assert(thread_info::main() && has_entity());

		m_model->add(entity);

		emit sizeChanged();
	}

	void group_controller::update()
	{
		assert(thread_info::main() && has_entity());

		m_model->update();

		emit sizeChanged();
	}

	void group_controller::clear()
	{
		assert(thread_info::main() && has_entity());

		m_model->clear();

		emit sizeChanged();
	}

	void group_controller::restart()
	{
		// Spam
		if (!m_enabled)
			return;

		// HACK: qml wigs out
		clear();

		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->async_client();

		// Spam
		m_enabled = false;
		QTimer::singleShot(ui::enter, [this]() { m_enabled = true; });
	}

	void group_controller::reset()
	{
		assert(has_entity());

		m_model->reset();

		emit sizeChanged();
	}

	// Utility
	bool group_controller::empty() const 
	{ 
		return m_model->size() <= 1; 
	}

	void group_controller::mute(const int row, const bool value /*= true*/)
	{
		const auto entity = m_model->get_entity(row);

		if (entity)
		{			
			const auto endpoint = entity->get<endpoint_component>();
			endpoint->set_mute(value);

			// HACK: Associate m_entity and the client list version
			//
			if (m_entity->equals<endpoint_component>(entity))
			{
				const auto endpoint = m_entity->get<endpoint_component>();
				endpoint->set_mute(value);
			}

			// UI
			const auto index = m_model->get_index(row);
			m_model->update(index);
		}
	}

	void group_controller::message(const int row)
	{
		assert(thread_info::main());

		const auto entity = m_model->get_entity(row);

		if (entity && entity->has_parent())
		{
			const auto app = application_controller::get();
			const auto control = app->get_message_controller();
			control->set_entity(entity);
			control->set_visible(true);
		}
	}

	// Set
	void group_controller::set_entity(const entity::ptr entity)
	{
		entity_object::set_entity(entity);

		m_model->set_entity(entity);

		emit sizeChanged();
	}

	void group_controller::set_enabled(const bool enabled)
	{
		const auto entity = get_group();
		entity->set_enabled(enabled);
		
		const auto group = entity->get<group_security_component>();
		group->set_size(0);

		emit enabledChanged();
	}

	void group_controller::set_entropy(const QString& qentropy)
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		const auto entropy = qentropy.trimmed().toStdString();
		group->set_entropy(entropy);

		emit entropyChanged();		
	}

	void group_controller::set_cipher(const QString& qcipher)
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		const auto cipher = qcipher.trimmed().toStdString();
		group->set_cipher(cipher);

		emit cipherChanged();
	}

	void group_controller::set_key_size(const size_t key_size)
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		group->set_key_size(key_size);

		emit key_sizeChanged();
	}

	void group_controller::set_hash(const QString& qhash)
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		const auto hash = qhash.trimmed().toStdString();
		group->set_hash(hash);

		emit hashChanged();
	}

	void group_controller::set_iterations(const size_t iterations)
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		group->set_iterations(iterations);

		emit iterationsChanged();
	}

	void group_controller::set_salt(const QString& qsalt)
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		const auto salt = qsalt.trimmed().toStdString();
		group->set_salt(salt);

		emit saltChanged();
	}

	// Get
	size_t group_controller::get_size() const 
	{ 
		return m_model->size(); 
	}

	QString group_controller::get_id() const
	{
		const auto group = m_entity->get<group_component>();
		if (group->has_id())
		{
			const auto group_id = hex::encode(static_cast<u32>(crc32()(group->get_id())));
			return QString::fromStdString(group_id);
		}
		
		return "";
	}

	entity::ptr group_controller::get_group() const
	{
		const auto group_list = m_entity->get<group_security_list_component>();
		assert(!group_list->empty());

		return group_list->front();
	}

	bool group_controller::get_enabled() const
	{
		const auto entity = get_group();
		return entity->enabled();
	}

	QString group_controller::get_entropy() const
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		return QString::fromStdString(group->get_entropy());
	}

	QString group_controller::get_cipher() const
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		return QString::fromStdString(group->get_cipher());
	}

	size_t group_controller::get_key_size() const
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		return group->get_key_size();
	}

	QString group_controller::get_hash() const
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		return QString::fromStdString(group->get_hash());
	}

	size_t group_controller::get_iterations() const
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		return group->get_iterations();
	}

	QString group_controller::get_salt() const
	{
		const auto entity = get_group();
		const auto group = entity->get<group_security_component>();
		return QString::fromStdString(group->get_salt());
	}
}
