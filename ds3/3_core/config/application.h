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

#ifndef _EJA_APPLICATION_H_
#define _EJA_APPLICATION_H_

#include <memory>
#include <string>
#include <type_traits>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

#include "config/cpptoml.h"
#include "entity/entity.h"
#include "system/type.h"

namespace eja
{
	class entity;

	class application
	{
	protected:
		size_t m_index = 0;
		std::string m_theme;

		entity_list m_clients;
		entity_list m_routers;

		boost::filesystem::path m_path;
		std::string m_geometry;
		std::string m_state;

		bool m_saving_enabled = true;
		bool m_clients_enabled = true;
		bool m_routers_enabled = true;

	public:
		using ptr = std::shared_ptr<application>;

	protected:
		// Read - Client
		void read_client(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void read_client_io(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void read_client_message(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void read_client_network(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void read_client_options(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void read_client_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void read_client_session(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void read_client_transfer(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);

		// Read - Router
		void read_router(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);		
		void read_router_options(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);

		// Read - Misc		
		template <typename T, typename U>
		void read(const std::shared_ptr<cpptoml::table>& table, const char* name, U u) const;

		template <typename T, typename U>
		void read_ex(const std::shared_ptr<cpptoml::table>& table, const char* name, U u) const;

		void read_groups(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void read_headers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void read_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void read_ui(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);

		// Write - Client
		void write_client(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void write_client_io(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity); 
		void write_client_message(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void write_client_network(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void write_client_options(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void write_client_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void write_client_session(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void write_client_transfer(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);

		// Write - Router
		void write_router(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);		
		void write_router_options(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);

		// Write - Misc
		template <typename T, typename U>
		void write(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t, U u) const;

		template <typename T>
		void write(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t) const { return write(table, name, t, []() { return true; }); }

		template <typename T, typename U>
		void write_ex(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t, U u) const;

		template <typename T>
		void write_ex(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t) const { return write_ex(table, name, t, []() { return true; }); }

		template <typename U>
		void write(const std::shared_ptr<cpptoml::table>& table, const char* name, const size_t t) const { return write(table, name, t, []() { return true; }); }		
		void write_groups(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void write_headers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void write_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);
		void write_ui(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity);

	public:
		application(int argc, char* argv[]);
		virtual ~application() { }

		// Interface
		virtual void init();
		virtual void update() { }
		virtual void shutdown();
		virtual void clear();

		// Utility
		bool empty() const { return m_routers->empty() && m_clients->empty(); }

		void load() { load(m_path.string()); }
		void load(const std::string& str) { load(str.c_str()); }
		virtual void load(const char* psz);

		void save() { save(m_path.string()); }
		void save(const std::string& str) { save(str.c_str()); }
		virtual void save(const char* psz);

		// Has
		bool has_routers() const { return !m_routers.empty(); }
		bool has_clients() const { return !m_clients.empty(); }

		bool has_geometry() const { return !m_geometry.empty(); }
		bool has_state() const { return !m_state.empty(); }
		bool has_theme() const { return !m_theme.empty(); }

		// Set
		void set_path(const std::string& path) { m_path = path; }
		void set_geometry(const std::string& geometry) { m_geometry = geometry; }
		void set_state(const std::string& state) { m_state = state; }

		virtual void set_theme() { m_theme.clear(); }
		virtual void set_theme(const std::string& theme) { m_theme = theme; }

		void set_index(const size_t index) { m_index = index; }

		// Get
		const entity_list& get_clients() const { return m_clients; }
		entity_list& get_clients() { return m_clients; }

		const entity_list& get_routers() const { return m_routers; }
		entity_list& get_routers() { return m_routers; }

		const boost::filesystem::path& get_path() const { return m_path;}
		const std::string& get_geometry() const { return m_geometry; }
		const std::string& get_state() const { return m_state; }
		const std::string& get_theme() const { return m_theme; }
		size_t get_index() const { return m_index; }

		// Static
		static ptr create(int argc, char* argv[]) { return ptr(new application(argc, argv)); }
	};

	// Read
	template <typename T, typename U>
	inline void application::read(const std::shared_ptr<cpptoml::table>& table, const char* name, U u) const
	{
		if (table->contains(name))
		{
			const auto value = table->get_as<T>(name);
			if (value)
				u(*value);
		}
	}

	template <typename T, typename U>
	inline void application::read_ex(const std::shared_ptr<cpptoml::table>& table, const char* name, U u) const
	{
		if (table->contains(name))
		{
			const auto value = table->get_as<int64_t>(name);
			if (value)
				u(static_cast<T>(*value));
		}
	}

	// Write
	template <typename T, typename U>
	inline void application::write(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t, U u) const
	{
		if (u())
			table->insert(name, t);
	}

	template <typename T, typename U>
	inline void application::write_ex(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t, U u) const
	{
		if (u())
			table->insert(name, static_cast<int64_t>(t));
	}
}

#endif
