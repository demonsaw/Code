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

// TODO: Eventually enable HTTPS
//
//#define BOOST_NETWORK_ENABLE_HTTPS 

#if _WIN32 && DEBUG
#pragma comment(lib, "cppnetlib.lib")
#endif

#include <exception>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/network.hpp>
#include <boost/network/uri.hpp>
#include <boost/network/protocol/http/client.hpp>

#include "client_security_component.h"
#include "component/client/client_option_component.h"
#include "component/group/group_component.h"
#include "component/group/group_cipher_component.h"
#include "component/group/group_option_component.h"
#include "component/group/group_security_component.h"
#include "component/group/group_status_component.h"
#include "component/io/file_component.h"
#include "http/http.h"
#include "security/hash.h"
#include "security/hmac.h"
#include "security/pbkdf.h"
#include "utility/default_value.h"

namespace eja
{
	// Interface
	void client_security_component::update()
	{
		component::update();

		std::string data;
		std::string group_id = default_security::signature;

		const auto owner = get_entity();
		const auto group_vector = owner->get<group_entity_vector_component>();
		if (!group_vector)
			throw std::runtime_error("Invalid component");

		for (const auto& entity : group_vector->copy())
		{
			// Security
			const auto status = entity->get<group_status_component>();
			if (!status)
				throw std::runtime_error("Invalid component");

			const auto cipher = entity->get<group_cipher_component>();
			if (!cipher)
				throw std::runtime_error("Invalid component");

			const auto option = entity->get<group_option_component>();
			if (!option)
				throw std::runtime_error("Invalid component");

			const auto security = entity->get<group_security_component>();
			if (!security)
				throw std::runtime_error("Invalid component");

			if (!security->modified())
			{
				// Group
				if (option->enabled())
				{
					group_id = cipher->encrypt(group_id);
					status->set_status(eja::status::success);
				}
				
				continue;
			}

			// Option						
			if (option->disabled())
			{
				cipher->clear();				
				status->set_status(eja::status::none);
				security->set_modified(false);
				continue;
			}

			// File
			const auto file = entity->get<file_component>();
			if (!file)
				throw std::runtime_error("Invalid component");

			file->set_size(0);

			if (!file->has_path())
			{
				status->set_status(eja::status::warning);
				const auto str = boost::str(boost::format("Invalid path: %s") % file->get_path());
				log(str);

				continue;
			}

			status->set_status(eja::status::info);

			if (file->exists())
			{
				// System
				file->set_size();
				
				const auto bytes = file->read(data);
				if (!bytes)
				{
					status->set_status(eja::status::warning);
					const auto str = boost::str(boost::format("Invalid file: %s") % file->get_path());
					log(str);

					continue;
				}
			}
			else
			{
				try
				{
					// URI
					boost::network::uri::uri uri(file->get_path());
					if (!uri.is_valid())
					{
						status->set_status(eja::status::error);
						const auto str = boost::str(boost::format("Invalid URI: %s") % file->get_path());
						log(str);

						continue;
					}

					// Request					
					boost::network::http::client::request http_request(uri);
					http_request << boost::network::header("Connection", "close");

					// Client
					boost::network::http::client::options options;
					const auto option = entity->get<client_option_component>();
					if (!option)
						throw std::runtime_error("Invalid component");

					options.cache_resolved(true).follow_redirects(true).timeout(option->get_socket_timeout() / 1000);
					boost::network::http::client client(options);

					// Response
					boost::network::http::client::response http_response = client.get(http_request);

					// Redirect?
					switch (http_response.status())
					{
						case 301:
						case 302:
						case 307:
						case 308:
						{
							const auto headers = http_response.headers();
							const auto it = headers.find("Location");
							if (it != headers.end())
							{
								uri = it->second;
								http_request.uri(uri);
								http_response = client.get(http_request);
							}

							break;
						}
					}

					// Data
					data = http_response.body();
					file->set_size(data.size());

					if (data.empty())
					{
						status->set_status(eja::status::error);
						const auto str = boost::str(boost::format("Invalid data: %s") % file->get_path());
						log(str);

						continue;
					}
				}
				catch (std::exception& e)
				{
					status->set_status(eja::status::error);
					unused(e);

					throw;
				}
				catch (...)
				{
					status->set_status(eja::status::error);

					throw;
				}
			}

			// Entropy
			auto input_size = data.size();
			const auto entropy = security->get_entropy();
			if (entropy < 100.0)
			{
				const auto percent = entropy / 100.0;
				input_size = static_cast<size_t>(input_size * percent);
			}

			// Cipher
			const auto type = static_cast<group_cipher_type>(security->get_cipher_type());
			switch (type)
			{
				case group_cipher_type::aes:
				{
					cipher->set_cipher<aes>();
					break;
				}
				case group_cipher_type::mars:
				{
					cipher->set_cipher<mars>();
					break;
				}
				case group_cipher_type::rc6:
				{
					cipher->set_cipher<rc6>();
					break;
				}
				case group_cipher_type::serpent:
				{
					cipher->set_cipher<serpent>();
					break;
				}
				case group_cipher_type::twofish:
				{
					cipher->set_cipher<twofish>();
					break;
				}
				default:
				{
					cipher->set_cipher();
					break;
				}
			}

			const auto input = reinterpret_cast<const byte*>(data.c_str());
			cipher->set_data(data);

			const auto key = get_key(security, input, input_size, (security->get_key_size() >> 3));
			cipher->set_key(key);
			
			// Group
			group_id = cipher->encrypt(group_id);			

			status->set_status(eja::status::success);
			security->set_modified(false);
		}

		// Group
		const auto group = owner->get<group_component>();
		if (!group)
			throw std::runtime_error("Invalid component");

		if (group_id != default_security::signature)
		{			
			const auto group_id_encoded = base64::encode(group_id);
			group->set_id(group_id_encoded);		
		}
		else
		{
			group->clear();
		}
	}

	// Utility
	bool client_security_component::valid() const
	{
		const auto owner = get_entity();
		const auto group_vector = owner->get<group_entity_vector_component>();
		if (!group_vector)
			throw std::runtime_error("Invalid component");

		for (const auto& entity : group_vector->copy())
		{
			const auto option = entity->get<group_option_component>();
			if (!option)
				throw std::runtime_error("Invalid component");

			if (option->enabled())
			{
				const auto security = entity->get<group_security_component>();
				if (!security)
					throw std::runtime_error("Invalid component");

				if (security->invalid())
					return false;

				const auto cipher = entity->get<group_cipher_component>();
				if (!cipher)
					throw std::runtime_error("Invalid component");

				if (cipher->invalid())
					return false;
			}
		}

		return true;
	}

	bool client_security_component::modified() const
	{
		const auto owner = get_entity();
		const auto group_vector = owner->get<group_entity_vector_component>();
		if (!group_vector)
			throw std::runtime_error("Invalid component");

		for (const auto& entity : group_vector->copy())
		{
			const auto security = entity->get<group_security_component>();
			if (!security)
				throw std::runtime_error("Invalid component");

			if (security->modified())
				return true;
		}

		return false;
	}

	bool client_security_component::empty() const
	{
		const auto owner = get_entity();
		const auto group_vector = owner->get<group_entity_vector_component>();
		if (!group_vector)
			throw std::runtime_error("Invalid component");

		for (const auto& entity : group_vector->copy())
		{
			const auto cipher = entity->get<group_cipher_component>();
			if (!cipher)
				throw std::runtime_error("Invalid component");

			if (cipher->valid())
				return false;
		}

		return true;
	}

	std::string client_security_component::encrypt(const byte* input, const size_t input_size) const
	{
		std::string output(reinterpret_cast<const char*>(input), input_size);

		const auto owner = get_entity();
		const auto group_vector = owner->get<group_entity_vector_component>();	
		if (!group_vector)
			throw std::runtime_error("Invalid component");

		const auto group_vector_copy = group_vector->copy();

		for (auto it = group_vector_copy.begin(); it != group_vector_copy.end(); ++it)
		{
			const auto& entity = *it;

			const auto cipher = entity->get<group_cipher_component>();
			if (!cipher)
				throw std::runtime_error("Invalid component");

			if (cipher->valid())
				output = cipher->encrypt(output);
		}

		return output;
	}

	std::string client_security_component::decrypt(const byte* input, const size_t input_size) const
	{
		std::string output(reinterpret_cast<const char*>(input), input_size);

		const auto owner = get_entity();
		const auto group_vector = owner->get<group_entity_vector_component>();
		if (!group_vector)
			throw std::runtime_error("Invalid component");

		const auto group_vector_copy = group_vector->copy();
		
		for (auto it = group_vector_copy.rbegin(); it != group_vector_copy.rend(); ++it)
		{
			const auto& entity = *it;

			const auto cipher = entity->get<group_cipher_component>();
			if (!cipher)
				throw std::runtime_error("Invalid component"); 
			
			if (cipher->valid())
				output = cipher->decrypt(output);
		}

		return output;
	}

	// Accessor
	std::string client_security_component::get_key(const group_security_component::ptr security, const byte* input, const size_t input_size, const size_t output_size) const
	{
		// Hash
		if (security->has_hash())
		{
			const auto hash = security->get_hash();
			hash->set_salt(security->get_salt());

			std::string output;
			output = hash->compute(input, input_size);
			output.resize(output_size);
			return output;
		}

		// PBKDF
		if (security->has_pbkdf())
		{
			const auto pbkdf = security->get_pbkdf();
			pbkdf->set_salt(security->get_salt());
			pbkdf->set_iterations(security->get_iterations());
			
			std::string output;
			output = pbkdf->compute(input, input_size, output_size);
			return output;
		}

		// NOTE: This is legacy code and should never be used!
		// It is here for worst case scenario: xor'd key is better than no key
		assert(false);

		// XOR
		std::string output;
		output.resize(output_size);
		assert(security->has_xor());

		if (output_size == default_security::seed_size)
		{
			assert(output_size == aes::get_default_size());
			output.assign(reinterpret_cast<const char*>(default_security::seed), output_size);
		}

		for (size_t i = 0; i < (input_size - (input_size % output_size)); i += output_size)
		{
			for (size_t j = 0; j < output_size; ++j)
				output[j] ^= input[i + j];
		}

		return output;
	}
}
