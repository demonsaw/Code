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

#ifndef _EJA_JSON_ARRAY_
#define _EJA_JSON_ARRAY_

#include <cassert>
#include <memory>
#include <string>

#include "json.h"
#include "json_lib.h"
#include "json_value.h"

namespace eja
{
	class json_array : public json_value
	{
	public:
		using ptr = std::shared_ptr<json_array>;

	protected:
		json_array() { init(); }
		json_array(const std::string& input) { parse(input); }
		json_array(const char* input) { parse(input); }

		json_array(const json_value::ptr value) : json_value(value) { init(); }
		json_array(const json_value::ptr value, const char* key) : json_value(value, key) { init(); }

		json_array(const Json::Value& value) : json_value(value) { init(); }
		json_array(const Json::Value& value, const char* key) : json_value(value, key) { init(); }

		// Interface
		void init();

	public:		
		virtual ~json_array() override { }

		// Iterator
		Json::Value::const_iterator begin() const { return get_value().begin(); }
		Json::Value::iterator begin() { return get_value().begin(); }

		Json::Value::const_iterator end() const { return get_value().end(); }
		Json::Value::iterator end() { return get_value().end(); }

		// Interface
		template <typename T = json_value>
		typename T::ptr add();

		// Static
		static ptr create() { return ptr(new json_array()); }
		static ptr create(const std::string& input) { return ptr(new json_array(input)); }
		static ptr create(const char* input) { return ptr(new json_array(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_array(value)); }
		static ptr create(const Json::Value& value) { return ptr(new json_array(value)); }
	};

	// Interface
	template <typename T>
	typename T::ptr json_array::add()
	{
		assert(get_value().isArray());

		const auto& value = get_value().append(Json::nullValue);
		const auto json = json_value::create(value);
		return T::create(json);
	}
}

#endif
