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

#include "config/application.h"
#include "entity/entity.h"
#include "system/type.h"

namespace eja
{
	class cli_application final : public application
	{
	private:
		entity_list m_routers;

	public:
		using ptr = std::shared_ptr<cli_application>;

	private:
		// Utility
		//void error_add(const std::shared_ptr<entity>& entity) const;

		// Read
		void read_router(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		//void read_headers(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_option(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_rooms(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_routers(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_verifieds(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);

	protected:
		using application::read;
		using application::write;

	public:
		cli_application(int& argc, char* argv[]);

		// Interface
		virtual void init() override;
		virtual void shutdown() override;
		virtual void clear() override;

		// Utility
		virtual bool empty() const override { return application::empty() || !has_routers(); }

		// Read
		virtual void read() override;

		// Has
		bool has_routers() const { return !m_routers.empty(); }

		// Get
		const entity_list& get_routers() const { return m_routers; }
		entity_list& get_routers() { return m_routers; }

		// Static
		static ptr create(int argc, char* argv[]) { return std::make_shared<cli_application>(argc, argv); }
	};
}

#endif
