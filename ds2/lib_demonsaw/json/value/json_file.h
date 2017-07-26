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

#ifndef _EJA_JSON_FILE_
#define _EJA_JSON_FILE_

#include <memory>
#include <string>
#include <boost/filesystem.hpp>

#include "json_name.h"
#include "json/json.h"
#include "json/json_array.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "object/io/file.h"

namespace eja
{
	class json_file final : public json_name
	{
	public:
		using ptr = std::shared_ptr<json_file>;

	private:
		json_file() { }
		json_file(const std::string& input) { parse(input); }
		json_file(const char* input) { parse(input); }

		json_file(const json_value::ptr value) : json_name(value) { }
		json_file(const json_value::ptr value, const char* key) : json_name(value, key) { }

		json_file(const Json::Value& value) : json_name(value) { }
		json_file(const Json::Value& value, const char* key) : json_name(value, key) { }

	public:		
		virtual ~json_file() { }

		// Utility
		virtual bool valid() const override { return json_name::valid() && has_size(); }

		bool has_size() const { return get_value().isMember(json::size) && !get_value()[json::size].empty(); }

		// Mutator
		void set(const file::ptr file);
		void set_size(const u64 size) { get_value()[json::size] = static_cast<Json::Value::UInt64>(size); }

		// TODO: v2.0 default size should be 0 (not -1)
		//
		//
		// Accessor
		u64 get_size() const { return static_cast<u64>(get_value()[json::size].asInt64()); }
		const std::string get_stem() const { return boost::filesystem::path(get_name()).stem().string(); }
		const std::string get_parent() const { return boost::filesystem::path(get_name()).parent_path().string(); }
		const std::string get_extension() const { return boost::filesystem::path(get_name()).extension().string(); }

		// Static
		static ptr create() { return ptr(new json_file()); }
		static ptr create(const std::string& input) { return ptr(new json_file(input)); }
		static ptr create(const char* input) { return ptr(new json_file(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_file(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_file(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_file(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_file(value, key)); }
	};

	class json_file_array final : public json_array
	{
	public:
		using ptr = std::shared_ptr<json_file_array>;

	private:
		json_file_array() { }
		json_file_array(const char* input) { parse(input); }

		json_file_array(const json_value::ptr value) : json_array(value) { }
		json_file_array(const json_value::ptr value, const char* key) : json_array(value, key) { }

		json_file_array(const Json::Value& value) : json_array(value) { }
		json_file_array(const Json::Value& value, const char* key) : json_array(value, key) { }

	public:
		virtual ~json_file_array() { }

		// Interface
		void add(const file::ptr file) { json_array::add<json_file>()->set(file); }

		// Utility
		virtual bool valid() const override { return json_array::valid() && has_files(); }

		bool has_files() const { return get_value().isMember(json::files) && !get_value()[json::files].empty(); }

		// Static
		static ptr create() { return ptr(new json_file_array()); }
		static ptr create(const char* input) { return ptr(new json_file_array(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_file_array(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_file_array(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_file_array(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_file_array(value, key)); }
	};
}

#endif
