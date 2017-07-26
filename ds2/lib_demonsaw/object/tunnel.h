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

#ifndef _EJA_SOCKET_
#define _EJA_SOCKET_

#include <cassert>
#include <memory>
#include <string>

#include "object.h"
#include "http/http_socket.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class tunnel : public object
	{
	private:
		http_socket::ptr m_socket = nullptr;

	public:
		using ptr = std::shared_ptr<tunnel>;

	public:
		tunnel() { }
		tunnel(const eja::tunnel& tunnel) : object(tunnel), m_socket(tunnel.m_socket) { }
		tunnel(const std::string& id) : object(id) { }
		tunnel(const char* id) : object(id) { }
		tunnel(const http_socket::ptr socket) : m_socket(socket) { assert(socket->valid()); }
		virtual ~tunnel() override { close(); }

		// Interface
		virtual void clear() override { m_socket = nullptr; }

		// Utility
		void close();
		virtual bool valid() const override { return object::valid() && has_socket() && m_socket->valid(); }
		bool has_socket() const { return m_socket != nullptr; }

		// Mutator
		void set_socket(const http_socket::ptr socket) { m_socket = socket; }

		// Accessor
		const http_socket::ptr get_socket() const { return m_socket; }
		http_socket::ptr get_socket() { return m_socket; }

		// Static
		static ptr create() { return std::make_shared<tunnel>(); }		
		static ptr create(const eja::tunnel& tunnel) { return std::make_shared<eja::tunnel>(tunnel); }
		static ptr create(const tunnel::ptr tunnel) { return std::make_shared<eja::tunnel>(*tunnel); }
		static ptr create(const std::string& id) { return std::make_shared<tunnel>(id); }
		static ptr create(const char* id) { return std::make_shared<tunnel>(id); }
		static ptr create(const http_socket::ptr socket) { return std::make_shared<tunnel>(socket); }
	};

	// Container
	derived_type(tunnel_list, mutex_list<tunnel>);
	derived_type(tunnel_map, mutex_map<std::string, tunnel>);
	derived_type(tunnel_queue, mutex_queue<tunnel>);
	derived_type(tunnel_vector, mutex_vector<tunnel>);
}

#endif
