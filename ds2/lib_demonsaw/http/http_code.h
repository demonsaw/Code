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

#ifndef _EJA_HTTP_CODE_
#define _EJA_HTTP_CODE_

#include <iostream>
#include <string>

namespace eja
{
	enum class http_code : size_t
	{
		none = 0,
		ok = 200,

		moved = 301,				// Permanent redirect
		found = 302,				// Temporary redirect
		see_other = 303,			// Temporary redirect (GET) - HTTP 1.1

		bad_request = 400,			// The server cannot or will not process the request due to something that is perceived to be a client error.
		unauthorized = 401,			// Similar to 403 Forbidden, but specifically for use when authentication is required and has failed or has not yet been provided. 
		forbidden = 403,			// The request was a valid request, but the server is refusing to respond to it. Unlike a 401 Unauthorized response, authenticating will make no difference.
		not_found = 404,			// The requested resource could not be found but may be available again in the future. Subsequent requests by the client are permissible.
		method_not_allowed = 405,	// A request was made of a resource using a request method not supported by that resource; for example, using GET on a form which requires data to be presented via POST, or using PUT on a read-only resource.
		too_many_requests = 429,	// The user has sent too many requests in a given amount of time.
		login_timeout = 440,		// Indicates that your session has expired.
		
		internal_server_error = 500,// A generic error message, given when an unexpected condition was encountered and no more specific message is suitable.
		not_implemented = 501,		// The server either does not recognize the request method, or it lacks the ability to fulfill the request. Usually this implies future availability.
		service_unavailable = 503,	// The server is currently unavailable (because it is overloaded or down for maintenance). Generally, this is a temporary state.
	};

	extern std::ostream& operator<<(std::ostream& os, const http_code& code);
	extern std::string str(const http_code& code);
}

#endif
