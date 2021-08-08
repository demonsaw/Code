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

#ifndef _EJA_UI_COMPONENT_H_
#define _EJA_UI_COMPONENT_H_

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class ui_component : public component
	{
		make_factory(ui_component);

	private:
		bool m_browse = false;
		bool m_chat = false;
		bool m_client = false;
		bool m_download = false;
		bool m_error = false;
		bool m_group = false;
		bool m_help = false;
		bool m_message = false;
		bool m_option = false;
		bool m_queue = false;
		bool m_search = false;
		bool m_session = false;
		bool m_share = false;
		bool m_status = false;
		bool m_transfer = false;
		bool m_upload = false;

	public:
		ui_component();
		ui_component(const ui_component& comp) : component(comp) { }

		// Operator
		ui_component& operator=(const ui_component& comp);

		// Interface
		virtual void clear() override;

		// Has
		bool has_browse() const { return m_browse; }
		bool has_chat() const { return m_chat; }
		bool has_client() const { return m_client; }		
		bool has_download() const { return m_download; }
		bool has_error() const { return m_error; }
		bool has_group() const { return m_group; }
		bool has_help() const { return m_help; }
		bool has_message() const { return m_message; }
		bool has_option() const { return m_option; }
		bool has_queue() const { return m_queue; }
		bool has_search() const { return m_search; }
		bool has_session() const { return m_session; }
		bool has_share() const { return m_share; }
		bool has_status() const { return m_status; }
		bool has_transfer() const { return m_transfer; }
		bool has_upload() const { return m_upload; }

		// Set
		void set_browse(const bool browse) { m_browse = browse; }
		void set_chat(const bool chat) { m_chat = chat; }
		void set_client(const bool client) { m_client = client; }		
		void set_download(const bool download) { m_download = download; }
		void set_error(const bool error) { m_error = error; }
		void set_group(const bool group) { m_group = group; }
		void set_help(const bool help) { m_help = help; }
		void set_message(const bool message) { m_message = message; }
		void set_option(const bool option) { m_option = option; }
		void set_queue(const bool queue) { m_queue = queue; }
		void set_search(const bool search) { m_search = search; }
		void set_session(const bool session) { m_session = session; }
		void set_share(const bool share) { m_share = share; }
		void set_status(const bool status) { m_status = status; }
		void set_transfer(const bool transfer) { m_transfer = transfer; }
		void set_upload(const bool upload) { m_upload = upload; }
	};
}

#endif
