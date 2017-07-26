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

#ifndef _EJA_JSON_CLIENT_
#define _EJA_JSON_CLIENT_

#include <memory>
#include <string>

#include "json_name.h"
#include "json/json.h"
#include "json/json_array.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "object/client/client.h"

namespace eja
{
	class json_client final : public json_name
	{
	public:
		using ptr = std::shared_ptr<json_client>;

	private:
		json_client() { }
		json_client(const std::string& input) { parse(input); }
		json_client(const char* input) { parse(input); }

		json_client(const json_value::ptr value) : json_name(value) { }
		json_client(const json_value::ptr value, const char* key) : json_name(value, key) { }

		json_client(const Json::Value& value) : json_name(value) { }
		json_client(const Json::Value& value, const char* key) : json_name(value, key) { }


	public:		
		virtual ~json_client() { }

		// Mutator
		void set(const client::ptr client);

		// Static
		static ptr create() { return ptr(new json_client()); }
		static ptr create(const std::string& input) { return ptr(new json_client(input)); }
		static ptr create(const char* input) { return ptr(new json_client(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_client(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_client(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_client(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_client(value, key)); }
	};

	class json_client_array final : public json_array
	{
	public:
		using ptr = std::shared_ptr<json_client_array>;

	private:
		json_client_array() { }
		json_client_array(const std::string& input) { parse(input); }
		json_client_array(const char* input) { parse(input); }

		json_client_array(const json_value::ptr value) : json_array(value) { }
		json_client_array(const json_value::ptr value, const char* key) : json_array(value, key) { }

		json_client_array(const Json::Value& value) : json_array(value) { }
		json_client_array(const Json::Value& value, const char* key) : json_array(value, key) { }

	public:		
		virtual ~json_client_array() { }

		// Interface
		void add(const client::ptr client) { json_array::add<json_client>()->set(client); }

		// Utility
		virtual bool valid() const override { return json_array::valid() && has_clients(); }
		bool has_clients() const { return get_value().isMember(json::clients) && !get_value()[json::clients].empty(); }

		// Static
		static ptr create() { return ptr(new json_client_array()); }
		static ptr create(const char* input) { return ptr(new json_client_array(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_client_array(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_client_array(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_client_array(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_client_array(value, key)); }
	};
}

#endif
