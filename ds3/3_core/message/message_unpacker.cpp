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
#include "message/message_unpacker.h"
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
#include "utility/value.h"

namespace eja
{
	// Macro
	#define message_type_unpack(type, class)	\
	case type:									\
	{											\
		const auto message = class::create();	\
		obj.convert(*message);					\
		push_back(message);						\
		break;									\
	}

	// Operator
	message_unpacker& message_unpacker::operator=(const message_unpacker& unpacker)
	{
		if (this != &unpacker)
		{
			entity_type::operator=(unpacker);
			message_list::operator=(unpacker);

			m_version = unpacker.m_version;
		}

		return *this;
	}

	// Interface
	void message_unpacker::clear()
	{
		message_list::clear();

		m_version = 0;
	}

	// Utility
	void message_unpacker::unpack(const void* input, const size_t input_size)
	{
		// Buffer
		msgpack::unpacker unpacker;
		unpacker.reserve_buffer(input_size);
		memcpy(unpacker.buffer(), input, input_size);
		unpacker.buffer_consumed(input_size);
		const auto buffer = unpacker.nonparsed_buffer();

		// Checksum		
		msgpack::unpacked unpacked;
		unpacker.next(&unpacked);
		msgpack::object obj = unpacked.get();

		checksum_message checksum;
		obj.convert(checksum);		

		const auto crc_offset = unpacker.nonparsed_buffer() - buffer;
		const auto crc_input = reinterpret_cast<const char*>(input) + crc_offset;
		const auto crc_input_size = input_size - crc_offset;
		const auto crc_value = static_cast<u32>(crc32()(crc_input, crc_input_size));

		if (checksum.get_checksum() != crc_value)
		{
			std::cout << "Expected: " << checksum.get_checksum() << " - got: " << crc_value << std::endl;
			throw std::runtime_error("Invalid checksum");
		}			

		// Version
		unpacker.next(&unpacked);
		obj = unpacked.get();

		version_message version;
		obj.convert(version);
		m_version = version.get_version();

		if (version.get_version() > version::msgpack)
			throw std::runtime_error("Invalid protocol version");

		// Header
		unpacker.next(&unpacked);
		obj = unpacked.get();

		header_message header;
		obj.convert(header);
		
		for (const auto& type : header)
		{
			// Message
			unpacker.next(&unpacked);
			obj = unpacked.get();

			switch (type)
			{
				// Request
				message_type_unpack(message_type::browse_request, browse_request_message);
				message_type_unpack(message_type::chat_request, chat_request_message);
				message_type_unpack(message_type::client_request, client_request_message);
				message_type_unpack(message_type::download_request, download_request_message);
				message_type_unpack(message_type::file_request, file_request_message);
				message_type_unpack(message_type::handshake_request, handshake_request_message);
				message_type_unpack(message_type::join_request, join_request_message);				
				message_type_unpack(message_type::quit_request, quit_request_message);
				message_type_unpack(message_type::search_request, search_request_message);
				message_type_unpack(message_type::socket_request, socket_request_message);
				message_type_unpack(message_type::tunnel_request, tunnel_request_message);
				message_type_unpack(message_type::upload_request, upload_request_message);

				// Response
				message_type_unpack(message_type::browse_response, browse_response_message);
				message_type_unpack(message_type::chat_response, chat_response_message);
				message_type_unpack(message_type::client_response, client_response_message);
				message_type_unpack(message_type::download_response, download_response_message);
				message_type_unpack(message_type::file_response, file_response_message);
				message_type_unpack(message_type::handshake_response, handshake_response_message);
				message_type_unpack(message_type::join_response, join_response_message);				
				message_type_unpack(message_type::quit_response, quit_response_message);
				message_type_unpack(message_type::search_response, search_response_message);
				message_type_unpack(message_type::socket_response, socket_response_message);
				message_type_unpack(message_type::tunnel_response, tunnel_response_message);
				message_type_unpack(message_type::upload_response, upload_response_message);

				// Callback
				message_type_unpack(message_type::browse_callback, browse_callback_message);
				message_type_unpack(message_type::file_callback, file_callback_message); 
				message_type_unpack(message_type::search_callback, search_callback_message);

				default:
				{
					assert(false);
					break;
				}
			}
		}

		assert(!empty());
	}
}
