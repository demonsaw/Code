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
#include "component/chat_model_component.h"
#include "component/debug_model_component.h"
#include "component/group_model_component.h"
#include "component/client/client_option_component.h"
#include "component/client/client_service_component.h"
#include "component/status/status_component.h"

#include "controller/application_controller.h"
#include "controller/client_controller.h"
#include "model/client_model.h"
#include "thread/thread_info.h"
#include "utility/locale.h"
#include "utility/utf8.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	client_controller::client_controller(entity_list& list, QObject* parent /*= nullptr*/) : entity_object(parent)
	{
		m_model = new client_model(list, this);

		/*m_proxy = new QSortFilterProxyModel(this);
		m_proxy->setSortRole(Qt::UserRole);
		m_proxy->setSourceModel(m_model);		
		m_proxy->setDynamicSortFilter(true);
		m_proxy->sort(client_model::column::role_name);*/
	}

	// Interface
	void client_controller::add()
	{
		assert(thread_info::main() && has_entity());

		m_model->add(m_entity);

		const auto service = m_entity->get<client_service_component>();
		if (service->invalid())
		{
			m_entity->enable();
			service->start();
		}

		const auto app = application_controller::get();
		app->set_index(m_model->size() - 1);
		
		emit nameChanged();
		emit sizeChanged();
	}

	void client_controller::remove(const int row)
	{
		assert(thread_info::main() && has_entity());

		const auto entity = m_model->get_entity(row);

		if (entity)
		{
			const auto chat_model = entity->get<chat_model_component>();
			chat_model->reset();

			const auto debug_model = entity->get<debug_model_component>();
			debug_model->reset();

			const auto group_model = entity->get<group_model_component>();
			group_model->reset();

			if (entity->enabled())
			{
				const auto service = entity->get<client_service_component>();
				if (service->valid())
					service->async_quit();
			}

			m_model->removeRow(row);

			emit sizeChanged();
		}
	}

	void client_controller::update()
	{
		assert(thread_info::main() && has_entity());

		m_model->update();

		emit sizeChanged();
	}

	void client_controller::restart()
	{
		assert(thread_info::main() && has_entity());

		// Spam
		if (!m_enabled)
			return;

		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->restart();

		// Spam
		m_enabled = false;
		QTimer::singleShot(ui::enter, [this]() { m_enabled = true; });
	}

	void client_controller::clear()
	{
		assert(thread_info::main() && has_entity());

		m_model->clear();

		emit sizeChanged();
	}

	void client_controller::reset()
	{
		assert(has_entity());

		m_model->reset();

		emit sizeChanged();
	}

	// Utility
	bool client_controller::empty() const
	{
		return m_model->empty(); 
	}

	void client_controller::sort()
	{
		m_model->sort();
	}

	// Set
	void client_controller::set_entity(const entity::ptr entity)
	{
		entity_object::set_entity(entity);

		m_model->update();

		emit nameChanged();
		emit colorChanged();
		emit text_colorChanged();
		emit statusChanged();
		emit sizeChanged();
		emit flashChanged();
	}

	void client_controller::set_name(const QString& qname)
	{
		const auto endpoint = m_entity->get<endpoint_component>();
		const auto name = qname.trimmed().toStdString();
		endpoint->set_name(name);
		
		// Model
		application_controller::set_initial(m_entity);

		m_model->update(m_entity);

		emit nameChanged();
	}

	void client_controller::set_color(const QColor& color)
	{
		const auto endpoint = m_entity->get<endpoint_component>();
		endpoint->set_color(color.rgba());

		m_model->update(m_entity);

		emit colorChanged();
		emit text_colorChanged();
	}

	void client_controller::set_flash(const bool flash)
	{
		const auto option = m_entity->get<client_option_component>();
		option->set_flash(flash);

		emit flashChanged();
	}

	// Get
	size_t client_controller::get_size() const 
	{ 
		return m_model->size(); 
	}

	QString client_controller::get_name() const
	{
		const auto endpoint = m_entity->get<endpoint_component>();
		return QString::fromStdString(endpoint->get_name());
	}

	QColor client_controller::get_color() const
	{
		QColor qcolor;
		const auto endpoint = m_entity->get<endpoint_component>();
		qcolor.setRgba(endpoint->get_color());
		return qcolor;
	}

	QColor client_controller::get_text_color() const
	{
		const auto endpoint = m_entity->get<endpoint_component>();
		const double value = ((0.299 * endpoint->get_red()) + (0.587 * endpoint->get_green()) + 0.114 * endpoint->get_blue()) / 255.0;
		return (value <= 0.5) ? QColor(0xffffffff) : QColor(0xff484840);
	}

	QVector4D client_controller::get_color_hsva() const
	{
		QColor qcolor = get_color();
		QVector4D hsva = QVector4D(qcolor.hsvHueF(), qcolor.hsvSaturationF(), qcolor.valueF(), qcolor.alphaF());
		return hsva;
	}

	QColor client_controller::get_status() const
	{
		QColor color;
		const auto status = m_entity->get<status_component>();
		color.setRgba(status->get_color());
		return color;
	}

	bool client_controller::has_flash() const
	{
		const auto option = m_entity->get<client_option_component>();
		return option->has_flash();
	}
}
