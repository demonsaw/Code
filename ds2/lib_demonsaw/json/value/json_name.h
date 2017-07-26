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

#ifndef _EJA_JSON_NAME_
#define _EJA_JSON_NAME_

#include <memory>
#include <string>

#include "json_id.h"
#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "utility/limit.h"

namespace eja
{
	class json_name : public json_id
	{
	public:
		using ptr = std::shared_ptr<json_name>;

	protected:
		json_name() { }
		json_name(const std::string& input) { parse(input); }
		json_name(const char* input) { parse(input); }

		json_name(const json_value::ptr value) : json_id(value) { }
		json_name(const json_value::ptr value, const char* key) : json_id(value, key) { }

		json_name(const Json::Value& value) : json_id(value) { }
		json_name(const Json::Value& value, const char* key) : json_id(value, key) { }

	public:
		virtual ~json_name() override { }

		// Utility
		virtual bool valid() const override { return json_value::valid() && (!has_name() || (strlen(get_name()) <= limit::name)); }

		bool has_name() const { return get_value().isMember(json::name) && !get_value()[json::name].empty(); }

		// Mutator
		void set_name(const std::string& name) { get_value()[json::name] = name; }
		void set_name(const char* name) { get_value()[json::name] = name; }

		// Accessor		
		const char* get_name() const { return get_value()[json::name].asCString(); }

		// Static
		static ptr create() { return ptr(new json_name()); }
		static ptr create(const std::string& input) { return ptr(new json_name(input)); }
		static ptr create(const char* input) { return ptr(new json_name(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_name(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_name(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_name(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_name(value, key)); }
	};
}

#endif
