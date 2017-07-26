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

#ifndef _EJA_XML_CONFIG_FILE_
#define _EJA_XML_CONFIG_FILE_

#include <boost/property_tree/ptree.hpp>

#include "xml_file.h"
#include "controller/client_controller.h"
#include "controller/router_controller.h"
#include "utility/default_value.h"

namespace eja
{
	class xml_config_file : public xml_file
	{
	protected:
		client_controller m_clients;
		router_controller m_routers;
		size_t m_version = default_version::xml;

		// Mutator
		void put_client(boost::property_tree::iptree& tree, const entity::ptr& entity);
		void put_client_file(boost::property_tree::iptree& tree, const entity::ptr& entity);
		void put_client_folder(boost::property_tree::iptree& tree, const entity::ptr& entity);
		void put_client_group(boost::property_tree::iptree& tree, const entity::ptr& entity);
		void put_client_option(boost::property_tree::iptree& tree, const entity::ptr& entity);
		void put_client_router(boost::property_tree::iptree& tree, const entity::ptr& entity);
		void put_client_security(boost::property_tree::iptree& tree, const entity::ptr& entity);

		void put_router(boost::property_tree::iptree& tree, const entity::ptr& entity);
		void put_router_option(boost::property_tree::iptree& tree, const entity::ptr& entity);
		void put_router_servers(boost::property_tree::iptree& tree, const entity::ptr& entity);

		void put_server(boost::property_tree::iptree& tree, const entity::ptr& entity);
		void put_server_option(boost::property_tree::iptree& tree, const entity::ptr& entity);

		// Accessor
		virtual void get_client(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;
		virtual void get_client_file(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;
		virtual void get_client_folder(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;
		virtual void get_client_group(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;
		virtual void get_client_option(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;
		virtual void get_client_router(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;
		virtual void get_client_security(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;

		virtual void get_router(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;
		virtual void get_router_option(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;
		virtual void get_router_servers(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;

		virtual void get_server(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;
		virtual void get_server_option(const boost::property_tree::iptree& tree, const entity::ptr& entity) const;

	protected:
		xml_config_file() { }
		virtual ~xml_config_file() override { }

	public:
		// Using
		using xml_file::read;
		using xml_file::write;

		// Interface
		virtual void clear() override;
		virtual bool read(const char* path) override;
		virtual bool write(const char* path) override;

		// Accessor		
		client_controller& get_clients() { return m_clients; }
		router_controller& get_routers() { return m_routers; }
		size_t get_version() const { return m_version; }
	};
}

#endif
