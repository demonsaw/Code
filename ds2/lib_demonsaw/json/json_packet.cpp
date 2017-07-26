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

#include "json_packet.h"
#include "json_header.h"
#include "json_value.h"
#include "utility/default_value.h"

namespace eja
{
	// Interface
	void json_packet::clear()
	{
		json_value::clear();

		init();
	}

	// Utility
	void json_packet::init()
	{
		const auto header = json_header::create(get_value());
		header->set_version(default_version::json);
	}

	void json_packet::parse(const std::string& input)
	{
		json_value::parse(input);

		// HACK: Message block is string data (not a proper json tree)
		//
		Json::Value& value = get_value();
		Json::Value message = value.get(json::message, Json::Value::null);

		if (!message.empty())
		{
			Json::Value node;
			Json::Reader reader;
			if (!reader.parse(message.asCString(), node))
				throw std::runtime_error(reader.getFormattedErrorMessages());

			value[json::message] = node;
		}
	}

	void json_packet::parse(const char* input)
	{
		json_value::parse(input);

		// HACK: Message block is string data (not a proper json tree)
		//
		Json::Value& value = get_value();
		Json::Value message = value.get(json::message, Json::Value::null);

		if (!message.empty())
		{
			Json::Value node;
			Json::Reader reader;
			if (!reader.parse(message.asCString(), node))
				throw std::runtime_error(reader.getFormattedErrorMessages());

			value[json::message] = node;
		}
	}

	std::string json_packet::str() const
	{
		// HACK: Message block is string data (not a proper json tree)
		//
		Json::Value message = get_value().get(json::message, Json::Value::null);

		if (!message.empty())
		{
			Json::FastWriter writer;
			writer.omitEndingLineFeed();

			Json::Value value = const_cast<Json::Value&>(get_value());
			value[json::message] = writer.write(message);
			return writer.write(value);
		}

		return json_value::str();
	}
}
