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

#ifndef _EJA_JSON_VALUE_
#define _EJA_JSON_VALUE_

#include <iostream>
#include <memory>
#include <string>

#include "json_lib.h"

namespace eja
{
	class json_value : public std::enable_shared_from_this<json_value>
	{
	public:
		using ptr = std::shared_ptr<json_value>;
		using value_ptr = std::shared_ptr<Json::Value>;

	protected:
		value_ptr m_value = nullptr;
		json_value::ptr m_root = nullptr;

	protected:
		json_value() : m_value(std::make_shared<Json::Value>()) { }
		json_value(const std::string& input) : json_value() { parse(input); }
		json_value(const char* input) : json_value() { parse(input); }

		json_value(const json_value::ptr value) : m_root(value) { set_value(value->get_value()); }
		json_value(const json_value::ptr value, const char* key) : m_root(value) { set_value(value->get_value(), key); }

		json_value(const Json::Value& value) { set_value(value); }
		json_value(const Json::Value& value, const char* key) { set_value(value, key); }

		// Mutator
		void set_value(const Json::Value& value) { m_value = std::shared_ptr<Json::Value>(const_cast<Json::Value*>(&value), [](Json::Value* value){}); }		
		void set_value(const Json::Value& value, const size_t index) { set_value(const_cast<Json::Value&>(value)[static_cast<Json::Value::UInt>(index)]); }

		void set_value(const Json::Value& value, const char* key) { set_value(const_cast<Json::Value&>(value)[key]); }		
		void set_value(const char* key) { set_value(get_value(), key); }

	public:		
		virtual ~json_value() { }

		// Operator
		friend std::ostream& operator<<(std::ostream& os, const json_value& value);

		// Interface
		virtual void clear() { m_value->clear(); }		
		virtual std::string str() const;
		
		// Utility
		bool empty() const { return !m_value || m_value->empty(); }
		virtual bool valid() const { return true; }
		bool invalid() const { return !valid(); }

		virtual void parse(const std::string& input);
		virtual void parse(const char* input);

		// Mutator
		template <typename T> void set(const std::string& name, const T& t) { get_value()[name] = t; }
		template <typename T> void set(const char* name, const T& t) { get_value()[name] = t; }
		void set(const Json::Value& value) { m_value = value_ptr(const_cast<Json::Value*>(&value), [](Json::Value* value){}); }		

		// Accessor
		const Json::Value& get(const std::string& name) const { return get_value()[name]; }
		Json::Value& get(const std::string& name) { return get_value()[name]; }

		const Json::Value& get(const char* name) const { return get_value()[name]; }
		Json::Value& get(const char* name) { return get_value()[name]; }

		// Accessor
		const Json::Value& get_value() const { return *m_value; }
		Json::Value& get_value() { return *m_value; }

		const json_value::ptr get_root() const { return m_root; }
		json_value::ptr get_root() { return m_root; }

		// Static
		static ptr create() { return ptr(new json_value()); }
		static ptr create(const std::string& input) { return ptr(new json_value(input)); }
		static ptr create(const char* input) { return ptr(new json_value(input)); }
		
		static ptr create(const json_value::ptr value) { return ptr(new json_value(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_value(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_value(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_value(value, key)); }
	};
}

#endif
