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

#ifndef _EJA_JSON_OPTION_
#define _EJA_JSON_OPTION_

#include <cstring>
#include <memory>
#include <string>

#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "object/client/client_option.h"

namespace eja
{
	class json_option : public json_value
	{
	public:
		using ptr = std::shared_ptr<json_option>;

	protected:
		json_option() { }
		json_option(const std::string& input) { parse(input); }
		json_option(const char* input) { parse(input); }

		json_option(const json_value::ptr value) : json_value(value) { }
		json_option(const json_value::ptr value, const char* key) : json_value(value, key) { }

		json_option(const Json::Value& value) : json_value(value) { }
		json_option(const Json::Value& value, const char* key) : json_value(value, key) { }

	public:
		virtual ~json_option() override { }

		// Utility
		virtual bool valid() const override { return json_value::valid() && has_browse() && has_search() && has_transfer() && has_chat() && has_message(); }
		
		bool has_browse() const { return get_value().isMember(json::type_browse) && !get_value()[json::type_browse].empty(); }
		bool has_search() const { return get_value().isMember(json::type_search) && !get_value()[json::type_search].empty(); }
		bool has_transfer() const { return get_value().isMember(json::type_transfer) && !get_value()[json::type_transfer].empty(); }
		bool has_chat() const { return get_value().isMember(json::type_chat) && !get_value()[json::type_chat].empty(); }
		bool has_message() const { return get_value().isMember(json::type_message) && !get_value()[json::type_message].empty(); }

		// Mutator
		void set(const client_option::ptr option);

		void set_browse(const bool value) { get_value()[json::type_browse] = value; }
		void set_search(const bool value) { get_value()[json::type_search] = value; }
		void set_transfer(const bool value) { get_value()[json::type_transfer] = value; }
		void set_chat(const bool value) { get_value()[json::type_chat] = value; }
		void set_message(const bool value) { get_value()[json::type_message] = value; }

		// Accessor
		bool get_browse() const { return get_value()[json::type_browse].asBool(); }
		bool get_search() const { return get_value()[json::type_search].asBool(); }
		bool get_transfer() const { return get_value()[json::type_transfer].asBool(); }
		bool get_chat() const { return get_value()[json::type_chat].asBool(); }
		bool get_message() const { return get_value()[json::type_message].asBool(); }

		// Static
		static ptr create() { return ptr(new json_option()); }
		static ptr create(const std::string& input) { return ptr(new json_option(input)); }
		static ptr create(const char* input) { return ptr(new json_option(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_option(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_option(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_option(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_option(value, key)); }
	};
}

#endif
