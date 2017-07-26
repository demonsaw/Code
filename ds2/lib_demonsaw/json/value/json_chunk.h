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

#ifndef _EJA_JSON_CHUNK_
#define _EJA_JSON_CHUNK_

#include <memory>
#include <string>

#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "object/transfer/chunk.h"

namespace eja
{
	class chunk;

	class json_chunk final : public json_value
	{
	public:
		using ptr = std::shared_ptr<json_chunk>;

	private:
		json_chunk() { }
		json_chunk(const std::string& input) { parse(input); }
		json_chunk(const char* input) { parse(input); }

		json_chunk(const json_value::ptr value) : json_value(value) { }
		json_chunk(const json_value::ptr value, const char* key) : json_value(value, key) { }

		json_chunk(const Json::Value& value) : json_value(value) { }
		json_chunk(const Json::Value& value, const char* key) : json_value(value, key) { }


	public:		
		virtual ~json_chunk() override { }

		// Utility
		bool has_size() const { return get_value().isMember(json::size) && !get_value()[json::size].empty(); }		
		bool has_offset() const { return get_value().isMember(json::offset) && !get_value()[json::offset].empty(); }

		// Mutator
		void set(const chunk::ptr chunk);
		void set_size(const u64 size) { get_value()[json::size] = static_cast<Json::Value::UInt64>(size); }
		void set_offset(const u64 offset) { get_value()[json::offset] = static_cast<Json::Value::UInt64>(offset); }

		// Accessor
		u64 get_size() const { return static_cast<u64>(get_value()[json::size].asUInt64()); }
		u64 get_offset() const { return static_cast<u64>(get_value()[json::offset].asUInt64()); }

		// Static
		static ptr create() { return ptr(new json_chunk()); }
		static ptr create(const std::string& input) { return ptr(new json_chunk(input)); }
		static ptr create(const char* input) { return ptr(new json_chunk(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_chunk(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_chunk(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_chunk(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_chunk(value, key)); }
	};
}

#endif
