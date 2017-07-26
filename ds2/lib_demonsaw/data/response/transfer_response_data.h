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

#ifndef _EJA_TRANSFER_RESPONSE_DATA_
#define _EJA_TRANSFER_RESPONSE_DATA_

#include <memory>
#include <string>

#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"

namespace eja
{
	class transfer_response_data final : public json_value
	{
	public:
		using ptr = std::shared_ptr<transfer_response_data>;

	private:
		transfer_response_data() { }
		transfer_response_data(const std::string& input) { parse(input); }
		transfer_response_data(const char* input) { parse(input); }

		transfer_response_data(const json_value::ptr value) : json_value(value) { }
		transfer_response_data(const json_value::ptr value, const char* key) : json_value(value, key) { }

		transfer_response_data(const Json::Value& value) : json_value(value) { }
		transfer_response_data(const Json::Value& value, const char* key) : json_value(value, key) { }

	public:
		virtual ~transfer_response_data() override { }

		// Static
		static ptr create() { return ptr(new transfer_response_data()); }
		static ptr create(const std::string& input) { return ptr(new transfer_response_data(input)); }
		static ptr create(const char* input) { return ptr(new transfer_response_data(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new transfer_response_data(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new transfer_response_data(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new transfer_response_data(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new transfer_response_data(value, key)); }
	};
}

#endif
