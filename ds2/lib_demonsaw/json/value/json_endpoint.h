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

#ifndef _EJA_JSON_ENDPOINT_
#define _EJA_JSON_ENDPOINT_

#include <memory>
#include <string>

#include "json_name.h"
#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"

namespace eja
{
	class json_endpoint : public json_name
	{
	public:
		using ptr = std::shared_ptr<json_endpoint>;

	protected:
		json_endpoint() { }
		json_endpoint(const std::string& input) { parse(input); }
		json_endpoint(const char* input) { parse(input); }

		json_endpoint(const json_value::ptr value) : json_name(value) { }
		json_endpoint(const json_value::ptr value, const char* key) : json_name(value, key) { }

		json_endpoint(const Json::Value& value) : json_name(value) { }
		json_endpoint(const Json::Value& value, const char* key) : json_name(value, key) { }

	public:
		virtual ~json_endpoint() override { }

		// Utility
		virtual bool valid() const override { return json_name::valid() && has_address() && has_port(); }

		bool has_passphrase() const { return get_value().isMember(json::passphrase) && !get_value()[json::passphrase].empty() && strlen(get_passphrase()); }
		bool has_address() const { return get_value().isMember(json::address) && !get_value()[json::address].empty() && strlen(get_address()); }
		bool has_port() const { return get_value().isMember(json::port) && !get_value()[json::port].empty() && get_port(); }

		// Mutator
		void set_passphrase(const std::string& passphrase) { get_value()[json::passphrase] = passphrase; }
		void set_passphrase(const char* passphrase) { get_value()[json::passphrase] = passphrase; }
		void set_address(const std::string& address) { get_value()[json::address] = address; }
		void set_address(const char* address) { get_value()[json::address] = address; }
		void set_port(const size_t port) { get_value()[json::port] = static_cast<Json::Value::UInt>(port); }

		// Accessor
		const char* get_passphrase() const { return get_value()[json::passphrase].asCString(); }
		const char* get_address() const { return get_value()[json::address].asCString(); }
		size_t get_port() const { return static_cast<size_t>(get_value()[json::port].asUInt()); }

		// Static
		static ptr create() { return ptr(new json_endpoint()); }
		static ptr create(const std::string& input) { return ptr(new json_endpoint(input)); }
		static ptr create(const char* input) { return ptr(new json_endpoint(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_endpoint(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_endpoint(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_endpoint(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_endpoint(value, key)); }
	};
}

#endif
