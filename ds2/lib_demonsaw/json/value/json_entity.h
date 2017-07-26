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

#ifndef _EJA_JSON_ENTITY_
#define _EJA_JSON_ENTITY_

#include <memory>
#include <string>

#include "json_id.h"
#include "entity/entity.h"
#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"

namespace eja
{
	class json_entity final : public json_id
	{
	public:
		using ptr = std::shared_ptr<json_entity>;

	private:
		json_entity() { }
		json_entity(const std::string& input) { parse(input); }
		json_entity(const char* input) { parse(input); }

		json_entity(const json_value::ptr value) : json_id(value) { }
		json_entity(const json_value::ptr value, const char* key) : json_id(value, key) { }

		json_entity(const Json::Value& value) : json_id(value) { }
		json_entity(const Json::Value& value, const char* key) : json_id(value, key) { }

	public:		
		virtual ~json_entity() override { }

		// Mutator
		void set(const entity::ptr entity) { set_id(entity->get_id()); }

		// Static
		static ptr create() { return ptr(new json_entity()); }
		static ptr create(const std::string& input) { return ptr(new json_entity(input)); }
		static ptr create(const char* input) { return ptr(new json_entity(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_entity(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_entity(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_entity(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_entity(value, key)); }
	};
}

#endif
