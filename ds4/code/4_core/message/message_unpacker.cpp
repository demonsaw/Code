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

#include <cassert>

#include "exception/checksum_exception.h"
#include "exception/version_exception.h"

#include "message/message_unpacker.h"
#include "message/checksum_message.h"
#include "message/header_message.h"
#include "message/version_message.h"

#include "message/request/client_request_message.h"
#include "message/request/group_request_message.h"
#include "message/request/router_request_message.h"

#include "message/response/client_response_message.h"
#include "message/response/group_response_message.h"
#include "message/response/router_response_message.h"

#include "security/checksum/xxhash.h"
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
		msgpack::object_handle handle;
		unpacker.next(handle);
		msgpack::object obj = handle.get();

		checksum_message checksum;
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

		version_message version;
		obj.convert(version);

		if ((version.get_version() < software::min_version) || (version.get_version() > software::max_version))
			throw version_exception(version.get_version());

		// Header
		unpacker.next(handle);
		obj = handle.get();

		header_message header;
		obj.convert(header);

		for (const auto& type : header)
		{
			// Message
			unpacker.next(handle);
			obj = handle.get();

			switch (type)
			{
				// Request
				message_type_unpack(message_type::client_request, client_request_message);
				message_type_unpack(message_type::group_request, group_request_message);
				message_type_unpack(message_type::router_request, router_request_message);

				// Response
				message_type_unpack(message_type::client_response, client_response_message);
				message_type_unpack(message_type::group_response, group_response_message);
				message_type_unpack(message_type::router_response, router_response_message);

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
