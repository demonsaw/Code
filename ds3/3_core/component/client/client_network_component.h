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

#ifndef _EJA_CLIENT_NETWORK_COMPONENT_H_
#define _EJA_CLIENT_NETWORK_COMPONENT_H_

#include <string>
#include <vector>

#include "component/component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	enum class network_mode { none, poll, server, tunnel, min = none, max = tunnel };
	enum class network_trust { none, router, any, min = none, max = any };

	class client_network_component : public component
	{
		make_factory(client_network_component);

	private:
		std::string m_redirect;
		network_mode m_mode = network_mode::none;
		network_trust m_trust = network_trust::none;		

		size_t m_chunk = 0;		
		size_t m_downloads = 0;
		size_t m_interval = 0;
		size_t m_retries = 0;		
		size_t m_sockets = 0;
		size_t m_threads = 0;
		size_t m_timeout = 0;
		size_t m_uploads = 0;
		size_t m_window = 0;

		bool m_motd = false;
		bool m_share = false;
		bool m_version = false;

		bool m_browse = false;
		bool m_chat = false;
		bool m_pm = false;
		bool m_search = false;		
		bool m_upload = false;

	public:
		client_network_component();
		client_network_component(const client_network_component& comp) : component(comp), m_mode(comp.m_mode), m_redirect(comp.m_redirect), m_trust(comp.m_trust),
			m_chunk(comp.m_chunk), m_downloads(comp.m_downloads), m_interval(comp.m_interval), m_retries(comp.m_retries), m_threads(comp.m_threads), m_sockets(comp.m_sockets), m_timeout(comp.m_timeout), m_uploads(comp.m_uploads),
			m_motd(comp.m_motd), m_share(comp.m_share), m_version(comp.m_version), m_browse(comp.m_browse), m_chat(comp.m_chat), m_pm(comp.m_pm), m_search(comp.m_search), m_upload(comp.m_upload), m_window(comp.m_window) { }

		// Operator
		client_network_component& operator=(const client_network_component& comp);

		// Interface
		virtual void clear() override;

		// Has		
		bool has_mode() const { return m_mode != network_mode::none; }
		bool has_redirect() const { return !m_redirect.empty(); }
		bool has_trust() const { return m_trust != network_trust::none; }

		bool has_chunk() const { return m_chunk > 0; }
		bool has_downloads() const { return m_downloads > 0; }
		bool has_interval() const { return m_interval > 0; }
		bool has_retries() const { return m_retries > 0; }		
		bool has_sockets() const { return m_sockets > 0; }
		bool has_threads() const { return m_threads > 0; }
		bool has_timeout() const { return m_timeout > 0; }
		bool has_uploads() const { return m_uploads > 0; }
		bool has_window() const { return m_window > 0; }

		bool has_motd() const { return m_motd; }
		bool has_share() const { return m_share; }
		bool has_version() const { return m_version; }

		bool has_browse() const { return m_browse; }
		bool has_chat() const { return m_chat; }
		bool has_pm() const { return m_pm; }
		bool has_search() const { return m_search; }		
		bool has_upload() const { return m_upload; }

		// Set
		void set_mode(const network_mode type) { m_mode = type; }
		void set_redirect(const std::string& redirect) { m_redirect = redirect; }
		void set_trust(const network_trust trust) { m_trust = trust; }

		void set_chunk(const size_t chunk) { m_chunk = chunk; }
		void set_downloads(const size_t downloads) { m_downloads = downloads; }
		void set_interval(const size_t interval) { m_interval = interval; }
		void set_retries(const size_t retries) { m_retries = retries; }		
		void set_sockets(const size_t sockets) { m_sockets = sockets; }
		void set_threads(const size_t threads) { m_threads = threads; }
		void set_timeout(const size_t timeout) { m_timeout = timeout; }
		void set_uploads(const size_t uploads) { m_uploads = uploads; }
		void set_window(const size_t window) { m_window = window; }

		void set_motd(const bool motd) { m_motd = motd; }
		void set_share(const bool share) { m_share = share; }
		void set_version(const bool version) { m_version = version; }

		void set_browse(const bool browse) { m_browse = browse; }
		void set_chat(const bool chat) { m_chat = chat; }
		void set_pm(const bool pm) { m_pm = pm; }
		void set_search(const bool search) { m_search = search; }		
		void set_upload(const bool upload) { m_upload = upload; }

		// Get
		network_mode get_mode() const { return m_mode; }
		const std::string& get_redirect() const { return m_redirect; }
		network_trust get_trust() const { return m_trust; }
		
		size_t get_chunk() const { return m_chunk; }
		size_t get_downloads() const { return m_downloads; }
		size_t get_interval() const { return m_interval; }
		size_t get_retries() const { return m_retries; }		
		size_t get_sockets() const { return m_sockets; }
		size_t get_threads() const { return m_threads; }
		size_t get_timeout() const { return m_timeout; }
		size_t get_uploads() const { return m_uploads; }
		size_t get_window() const { return m_window; }
	};
}

#endif
