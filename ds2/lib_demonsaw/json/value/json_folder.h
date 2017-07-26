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

#ifndef _EJA_JSON_FOLDER_
#define _EJA_JSON_FOLDER_

#include <memory>
#include <string>

#include "json_name.h"
#include "json/json.h"
#include "json/json_array.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "object/io/folder.h"

namespace eja
{
	class json_folder final : public json_name
	{
	public:
		using ptr = std::shared_ptr<json_folder>;

	private:
		json_folder() { }
		json_folder(const std::string& input) { parse(input); }
		json_folder(const char* input) { parse(input); }

		json_folder(const json_value::ptr value) : json_name(value) { }
		json_folder(const json_value::ptr value, const char* key) : json_name(value, key) { }

		json_folder(const Json::Value& value) : json_name(value) { }
		json_folder(const Json::Value& value, const char* key) : json_name(value, key) { }

	public:
		virtual ~json_folder() { }

		// Utility
		virtual bool valid() const override { return json_name::valid() && has_size(); }

		bool has_size() const { return get_value().isMember(json::size); }

		// Mutator
		void set(const folder::ptr folder);
		void set_size(const u64 size) { get_value()[json::size] = static_cast<Json::Value::UInt64>(size); }

		// TODO: v2.0 default size should be 0 (not -1)
		//
		//
		// Accessor
		u64 get_size() const { return static_cast<u64>(get_value()[json::size].asInt64()); }

		// Static
		static ptr create() { return ptr(new json_folder()); }
		static ptr create(const std::string& input) { return ptr(new json_folder(input)); }
		static ptr create(const char* input) { return ptr(new json_folder(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_folder(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_folder(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_folder(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_folder(value, key)); }
	};

	class json_folder_array final : public json_array
	{
	public:
		using ptr = std::shared_ptr<json_folder_array>;

	private:
		json_folder_array() { }
		json_folder_array(const char* input) { parse(input); }

		json_folder_array(const json_value::ptr value) : json_array(value) { }
		json_folder_array(const json_value::ptr value, const char* key) : json_array(value, key) { }

		json_folder_array(const Json::Value& value) : json_array(value) { }
		json_folder_array(const Json::Value& value, const char* key) : json_array(value, key) { }

	public:
		virtual ~json_folder_array() { }

		// Interface
		void add(const folder::ptr folder) { json_array::add<json_folder>()->set(folder); }

		// Utility
		virtual bool valid() const override { return json_array::valid() && has_folders(); }

		bool has_folders() const { return get_value().isMember(json::folders) && !get_value()[json::folders].empty(); }

		// Static
		static ptr create() { return ptr(new json_folder_array()); }
		static ptr create(const char* input) { return ptr(new json_folder_array(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_folder_array(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_folder_array(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_folder_array(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_folder_array(value, key)); }
	};
}

#endif
