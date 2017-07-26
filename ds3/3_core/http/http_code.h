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

#ifndef _EJA_HTTP_CODE_H_
#define _EJA_HTTP_CODE_H_

#include <iostream>

#include "system/type.h"

namespace eja
{
	enum class http_code
	{
		none,
		ok = 200,
		see_other = 303,				// The response to the request can be found under another URI using a GET method.
		unauthorized = 401,				// Similar to 403 Forbidden, but specifically for use when authentication is required and has failed or has not yet been provided. 
		not_found = 404,				// The requested resource could not be found but may be available again in the future. Subsequent requests by the client are permissible.		
		internal_service_error = 500,	// The server is currently unavailable (because it is overloaded or down for maintenance). Generally, this is a temporary state.
		not_implemented = 501,			// The server either does not recognize the request method, or it lacks the ability to fulfill the request. Usually this implies future availability.
	};

	inline std::ostream& operator<<(std::ostream& os, const http_code& code) { return os << static_cast<size_t>(code); }
}

#endif
