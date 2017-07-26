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
#include "data/transfer_data.h"
#include "data/version_data.h"

#include "data/request/browse_request_data.h"
#include "data/request/chat_request_data.h"
#include "data/request/file_request_data.h"
#include "data/request/search_request_data.h"

#include "data/response/browse_response_data.h"
//#include "data/response/chat_response_data.h"
#include "data/response/file_response_data.h"
#include "data/response/search_response_data.h"

#include "entity/entity.h"
#include "security/checksum/crc.h"
#include "security/filter/lz4.h"
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

	// Operator
	data_unpacker& data_unpacker::operator=(const data_unpacker& unpacker)
	{
		if (this != &unpacker)
		{
			entity_type::operator=(unpacker);
			data_list::operator=(unpacker);

			m_version = unpacker.m_version;
		}

		return *this;
	}

	// Interface
	void data_unpacker::clear()
	{
		data_list::clear();

		m_version = 0;
	}

	// Utility
	void data_unpacker::unpack(const void* input, const size_t input_size)
	{
		// Compression
		const auto decompressed = lz4::decode(input, input_size);

		// Buffer
		msgpack::unpacker unpacker;
		unpacker.reserve_buffer(decompressed.size());
		memcpy(unpacker.buffer(), decompressed.data(), decompressed.size());
		unpacker.buffer_consumed(decompressed.size());
		const auto buffer = unpacker.nonparsed_buffer();

		// Checksum		
		msgpack::unpacked unpacked;
		unpacker.next(&unpacked);
		msgpack::object obj = unpacked.get();

		checksum_data checksum;
		obj.convert(checksum);		

		const auto crc_offset = unpacker.nonparsed_buffer() - buffer;
		const auto crc_input = reinterpret_cast<const char*>(decompressed.data()) + crc_offset;
		const auto crc_input_size = decompressed.size() - crc_offset;				
		const auto crc_value = static_cast<u32>(crc32()(crc_input, crc_input_size));

		if (checksum.get_checksum() != crc_value)
			throw std::runtime_error("Invalid checksum");

		// Version
		unpacker.next(&unpacked);
		obj = unpacked.get();

		version_data version;
		obj.convert(version);
		m_version = version.get_version();

		if (version.get_version() > version::msgpack)
			throw std::runtime_error("Invalid protocol version");

		// Header
		unpacker.next(&unpacked);
		obj = unpacked.get();

		header_data header;
		obj.convert(header);

		// Message
		for (const auto& type : header)
		{			
			unpacker.next(&unpacked);
			obj = unpacked.get();

			switch (type)
			{
				// Request/Response
				data_type_unpack(data_type::transfer, transfer_data);

				// Request
				data_type_unpack(data_type::browse_request, browse_request_data); 
				data_type_unpack(data_type::chat_request, chat_request_data);				
				data_type_unpack(data_type::file_request, file_request_data);
				data_type_unpack(data_type::search_request, search_request_data);				

				// Response
				data_type_unpack(data_type::browse_response, browse_response_data);
				//data_type_unpack(data_type::chat_response, chat_response_data);
				data_type_unpack(data_type::file_response, file_response_data);
				data_type_unpack(data_type::search_response, search_response_data);				

				default:
				{
					assert(false);
					break;
				}
			}
		}
	}
}
