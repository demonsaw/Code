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

#ifndef _EJA_JSON_ID_
#define _EJA_JSON_ID_

#include <cstring>
#include <memory>
#include <string>

#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "utility/limit.h"

namespace eja
{
	class json_id : public json_value
	{
	public:
		using ptr = std::shared_ptr<json_id>;		

	protected:
		json_id() { }
		json_id(const std::string& input) { parse(input); }
		json_id(const char* input) { parse(input); }

		json_id(const json_value::ptr value) : json_value(value) { }
		json_id(const json_value::ptr value, const char* key) : json_value(value, key) { }

		json_id(const Json::Value& value) : json_value(value) { }
		json_id(const Json::Value& value, const char* key) : json_value(value, key) { }

	public:
		virtual ~json_id() override { }

		// Utility
		virtual bool valid() const override { return json_value::valid() && (!has_id() || (strlen(get_id()) <= limit::id)); }

		bool has_id() const { return get_value().isMember(json::id) && !get_value()[json::id].empty(); }

		// Mutator
		void set_id(const std::string& id) { get_value()[json::id] = id; }
		void set_id(const char* id) { get_value()[json::id] = id; }

		// Accessor
		const char* get_id() const { return get_value()[json::id].asCString(); }

		// Static
		static ptr create() { return ptr(new json_id()); }
		static ptr create(const std::string& input) { return ptr(new json_id(input)); }
		static ptr create(const char* input) { return ptr(new json_id(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_id(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_id(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_id(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_id(value, key)); }
	};
}

#endif
