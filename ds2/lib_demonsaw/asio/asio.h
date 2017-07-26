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

#ifndef _EJA_ASIO_
#define _EJA_ASIO_

#include <memory>
#include <string>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>

namespace eja
{
	// Using	
	using io_service_ptr = std::shared_ptr<boost::asio::io_service>;
	using io_strand_ptr = std::shared_ptr<boost::asio::io_service::strand>;
	using io_work_ptr = std::shared_ptr<boost::asio::io_service::work>;

	using acceptor_ptr = std::shared_ptr<boost::asio::ip::tcp::acceptor>;
	using address_ptr = std::shared_ptr<boost::asio::ip::address>;
	using timer_ptr = std::shared_ptr<boost::asio::deadline_timer>;
	using endpoint_ptr = std::shared_ptr<boost::asio::ip::tcp::endpoint>;
	using socket_ptr = std::shared_ptr<boost::asio::ip::tcp::socket>;
	using streambuf_ptr = std::shared_ptr<boost::asio::streambuf>;

	using string_ptr = std::shared_ptr<std::string>;
	using string_tokenizer = boost::tokenizer<boost::char_separator<char>>;
	using thread_group_ptr = std::shared_ptr<boost::thread_group>;
}

#endif
