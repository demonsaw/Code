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

#ifndef _EJA_JSON_PACKET_
#define _EJA_JSON_PACKET_

#include <memory>
#include <string>

#include "json.h"
#include "json_lib.h"
#include "json_value.h"

namespace eja
{
	class json_packet final : public json_value
	{
	public:
		using ptr = std::shared_ptr<json_packet>;

	private:
		json_packet() { init(); }
		json_packet(const std::string& input) { parse(input); }
		json_packet(const char* input) { parse(input); }

		json_packet(const json_value::ptr value) : json_value(value) { init(); }
		json_packet(const Json::Value& value) : json_value(value) { init(); }

		// Utility
		void init();

	public:		
		virtual ~json_packet() override { }

		// Interface
		virtual void clear() override;
		virtual std::string str() const override;

		// Utility
		virtual void parse(const std::string& input) override;
		virtual void parse(const char* input) override;

		bool has_message() const { return get_value().isMember(json::message) && !get_value()[json::message].empty(); }
		bool has_data() const { return get_value().isMember(json::data) && !get_value()[json::data].empty(); }

		// Static
		static ptr create() { return ptr(new json_packet()); }
		static ptr create(const std::string& input) { return ptr(new json_packet(input)); }
		static ptr create(const char* input) { return ptr(new json_packet(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_packet(value)); }
		static ptr create(const Json::Value& value) { return ptr(new json_packet(value)); }
	};
}

#endif
