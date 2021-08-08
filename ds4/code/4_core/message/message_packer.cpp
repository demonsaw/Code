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

#include "message/message_buffer.h"
#include "message/message_packer.h"
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
	message_packer& message_packer::operator=(const message_packer& comp)
	{
		if (this != &comp)
			message_list::operator=(comp);

		return *this;
	}

	// Utility
	std::string message_packer::pack(const size_t size /*= 128*/) const
	{
		assert(!empty());

		// Buffer
		message_buffer buffer(size);
		msgpack::packer<message_buffer> packer(buffer);

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
				message_type_pack(message_type::client_request, client_request_message);
				message_type_pack(message_type::group_request, group_request_message);
				message_type_pack(message_type::router_request, router_request_message);

				// Response
				message_type_pack(message_type::client_response, client_response_message);
				message_type_pack(message_type::group_response, group_response_message);
				message_type_pack(message_type::router_response, router_response_message);

				default:
				{
					assert(false);
					break;
				}
			}
		}

		// Checksum
		u32& hash_value = *reinterpret_cast<u32*>(const_cast<char*>(buffer.data()) + (checksum_size - sizeof(u32)));
		const auto hash_input = buffer.data() + checksum_size;
		const auto hash_input_size = buffer.size() - checksum_size;

		// Endianness
		BOOST_ENDIAN_LITTLE_ONLY(hash_value = boost::endian::endian_reverse(xxhash()(hash_input, hash_input_size)));
		BOOST_ENDIAN_BIG_ONLY(hash_value = xxhash()(hash_input, hash_input_size));

		return buffer;
	}
}
