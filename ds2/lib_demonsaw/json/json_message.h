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

#ifndef _EJA_JSON_MESSAGE_
#define _EJA_JSON_MESSAGE_

#include <cstring>
#include <memory>
#include <string>

#include "json.h"
#include "json_action.h"
#include "json_lib.h"
#include "json_type.h"
#include "json_value.h"

namespace eja
{
	class json_message : public json_value
	{
	public:
		using ptr = std::shared_ptr<json_message>;
		using value_ptr = std::shared_ptr<Json::Value>;

	protected:
		json_message() { }		
		json_message(const std::string& input) { parse(input); }
		json_message(const char* input) { parse(input); }
		
		json_message(const json_value::ptr value) : json_value(value, json::message) {  }
		json_message(const Json::Value& value) : json_value(value, json::message) { }

		json_message(const json_message::ptr message, const json_type& type, const json_action& action) : json_value(message) { init(type, action); }
		json_message(const json_value::ptr value, const json_type& type, const json_action& action) : json_value(value, json::message)  { init(type, action); }
		json_message(const json_type& type, const json_action& action) : json_value(json_value::create(), json::message) { init(type, action); }

		// Utility
		void init(const json_type& type, const json_action& action);

	public:		
		virtual ~json_message() override { }

		// Utility
		virtual bool valid() const override { return json_value::valid() && has_type() && has_action(); }

		bool has_type() const { return get_value().isMember(json::type) && !get_value()[json::type].empty() && (get_type() != json_type::none); }
		bool has_action() const { return get_value().isMember(json::action) && !get_value()[json::action].empty() && (get_action() != json_action::none); }
		bool has_delay() const { return get_value().isMember(json::delay) && !get_value()[json::delay].empty() && get_delay(); }
		bool has_id() const { return get_value().isMember(json::id) && !get_value()[json::id].empty() && strlen(get_id()); }
		bool has_version() const { return get_value().isMember(json::version) && !get_value()[json::version].empty() && strlen(get_version()); }

		bool is_ping() const { return get_type() == json_type::ping; }
		bool is_info() const { return get_type() == json_type::info; }
		bool is_handshake() const { return get_type() == json_type::handshake; }
		bool is_join() const { return get_type() == json_type::join; }
		bool is_tunnel() const { return get_type() == json_type::tunnel; }
		bool is_search() const { return get_type() == json_type::search; }
		bool is_group() const { return get_type() == json_type::group; }
		bool is_browse() const { return get_type() == json_type::browse; }
		bool is_transfer() const { return get_type() == json_type::transfer; }
		bool is_download() const { return get_type() == json_type::download; }
		bool is_upload() const { return get_type() == json_type::upload; }
		bool is_quit() const { return get_type() == json_type::quit; }
		bool is_chat() const { return get_type() == json_type::chat; }

		bool is_request() const { return get_action() == json_action::request; }
		bool is_response() const { return get_action() == json_action::response; }

		// Mutator
		void set_type(const json_type& type) { get_value()[json::type] = static_cast<Json::Value::UInt>(type); }
		void set_action(const json_action& action) { get_value()[json::action] = static_cast<Json::Value::UInt>(action); }
		void set_delay(const size_t delay) { get_value()[json::delay] = static_cast<Json::Value::UInt>(delay); }
		
		void set_id(const std::string& id) { get_value()[json::id] = id; }
		void set_id(const char* id) { get_value()[json::id] = id; }

		void set_version(const std::string& version) { get_value()[json::version] = version; }
		void set_version(const char* version) { get_value()[json::version] = version; }

		// Accessor		
		json_type get_type() const { return static_cast<json_type>(get_value()[json::type].asUInt()); }
		json_action get_action() const { return static_cast<json_action>(get_value()[json::action].asUInt()); }
		size_t get_delay() const { return static_cast<size_t>(get_value()[json::delay].asUInt()); }
		const char* get_id() const { return get_value()[json::id].asCString(); }
		const char* get_version() const { return get_value()[json::version].asCString(); }

		// Static
		static ptr create() { return ptr(new json_message()); }
		static ptr create(const std::string& input) { return ptr(new json_message(input)); }
		static ptr create(const char* input) { return ptr(new json_message(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_message(value)); }
		static ptr create(const Json::Value& value) { return ptr(new json_message(value)); }
	};
}

#endif
