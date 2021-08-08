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

#ifndef _EJA_CLI_APPLICATION_H_
#define _EJA_CLI_APPLICATION_H_

#include <memory>
#include <boost/filesystem.hpp>

#include "config/application.h"
#include "entity/entity.h"
#include "http/http_throttle.h"
#include "system/type.h"

namespace eja
{
	class cli_application final : public application
	{
	private:
		entity_list m_clients;
		http_download_throttle m_download_throttle;
		http_upload_throttle m_upload_throttle;
		size_t m_tabs = 0;

	public:
		using ptr = std::shared_ptr<cli_application>;

	private:
		// Utility
		void error_add(const std::shared_ptr<entity>& entity) const;

		// Read
		using application::read;
		void read(const boost::filesystem::path& path);
		void read_global(const std::shared_ptr<cpptoml::table>& table);
		void read_client(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_group(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_option(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_routers(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_session(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_shares(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_verified(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);

	public:
		cli_application(int& argc, char* argv[]);

		// Interface
		virtual void init() override;
		virtual void shutdown() override;
		virtual void clear() override;

		// Utility
		virtual bool empty() const override { return application::empty() || !has_clients(); }
		bool exists();

		// Read
		virtual void read() override;

		// Has
		bool has_clients() const { return !m_clients.empty(); }
		bool has_tabs() const { return m_tabs > 0; }

		// Set
		void set_download_throttle() { m_download_throttle.set_max_size(); }
		void set_tabs(const size_t tabs) { m_tabs = tabs; }
		void set_upload_throttle() { m_upload_throttle.set_max_size(); }

		// Get
		size_t get_size() const { return m_clients->size(); }
		size_t get_max_size() const;

		const entity_list& get_clients() const { return m_clients; }
		entity_list& get_clients() { return m_clients; }

		const http_download_throttle& get_download_throttle() const { return m_download_throttle; }
		http_download_throttle& get_download_throttle() { return m_download_throttle; }

		const http_upload_throttle& get_upload_throttle() const { return m_upload_throttle; }
		http_upload_throttle& get_upload_throttle() { return m_upload_throttle; }

		// Static
		static ptr create(int argc, char* argv[]) { return std::make_shared<cli_application>(argc, argv); }
	};
}

#endif
