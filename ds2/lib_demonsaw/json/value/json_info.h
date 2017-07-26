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

#ifndef _EJA_JSON_INFO_
#define _EJA_JSON_INFO_

#include <memory>
#include <string>

#include "entity/entity.h"
#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"

namespace eja
{
	class json_info final : public json_value
	{
	private:
		std::string m_name;

	public:
		using ptr = std::shared_ptr<json_info>;

	private:
		json_info() { }
		json_info(const std::string& input) { parse(input); }
		json_info(const char* input) { parse(input); }

		json_info(const json_value::ptr value) : json_value(value) { }
		json_info(const json_value::ptr value, const char* key) : json_value(value, key) { }

		json_info(const Json::Value& value) : json_value(value) { }
		json_info(const Json::Value& value, const char* key) : json_value(value, key) { }

	public:		
		virtual ~json_info() override { }

		// HACK: These shouldn't go here!!!
		//		
		bool has_name() const { return get_value().isMember(json::name) && !get_value()[json::name].empty() && strlen(get_name()); }

		// Utility
		bool has_sessions() const { return get_value().isMember(json::sessions) && !get_value()[json::sessions].empty(); }
		bool has_clients() const { return get_value().isMember(json::clients) && !get_value()[json::clients].empty(); }
		bool has_groups() const { return get_value().isMember(json::groups) && !get_value()[json::groups].empty(); }
		bool has_message() const { return get_value().isMember(json::message) && !get_value()[json::message].empty(); }

		// Mutator
		void set(const entity::ptr entity);
		void set_sessions(const size_t size) { get_value()[json::sessions] = static_cast<Json::Value::UInt>(size); }
		void set_clients(const size_t size) { get_value()[json::clients] = static_cast<Json::Value::UInt>(size); }
		void set_groups(const size_t size) { get_value()[json::groups] = static_cast<Json::Value::UInt>(size); }

		// HACK: These shouldn't go here!!!
		//		
		void set_name(const std::string& name) { get_value()[json::name] = name; }
		void set_name(const char* name) { get_value()[json::name] = name; }
		
		void set_message(const std::string& message) { get_value()[json::message] = message; }
		void set_message(const char* message) { get_value()[json::message] = message; }

		// Accessor		
		size_t get_sessions() const { return static_cast<size_t>(get_value()[json::sessions].asUInt()); }
		size_t get_clients() const { return static_cast<size_t>(get_value()[json::clients].asUInt()); }
		size_t get_groups() const { return static_cast<size_t>(get_value()[json::groups].asUInt()); }

		// HACK: These shouldn't go here!!!
		//		
		const char* get_name() const { return get_value()[json::name].asCString(); }
		const char* get_message() const { return get_value()[json::message].asCString(); }

		// Static
		static ptr create() { return ptr(new json_info()); }
		static ptr create(const std::string& input) { return ptr(new json_info(input)); }
		static ptr create(const char* input) { return ptr(new json_info(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_info(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_info(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_info(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_info(value, key)); }
	};
}

#endif
