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

#ifndef _EJA_JSON_JOIN_
#define _EJA_JSON_JOIN_

#include <memory>
#include <string>

#include "json_name.h"
#include "entity/entity.h"
#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "object/client/client.h"

namespace eja
{
	class json_join final : public json_name
	{
	public:
		using ptr = std::shared_ptr<json_join>;

	private:
		json_join() { }
		json_join(const std::string& input) { parse(input); }
		json_join(const char* input) { parse(input); }

		json_join(const json_value::ptr value) : json_name(value) { }
		json_join(const json_value::ptr value, const char* key) : json_name(value, key) { }

		json_join(const Json::Value& value) : json_name(value) { }
		json_join(const Json::Value& value, const char* key) : json_name(value, key) { }


	public:
		virtual ~json_join() { }

		// Mutator
		void set(const client::ptr client);

		// Static
		static ptr create() { return ptr(new json_join()); }
		static ptr create(const std::string& input) { return ptr(new json_join(input)); }
		static ptr create(const char* input) { return ptr(new json_join(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_join(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_join(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_join(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_join(value, key)); }
	};
}

#endif
