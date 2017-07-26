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

#ifndef _EJA_UPLOAD_RESPONSE_MESSAGE_
#define _EJA_UPLOAD_RESPONSE_MESSAGE_

#include <memory>

#include "json/json.h"
#include "json/json_message.h"
#include "json/json_value.h"
#include "json/value/json_chunk.h"

namespace eja
{
	class upload_response_message final : public json_message
	{
	public:
		using ptr = std::shared_ptr<upload_response_message>;

	private:
		upload_response_message() : json_message(json_type::upload, json_action::response) { }
		upload_response_message(const json_message::ptr message) : json_message(message, json_type::upload, json_action::response) { }
		upload_response_message(const json_value::ptr value) : json_message(value, json_type::upload, json_action::response) { }

	public:
		virtual ~upload_response_message() override { }

		// Utility
		bool has_chunk() const { return get_value().isMember(json::chunk) && !get_value()[json::chunk].empty(); }

		// Accessor
		json_chunk::ptr get_chunk() { return json_chunk::create(shared_from_this(), json::chunk); }

		// Static
		static ptr create() { return ptr(new upload_response_message()); }
		static ptr create(const json_message::ptr message) { return ptr(new upload_response_message(message)); }
		static ptr create(const json_value::ptr value) { return ptr(new upload_response_message(value)); }
	};
}

#endif
