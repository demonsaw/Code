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

#ifndef _EJA_ENTITY_FACTORY_H_
#define _EJA_ENTITY_FACTORY_H_

#include <memory>
#include <string>

#include "component/status/statusbar_component.h"

namespace eja
{
	class entity;

	class entity_factory final
	{
	private:
		entity_factory() = delete;

	public:
		// Client
		static std::shared_ptr<entity> create_client(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_client_browse(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_client_chat(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_client_client(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_client_download(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_client_message(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_client_search(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_client_transfer(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_client_upload(const std::shared_ptr<entity> parent = nullptr);

		// Message
		static std::shared_ptr<entity> create_message(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_message_client(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_message_transfer(const std::shared_ptr<entity> parent = nullptr);

		// Transfer
		static std::shared_ptr<entity> create_transfer_download(const std::shared_ptr<entity> parent = nullptr);

		// Object
		static std::shared_ptr<entity> create_error(const std::string& text, const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_file(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_folder(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_group(const std::shared_ptr<entity> parent = nullptr);
		static std::shared_ptr<entity> create_statusbar(const statusbar& bar, const std::shared_ptr<entity> parent = nullptr);
	};
}

#endif
