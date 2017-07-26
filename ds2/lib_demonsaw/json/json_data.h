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

#ifndef _EJA_JSON_DATA_
#define _EJA_JSON_DATA_

#include <memory>
#include <string>

#include "json.h"
#include "json_lib.h"
#include "json_value.h"

namespace eja
{
	class json_data : public json_value
	{
	public:
		using ptr = std::shared_ptr<json_data>;

	protected:
		json_data() { }
		json_data(const std::string& input) { parse(input); }
		json_data(const char* input) { parse(input); }

		json_data(const json_value::ptr value) : json_value(value, json::data) { }
		json_data(const Json::Value& value) : json_value(value, json::data) { }

	public:		
		virtual ~json_data() override { }

		// Mutator
		void set(const json_data::ptr data) { get_value() = data->get(); }
		void set(const std::string& data) { get_value() = data; }
		void set(const char* data) { get_value() = data; }

		// Accessor
		const char* get() const { return get_value().asCString(); }

		// Static
		static ptr create() { return ptr(new json_data()); }
		static ptr create(const std::string& input) { return ptr(new json_data(input)); }
		static ptr create(const char* input) { return ptr(new json_data(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_data(value)); }
		static ptr create(const Json::Value& value) { return ptr(new json_data(value)); }
	};
}

#endif
