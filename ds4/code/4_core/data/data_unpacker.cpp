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

#include "data/data_unpacker.h"
#include "data/checksum_data.h"
#include "data/header_data.h"
#include "data/version_data.h"

#include "data/request/browse_request_data.h"
#include "data/request/chat_request_data.h"
#include "data/request/download_request_data.h"
#include "data/request/handshake_request_data.h"
#include "data/request/info_request_data.h"
#include "data/request/join_request_data.h"
#include "data/request/mute_request_data.h"
#include "data/request/ping_request_data.h"
#include "data/request/quit_request_data.h"
#include "data/request/room_request_data.h"
#include "data/request/search_request_data.h"
#include "data/request/transfer_request_data.h"
#include "data/request/tunnel_request_data.h"
#include "data/request/upload_request_data.h"

#include "data/response/browse_response_data.h"
#include "data/response/download_response_data.h"
#include "data/response/handshake_response_data.h"
#include "data/response/info_response_data.h"
#include "data/response/join_response_data.h"
#include "data/response/ping_response_data.h"
#include "data/response/room_response_data.h"
#include "data/response/search_response_data.h"
#include "data/response/transfer_response_data.h"
#include "data/response/upload_response_data.h"

#include "entity/entity.h"
#include "exception/checksum_exception.h"
#include "exception/version_exception.h"
#include "security/checksum/xxhash.h"
#include "utility/value.h"

namespace eja
{
	// Macro
	#define data_type_unpack(type, class)		\
	case type:									\
	{											\
		const auto data = class::create();		\
		obj.convert(*data);						\
		push_back(data);						\
		break;									\
	}

	// Utility
	void data_unpacker::unpack(const void* input, const size_t input_size)
	{
		// Buffer
		msgpack::unpacker unpacker;
		unpacker.reserve_buffer(input_size);
		memcpy(unpacker.buffer(), input, input_size);
		unpacker.buffer_consumed(input_size);
		const auto buffer = unpacker.nonparsed_buffer();

		// Checksum
		msgpack::object_handle handle;
		unpacker.next(handle);
		msgpack::object obj = handle.get();

		checksum_data checksum;
		obj.convert(checksum);

		const auto hash_offset = unpacker.nonparsed_buffer() - buffer;
		const auto hash_input = reinterpret_cast<const char*>(input) + hash_offset;
		const auto hash_input_size = input_size - hash_offset;
		const auto hash_value = xxhash()(hash_input, hash_input_size);

		if (checksum.get_checksum() != hash_value)
			throw checksum_exception(checksum.get_checksum());

		// Version
		unpacker.next(handle);
		obj = handle.get();

		version_data version;
		obj.convert(version);

		if ((version.get_version() < software::min_version) || (version.get_version() > software::max_version))
			throw version_exception(version.get_version());

		// Header
		unpacker.next(handle);
		obj = handle.get();

		header_data header;
		obj.convert(header);

		// Message
		for (const auto& type : header)
		{
			unpacker.next(handle);
			obj = handle.get();

			switch (type)
			{
				// Request
				data_type_unpack(data_type::browse_request, browse_request_data);
				data_type_unpack(data_type::chat_request, chat_request_data);
				data_type_unpack(data_type::download_request, download_request_data);
				data_type_unpack(data_type::handshake_request, handshake_request_data);
				data_type_unpack(data_type::info_request, info_request_data);
				data_type_unpack(data_type::join_request, join_request_data);
				data_type_unpack(data_type::mute_request, mute_request_data);
				data_type_unpack(data_type::ping_request, ping_request_data);
				data_type_unpack(data_type::quit_request, quit_request_data);
				data_type_unpack(data_type::room_request, room_request_data);
				data_type_unpack(data_type::search_request, search_request_data);
				data_type_unpack(data_type::transfer_request, transfer_request_data);
				data_type_unpack(data_type::tunnel_request, tunnel_request_data);
				data_type_unpack(data_type::upload_request, upload_request_data);

				// Response
				data_type_unpack(data_type::browse_response, browse_response_data);
				data_type_unpack(data_type::download_response, download_response_data);
				data_type_unpack(data_type::handshake_response, handshake_response_data);
				data_type_unpack(data_type::info_response, info_response_data);
				data_type_unpack(data_type::join_response, join_response_data);
				data_type_unpack(data_type::ping_response, ping_response_data);
				data_type_unpack(data_type::room_response, room_response_data);
				data_type_unpack(data_type::search_response, search_response_data);
				data_type_unpack(data_type::transfer_response, transfer_response_data);
				data_type_unpack(data_type::upload_response, upload_response_data);

				default:
				{
					assert(false);
					break;
				}
			}
		}
	}
}
