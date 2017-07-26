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

#ifndef _EJA_HANDSHAKE_REQUEST_MESSAGE_
#define _EJA_HANDSHAKE_REQUEST_MESSAGE_

#include <memory>

#include "json/json.h"
#include "json/json_message.h"
#include "json/json_value.h"
#include "json/value/json_key.h"

namespace eja
{
	class handshake_request_message final : public json_message
	{
	public:
		using ptr = std::shared_ptr<handshake_request_message>;

	private:
		handshake_request_message() : json_message(json_type::handshake, json_action::request) { }
		handshake_request_message(const json_message::ptr message) : json_message(message, json_type::handshake, json_action::request) { }
		handshake_request_message(const json_value::ptr value) : json_message(value, json_type::handshake, json_action::request) { }

	public:
		virtual ~handshake_request_message() override { }

		// Utility
		virtual bool valid() const override { return json_message::valid() && has_key(); }
		bool has_key() const { return get_value().isMember(json::key) && !get_value()[json::key].empty(); }

		// Accessor
		json_key::ptr get_key() { return json_key::create(shared_from_this(), json::key); }

		// Static
		static ptr create() { return ptr(new handshake_request_message()); }
		static ptr create(const json_message::ptr message) { return ptr(new handshake_request_message(message)); }
		static ptr create(const json_value::ptr value) { return ptr(new handshake_request_message(value)); }
	};
}

#endif
