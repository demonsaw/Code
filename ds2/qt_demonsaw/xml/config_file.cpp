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

#include "config_file.h"
#include "component/pixmap_component.h"
#include "component/session_component.h"
#include "component/client/client_component.h"
#include "component/error/error_component.h"
#include "component/group/group_component.h"
#include "component/io/file_component.h"
#include "component/server/server_component.h"
#include "component/transfer/chunk_component.h"
#include "xml/xml.h"

namespace eja
{
	// Interface
	bool config_file::read(const char* path)
	{
		const auto status = xml_config_file::read(path);

		if (status)
		{
			const auto& pt_root = m_tree.get_child(xml::demonsaw);

			// View
			const auto& pt_view = pt_root.get_child(xml::view, empty_ptree());
			m_option = pt_view.get<bool>(xml::option, default_view::option);
			m_toolbar = pt_view.get<bool>(xml::toolbar, default_view::toolbar);
			m_menubar = pt_view.get<bool>(xml::menubar, default_view::menubar);
			m_statusbar = pt_view.get<bool>(xml::statusbar, default_view::statusbar);
			m_detail = pt_view.get<bool>(xml::detail, default_view::detail);

			{
				// Router
				auto_lock_ref(m_routers);

				for (const auto& entity : m_routers)
				{
					entity->add<session_entity_list_component>();
					entity->add<client_entity_vector_component>();
					entity->add<group_entity_vector_component>();
					entity->add<chunk_entity_list_component>();
				}
			}
		}

		return status;
	}

	bool config_file::write(const char* path)
	{
		clear();

		// Root
		auto& pt_root = m_tree.get_child(xml::demonsaw);

		// View
		boost::property_tree::iptree pt_view;
		pt_view.put(xml::option, m_option);
		pt_view.put(xml::toolbar, m_toolbar);
		pt_view.put(xml::menubar, m_menubar);
		pt_view.put(xml::statusbar, m_statusbar);
		pt_view.put(xml::detail, m_detail);
		pt_root.put_child(xml::view, pt_view);

		return xml_config_file::write(path);
	}

	// Accessor
	void config_file::get_client_group(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		xml_config_file::get_client_group(tree, entity);

		const auto group_vector = entity->get<group_entity_vector_component>();
		
		auto_lock_ptr(group_vector);

		for (const auto& e : *group_vector)
		{
			// File
			const auto file = e->get<file_component>();

			// Pixmap
			const auto pixmap = pixmap_component::create();
			pixmap->set_path(file->get_path());
			e->add(pixmap);
		}
	}
}
