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

#ifndef _EJA_CLIENT_FACTORY_H_
#define _EJA_CLIENT_FACTORY_H_

#include <memory>
#include <string>
#include <boost/filesystem.hpp>

namespace eja
{
	class entity;

	class client_factory final
	{
	private:
		client_factory() = delete;
		~client_factory() = delete;

	public:
		// Client/Router
		static std::shared_ptr<entity> create_client(const std::shared_ptr<entity>& owner = nullptr);
		static std::shared_ptr<entity> create_router(const std::shared_ptr<entity>& owner);

		static std::shared_ptr<entity> create_browse_file(const std::shared_ptr<entity>& owner, const std::shared_ptr<entity>& parent);
		static std::shared_ptr<entity> create_browse_folder(const std::shared_ptr<entity>& owner, const std::shared_ptr<entity>& parent);

		static std::shared_ptr<entity> create_chat(const std::shared_ptr<entity>& owner);
		static std::shared_ptr<entity> create_chat(const std::shared_ptr<entity>& owner, std::string&& text);
		static std::shared_ptr<entity> create_chat(const std::shared_ptr<entity>& owner, const std::string& text);

		static std::shared_ptr<entity> create_file(const std::shared_ptr<entity>& owner);
		static std::shared_ptr<entity> create_folder(const std::shared_ptr<entity>& owner);
		
		static std::shared_ptr<entity> create_group(const std::shared_ptr<entity>& owner);
		static std::shared_ptr<entity> create_group_option(const std::shared_ptr<entity>& owner);

		static std::shared_ptr<entity> create_motd(const std::shared_ptr<entity>& owner);
		static std::shared_ptr<entity> create_room(const std::shared_ptr<entity>& owner);

		static std::shared_ptr<entity> create_search(const std::shared_ptr<entity>& owner, const std::string& text = "");
		static std::shared_ptr<entity> create_search_file(const std::shared_ptr<entity>& owner);
		static std::shared_ptr<entity> create_search_folder(const std::shared_ptr<entity>& owner);

		static std::shared_ptr<entity> create_download(const std::shared_ptr<entity>& owner, const std::shared_ptr<entity>& file_entity);
		static std::shared_ptr<entity> create_download(const std::shared_ptr<entity>& owner, const std::shared_ptr<entity>& file_entity, const boost::filesystem::path& parent);
		static std::shared_ptr<entity> create_download(const std::shared_ptr<entity>& owner, const std::shared_ptr<entity>& file_entity, const std::string& parent) { return create_download(owner, file_entity, boost::filesystem::path(parent)); }		
		static std::shared_ptr<entity> create_download_router(const std::shared_ptr<entity>& owner, const std::shared_ptr<entity>& router_entity);

		static std::shared_ptr<entity> create_upload(const std::shared_ptr<entity>& owner, const std::shared_ptr<entity>& file_entity);
	};
}

#endif
