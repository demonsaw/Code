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

#ifndef _EJA_JSON_HEADER_
#define _EJA_JSON_HEADER_

#include <memory>
#include <string>

#include "json.h"
#include "json_lib.h"
#include "json_value.h"
#include "utility/default_value.h"

namespace eja
{
	class json_header final : public json_value
	{
	public:
		using ptr = std::shared_ptr<json_header>;

	private:
		json_header() { init(); }
		json_header(const std::string& input) { parse(input); }
		json_header(const char* input) { parse(input); }

		json_header(const json_value::ptr value) : json_value(value) { }
		json_header(const Json::Value& value) : json_value(value) { }

		// Utility
		void init() { set_version(default_version::json); }

	public:		
		virtual ~json_header() override { }

		// Utility
		virtual bool valid() const override { return json_value::valid() && has_version(); }

		bool has_version() const { return get_value().isMember(json::version) && !get_value()[json::version].empty(); }
		bool has_nonce() const { return get_value().isMember(json::nonce) && !get_value()[json::nonce].empty(); }
		bool has_session() const { return get_value().isMember(json::session) && !get_value()[json::session].empty() && strlen(get_session()); }

		// Mutator
		void set_version(const size_t version) { get_value()[json::version] = static_cast<Json::Value::UInt>(version); }		
		void set_nonce(const size_t nonce) { get_value()[json::nonce] = static_cast<Json::Value::UInt>(nonce); }
		void set_session(const std::string& session) { get_value()[json::session] = session; }
		void set_session(const char* session) { get_value()[json::session] = session; }

		// Accessor
		size_t get_version() const { return static_cast<size_t>(get_value()[json::version].asUInt()); }
		size_t get_nonce() const { return static_cast<size_t>(get_value()[json::nonce].asUInt()); }
		const char* get_session() const { return get_value()[json::session].asCString(); }

		// Static
		static ptr create() { return ptr(new json_header()); }
		static ptr create(const std::string& input) { return ptr(new json_header(input)); }
		static ptr create(const char* input) { return ptr(new json_header(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_header(value)); }
		static ptr create(const Json::Value& value) { return ptr(new json_header(value)); }
	};
}

#endif
