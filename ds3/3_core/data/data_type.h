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

#ifndef _EJA_DATA_TYPE_H_
#define _EJA_DATA_TYPE_H_

#include <list>

#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Macro
	#define make_data_type(T)										\
	T,																\
	T ## _request,													\
	T ## _response,													\
	T ## _callback,													\
	T ## _unused

	// Using
	enum class data_type;
	using data_type_list = std::list<data_type>;

	// Type
	enum class data_type
	{
		none,
		padding = 9,

		make_data_type(empty),
		make_data_type(browse),
		make_data_type(chat),
		make_data_type(client),
		make_data_type(file),
		make_data_type(group),
		make_data_type(handshake),
		make_data_type(join),
		make_data_type(ping),
		make_data_type(quit),
		make_data_type(router), 
		make_data_type(search),
		make_data_type(session),
		make_data_type(transfer),
		make_data_type(tunnel),
	};
}

MSGPACK_ADD_ENUM(eja::data_type);

#endif
