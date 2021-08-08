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

#ifndef _EJA_UPLOAD_SERVICE_COMPONENT_H_
#define _EJA_UPLOAD_SERVICE_COMPONENT_H_

#include "component/callback/callback.h"
#include "component/transfer/transfer_service_component.h"

namespace eja
{
	class entity;
	class http_request;
	class http_response;
	class upload_service_component;

	enum class upload_service_state { none, handshake, upload, quit };
	typedef void (upload_service_component::*upload_service_func)(const std::shared_ptr<entity>& entity);

	class upload_service_component final : public transfer_service_component
	{
		make_factory(upload_service_component);

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<upload_service_component>(); }
		const ptr shared_from_this() const { return std::const_pointer_cast<upload_service_component>(shared_from_base<upload_service_component>()); }

		// Write
		template <typename T>
		void write(const std::shared_ptr<entity>& entity, const T& t, const upload_service_state state);
		void write(const std::shared_ptr<entity>& entity, const message_packer& packer, const upload_service_state state);

		// Parse
		void parse(const std::shared_ptr<entity>& entity, const std::shared_ptr<http_response>& response);

		// Handshake
		void handshake(const std::shared_ptr<entity>& entity);

		// Main
		void async_main(const std::shared_ptr<entity>& entity, const size_t ms) { async_wait(entity, ms, &upload_service_component::async_main); }
		void async_main(const std::shared_ptr<entity>& entity);
		void main(const std::shared_ptr<entity>& entity);

		// Quit
		void quit(const std::shared_ptr<entity>& entity);

		// Upload
		void upload(const std::shared_ptr<entity>& entity);

		// Wait
		void async_wait(const std::shared_ptr<entity>& entity, const size_t ms, upload_service_func func);
		void wait(const std::shared_ptr<entity>& entity, const timer_ptr timer, upload_service_func func);

		// Get
		template <typename T>
		std::shared_ptr<http_request> get_request(const std::shared_ptr<entity>& entity, const T& t) const;
		std::shared_ptr<http_request> get_request(const std::shared_ptr<entity>& entity, const message_packer& packer) const;

	public:
		upload_service_component() : transfer_service_component(callback::upload) { }
		upload_service_component(const size_t threads) : transfer_service_component(callback::upload, threads) { }

		// Start
		void async_start(const std::shared_ptr<entity>& entity);

		// Stop
		void async_stop(const std::shared_ptr<entity>& entity);

		// Create
		static ptr create(const size_t threads) { return std::make_shared<upload_service_component>(threads); }
	};

	// Write
	template <typename T>
	void upload_service_component::write(const std::shared_ptr<entity>& entity, const T& t, const upload_service_state state)
	{
		message_packer packer(t);

		write(entity, packer, state);
	}

	// Get
	template <typename T>
	std::shared_ptr<http_request> upload_service_component::get_request(const std::shared_ptr<entity>& entity, const T& t) const
	{
		message_packer packer(t);

		return get_request(entity, packer);
	}
}

#endif
