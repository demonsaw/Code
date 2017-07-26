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

#ifndef _EJA_CHAT_REQUEST_DATA_
#define _EJA_CHAT_REQUEST_DATA_

#include <memory>
#include <string>

#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "object/chat.h"
#include "utility/limit.h"

namespace eja
{
	class chat_request_data final : public json_value
	{
	public:
		using ptr = std::shared_ptr<chat_request_data>;

	private:
		chat_request_data() { }
		chat_request_data(const std::string& input) { parse(input); }
		chat_request_data(const char* input) { parse(input); }

		chat_request_data(const json_value::ptr value) : json_value(value) { }
		chat_request_data(const json_value::ptr value, const char* key) : json_value(value, key) { }

		chat_request_data(const Json::Value& value) : json_value(value) { }
		chat_request_data(const Json::Value& value, const char* key) : json_value(value, key) { }

	public:
		virtual ~chat_request_data() override { }

		// Utility
		virtual bool valid() const override { return json_value::valid() && has_id() && (strlen(get_id()) <= limit::id) && has_name() && (strlen(get_name()) <= limit::name) && has_text() && (strlen(get_text()) <= limit::chat::text) && has_type(); }

		bool has_id() const { return get_value().isMember(json::id) && !get_value()[json::id].empty(); }
		bool has_name() const { return get_value().isMember(json::name) && !get_value()[json::name].empty(); }
		bool has_text() const { return get_value().isMember(json::text) && !get_value()[json::text].empty(); }
		bool has_type() const { return get_value().isMember(json::type) && !get_value()[json::type].empty(); }

		// Mutator
		void set_id(const std::string& id) { get_value()[json::id] = id; }
		void set_id(const char* id) { get_value()[json::id] = id; }
		
		void set_name(const std::string& name) { get_value()[json::name] = name; }
		void set_name(const char* name) { get_value()[json::name] = name; }

		void set_text(const std::string& text) { get_value()[json::text] = text; }
		void set_text(const char* text) { get_value()[json::text] = text; }

		void set_type(const chat_type type) { get_value()[json::type] = static_cast<Json::Value::Int>(type); }

		// Accessor
		const char* get_id() const { return get_value()[json::id].asCString(); }
		const char* get_name() const { return get_value()[json::name].asCString(); }
		const char* get_text() const { return get_value()[json::text].asCString(); }
		chat_type get_type() const { return static_cast<chat_type>(get_value()[json::type].asInt()); }

		// Static
		static ptr create() { return ptr(new chat_request_data()); }
		static ptr create(const std::string& input) { return ptr(new chat_request_data(input)); }
		static ptr create(const char* input) { return ptr(new chat_request_data(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new chat_request_data(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new chat_request_data(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new chat_request_data(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new chat_request_data(value, key)); }
	};
}

#endif
