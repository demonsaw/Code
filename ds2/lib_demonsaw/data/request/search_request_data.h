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

#ifndef _EJA_SEARCH_REQUEST_DATA_
#define _EJA_SEARCH_REQUEST_DATA_

#include <memory>
#include <string>

#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "object/io/file.h"
#include "utility/limit.h"

namespace eja
{
	class search_request_data final : public json_value
	{
	public:
		using ptr = std::shared_ptr<search_request_data>;

	private:
		search_request_data() { }
		search_request_data(const std::string& input) { parse(input); }
		search_request_data(const char* input) { parse(input); }

		search_request_data(const json_value::ptr value) : json_value(value) { }
		search_request_data(const json_value::ptr value, const char* key) : json_value(value, key) { }

		search_request_data(const Json::Value& value) : json_value(value) { }
		search_request_data(const Json::Value& value, const char* key) : json_value(value, key) { }

	public:
		virtual ~search_request_data() override { }

		// Utility
		virtual bool valid() const override { return json_value::valid() && has_keyword() && (strlen(get_keyword()) <= limit::search::keyword) && has_filter(); }

		bool has_keyword() const { return get_value().isMember(json::keyword) && !get_value()[json::keyword].empty(); }
		bool has_filter() const { return get_value().isMember(json::filter) && !get_value()[json::filter].empty(); }

		// Mutator
		void set_keyword(const std::string& keyword) { get_value()[json::keyword] = keyword; }
		void set_keyword(const char* keyword) { get_value()[json::keyword] = keyword; }
		void set_filter(const file_filter filter) { get_value()[json::filter] = static_cast<Json::Value::UInt>(filter); }

		// Accessor
		const char* get_keyword() const { return get_value()[json::keyword].asCString(); }
		file_filter get_filter() const { return static_cast<file_filter>(get_value()[json::filter].asUInt()); }

		// Static
		static ptr create() { return ptr(new search_request_data()); }
		static ptr create(const std::string& input) { return ptr(new search_request_data(input)); }
		static ptr create(const char* input) { return ptr(new search_request_data(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new search_request_data(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new search_request_data(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new search_request_data(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new search_request_data(value, key)); }
	};
}

#endif
