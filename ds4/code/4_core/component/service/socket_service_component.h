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

#ifndef _EJA_SOCKET_SERVICE_COMPONENT_H_
#define _EJA_SOCKET_SERVICE_COMPONENT_H_

#include "component/service/service_component.h"

namespace eja
{
	class entity;
	class http_socket;

	class socket_service_component : public service_component
	{
	public:
		using ptr = std::shared_ptr<socket_service_component>;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<socket_service_component>(); }
		const ptr shared_from_this() const { return std::const_pointer_cast<socket_service_component>(shared_from_base<socket_service_component>()); }

	protected:
		socket_service_component() { }
		socket_service_component(const socket_service_component& comp) : service_component(comp) { }
		socket_service_component(const size_t threads) : service_component(threads) { }

		// Operator
		socket_service_component& operator=(const socket_service_component& comp);

		// Utility
		virtual void close() override;
		virtual void close(const std::shared_ptr<entity>& entity);

		// Add
		void add_socket(const std::shared_ptr<http_socket>& socket);
		void add_socket(const std::shared_ptr<entity>& owner, const std::shared_ptr<http_socket>& socket);

		// Remove
		void remove_socket(const std::shared_ptr<http_socket>& socket);
		void remove_socket(const std::shared_ptr<entity>& owner, const std::shared_ptr<http_socket>& socket);

		// Has
		bool has_socket() const;
		bool has_socket(const std::shared_ptr<entity>& owner) const;

		// Get
		std::shared_ptr<http_socket> get_socket(const bool create = true);
		std::shared_ptr<http_socket> get_socket(const std::shared_ptr<entity>& owner, const bool create = true);
	};
}

#endif
