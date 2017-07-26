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

#include <boost/detail/endian.hpp>
#include <boost/endian/conversion.hpp>

#include "data/checksum_data.h"
#include "data/data_packer.h"
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

namespace eja
{
	// Macro
	#define data_type_pack(type, class)									\
	case type:															\
	{																	\
		const auto request = std::static_pointer_cast<class>(data);		\
		packer.pack(request);											\
		break;															\
	}

	// Operator
	data_packer& data_packer::operator=(const data_packer& packer)
	{
		if (this != &packer)
		{
			entity_type::operator=(packer);
			data_list::operator=(packer);
		}

		return *this;
	}

	// Utility
	std::string data_packer::pack(const size_t size /*= 128*/) const
	{
		// Buffer
		std::string buffer;
		buffer.reserve(size);
		msgpack::packer<std::string> packer(buffer);

		// Checksum		
		checksum_data checksum;
		packer.pack(checksum);
		const auto checksum_size = buffer.size();

		// Version
		version_data version;
		packer.pack(version);

		// Header
		header_data header;

		for (const auto& data : *this)
			header.push_back(data->get_type());

		packer.pack(header);

		// Data
		for (const auto& data : *this)
		{
			switch (data->get_type())
			{
				// Request/Response
				data_type_pack(data_type::transfer, transfer_data);

				// Request
				data_type_pack(data_type::browse_request, browse_request_data);
				data_type_pack(data_type::chat_request, chat_request_data);
				data_type_pack(data_type::file_request, file_request_data);
				data_type_pack(data_type::search_request, search_request_data);				

				// Response
				data_type_pack(data_type::browse_response, browse_response_data);
				//data_type_pack(data_type::chat_response, chat_response_data); 
				data_type_pack(data_type::file_response, file_response_data);
				data_type_pack(data_type::search_response, search_response_data);				

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

		// Compression
		return lz4::encode(buffer);
	}
}
