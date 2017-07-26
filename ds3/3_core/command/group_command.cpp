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

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#include <boost/format.hpp>
#include <boost/network.hpp>
#include <boost/network/uri.hpp>
#include <boost/network/protocol/http/client.hpp>

#include "command/group_command.h"
#include "component/endpoint_component.h"
#include "component/group/group_component.h"
#include "component/group/group_security_component.h"
#include "component/status/status_component.h"
#include "entity/entity.h"
#include "http/http_header.h"
#include "security/cipher/cipher.h"
#include "security/hash/sha.h"
#include "utility/value.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Client
	void client_group_command::execute(const group_security_list_component::ptr group_security_list)
	{
		// Group
		const auto owner = get_entity();
		if (!owner)
			return;

		const auto group = owner->get<group_component>();
		group->clear();

		// Security
		std::string group_id;

		for (const auto& entity : *group_security_list)
		{
			// Entropy?
			const auto security = entity->get<group_security_component>();
			if (!security->has_entropy())
				entity->disable();

			// Enabled?
			const auto status = entity->get<status_component>();
			if (entity->disabled())
			{
				status->set_status(eja::status::none);
				continue;
			}

			try
			{
				// Entropy
				status->set_status(eja::status::info);

				const auto& entropy = security->get_entropy();

				// File
				if (file_util::exists(entropy))
				{
					// Type
					security->set_type(group_security_type::file);

					// Data
					boost::filesystem::path path(entropy);
					/*const*/ auto data = file_util::read(path);

					const size_t size = (security->get_percent() / 100.0) * data.size();
					if (size < data.size())
						data.resize(size);

					security->set_data(std::move(data));
					security->set_size(size);

					// Empty?
					if (!security->has_data())
					{
						status->set_status(eja::status::error);

						const auto str = boost::str(boost::format("Invalid file: %s") % entropy);
						throw std::runtime_error(str);
					}
				}
				else
				{
					// URI
					boost::network::uri::uri uri(entropy);

					if (uri.is_valid())
					{
						// Type
						security->set_type(group_security_type::url);

						// Request					
						boost::network::http::client::request http_request(uri);
						http_request << boost::network::header("Connection", "close");

						// Client						
						boost::network::http::client::options options;
						options.cache_resolved(true).follow_redirects(true).timeout(milliseconds::socket / 1000);
						boost::network::http::client http_client(options);

						// Response
						boost::network::http::client::response http_response = http_client.get(http_request);
						const auto http_status = http_response.status();

						// Redirect?
						if ((http_status >= 301) && (http_status <= 308))
						{
							const auto& headers = http_response.headers();
							const auto it = headers.find(http_header::location);
							if (it != headers.end())
							{
								uri = it->second;
								http_request.uri(uri);
								http_response = http_client.get(http_request);
							}
						}

						// Data
						if (http_status == 200)
						{
							/*const*/ auto data = http_response.body();

							const size_t size = (security->get_percent() / 100.0) * data.size();
							if (size < data.size())
								data.resize(size);

							security->set_data(std::move(data));
							security->set_size(size);
						}

						// Error
						if (!security->has_data())
						{
							status->set_status(eja::status::error);

							const auto str = boost::str(boost::format("Invalid URL: %s") % entropy);
							throw std::runtime_error(str);
						}
					}
					else
					{
						// Type
						security->set_type(group_security_type::data);

						security->set_size(entropy.size());
					}
				}

				status->set_status(eja::status::success);

				// Cipher
				const auto cipher = security->create_cipher();
				group->push_back(cipher);

				// Group								
				hmac_sha256 hmac;
				hmac.set_key(cipher->get_key());
				hmac.set_salt(security->get_cipher());
				group_id = hmac.compute(group_id);
			}
			catch (const std::exception& ex)
			{
				// Clear
				group->clear();

				status->set_status(eja::status::error);

				throw;
			}
		}

		// Group
		group->set_id(group_id);
	}
}
