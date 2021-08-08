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

#ifndef _EJA_UPLOAD_REQUEST_DATA_H_
#define _EJA_UPLOAD_REQUEST_DATA_H_

#include <memory>

#include "data/id_data.h"
#include "data/response/download_response_data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(upload_request_data, 1);

	class upload_request_data final : public id_data
	{
		make_factory(upload_request_data);

	private:
		std::shared_ptr<download_response_data> m_data;

	public:
		upload_request_data() : id_data(data_type::upload_request) { }

		// Has
		bool has_data() const { return m_data != nullptr; }

		// Set
		void set_data(const std::shared_ptr<download_response_data>& data) { m_data = data; }

		// Get
		std::shared_ptr<download_response_data> get_data() const { return m_data; }

		MSGPACK_DEFINE(MSGPACK_BASE(id_data), m_data);
	};
}

#endif
