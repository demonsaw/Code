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

#ifndef _EJA_VERSION_MESSAGE_H_
#define _EJA_VERSION_MESSAGE_H_

#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class version_message final
	{
		make_factory(version_message);

	protected:
		u32 m_version;

	public:
		version_message();
		version_message(const version_message& msg) : m_version(msg.m_version) { }

		// Operator
		version_message& operator=(const version_message& msg);

	public:
		// Has
		bool has_version() const { return m_version != 0; }

		// Set
		void set_version(const u32& version) { m_version = version; }

		// Get
		u32 get_version() const { return m_version; }
	};

	// Operator
	inline version_message& version_message::operator=(const version_message& msg)
	{
		if (this != &msg)
			m_version = msg.m_version;

		return *this;
	}
}

namespace msgpack
{
	MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
	{
		namespace adaptor
		{
			// Read
			template<>
			struct convert<eja::version_message>
			{
				msgpack::object const& operator()(msgpack::object const& o, eja::version_message& v) const
				{
					v.set_version(o.as<u32>());
					return o;
				}
			};

			// Write
			template<>
			struct pack<eja::version_message>
			{
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, eja::version_message const& v) const
				{
					o.pack_uint32(v.get_version());
					return o;
				}
			};
		}
	}
}

#endif
