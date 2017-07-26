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

#include "entity/entity.h"
#include "message/checksum_message.h"
#include "message/header_message.h"
#include "message/message_packer.h"
#include "message/version_message.h"

#include "message/callback/browse_callback_message.h"
#include "message/callback/search_callback_message.h"
#include "message/callback/file_callback_message.h"

#include "message/request/browse_request_message.h"
#include "message/request/chat_request_message.h"
#include "message/request/client_request_message.h"
#include "message/request/download_request_message.h"
#include "message/request/file_request_message.h"
#include "message/request/handshake_request_message.h"
#include "message/request/join_request_message.h"
#include "message/request/quit_request_message.h"
#include "message/request/search_request_message.h"
#include "message/request/socket_request_message.h"
#include "message/request/tunnel_request_message.h"
#include "message/request/upload_request_message.h"

#include "message/response/browse_response_message.h"
#include "message/response/chat_response_message.h"
#include "message/response/client_response_message.h"
#include "message/response/download_response_message.h"
#include "message/response/file_response_message.h"
#include "message/response/handshake_response_message.h"
#include "message/response/join_response_message.h"
#include "message/response/quit_response_message.h"
#include "message/response/search_response_message.h"
#include "message/response/socket_response_message.h"
#include "message/response/tunnel_response_message.h"
#include "message/response/upload_response_message.h"

#include "security/checksum/crc.h"
#include "system/type.h"

namespace eja
{
	// Macro
	#define message_type_pack(type, class)								\
	case type:															\
	{																	\
		const auto request = std::static_pointer_cast<class>(message);	\
		packer.pack(request);											\
		break;															\
	}

	// Operator
	message_packer& message_packer::operator=(const message_packer& packer)
	{
		if (this != &packer)
		{
			entity_type::operator=(packer);
			message_list::operator=(packer);
		}			

		return *this;
	}

	// Utility
	std::string message_packer::pack(const size_t size /*= 128*/) const
	{
		assert(!empty());

		// Buffer
		std::string buffer;
		buffer.reserve(size);
		msgpack::packer<std::string> packer(buffer);
		
		// Checksum
		checksum_message checksum;
		packer.pack(checksum);
		const auto checksum_size = buffer.size();

		// Version
		version_message version;
		packer.pack(version);

		// Header
		header_message header;

		for (const auto& message : *this)
			header.push_back(message->get_type());

		packer.pack(header);

		for (const auto& message : *this)
		{
			// Message
			switch (message->get_type())
			{
				// Request
				message_type_pack(message_type::browse_request, browse_request_message);
				message_type_pack(message_type::chat_request, chat_request_message);
				message_type_pack(message_type::client_request, client_request_message);
				message_type_pack(message_type::download_request, download_request_message);
				message_type_pack(message_type::file_request, file_request_message);
				message_type_pack(message_type::handshake_request, handshake_request_message);
				message_type_pack(message_type::join_request, join_request_message);				
				message_type_pack(message_type::quit_request, quit_request_message);
				message_type_pack(message_type::search_request, search_request_message);
				message_type_pack(message_type::socket_request, socket_request_message);
				message_type_pack(message_type::tunnel_request, tunnel_request_message);
				message_type_pack(message_type::upload_request, upload_request_message);

				// Response
				message_type_pack(message_type::browse_response, browse_response_message);
				message_type_pack(message_type::chat_response, chat_response_message);
				message_type_pack(message_type::client_response, client_response_message);
				message_type_pack(message_type::download_response, download_response_message);
				message_type_pack(message_type::file_response, file_response_message);
				message_type_pack(message_type::handshake_response, handshake_response_message);
				message_type_pack(message_type::join_response, join_response_message);				
				message_type_pack(message_type::quit_response, quit_response_message);
				message_type_pack(message_type::search_response, search_response_message);
				message_type_pack(message_type::socket_response, socket_response_message);
				message_type_pack(message_type::tunnel_response, tunnel_response_message);
				message_type_pack(message_type::upload_response, upload_response_message);

				// Callback
				message_type_pack(message_type::browse_callback, browse_callback_message);
				message_type_pack(message_type::file_callback, file_callback_message); 
				message_type_pack(message_type::search_callback, search_callback_message);

				default:
				{
					assert(false);
					break;
				}
			}
		}

		// Checksum
		u32& crc_value = *reinterpret_cast<u32*>(const_cast<char*>(buffer.data()) + (checksum_size - sizeof(u32)));
		const auto crc_input = buffer.data() + checksum_size;
		const auto crc_input_size = buffer.size() - checksum_size;

		// Endianness
		BOOST_ENDIAN_LITTLE_ONLY(crc_value = boost::endian::endian_reverse(static_cast<u32>(crc32()(crc_input, crc_input_size)));)
		BOOST_ENDIAN_BIG_ONLY(crc_value = static_cast<u32>(crc32()(crc_input, crc_input_size));)

		return buffer;
	}
}
