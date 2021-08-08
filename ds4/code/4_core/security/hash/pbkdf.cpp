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

#include "security/hash/md.h"
#include "security/hash/pbkdf.h"
#include "security/hash/sha.h"

namespace eja
{
	// Interface
	void pbkdf::clear()
	{
		salt::clear();

		set_iterations();
	}

	// Static
	pbkdf::ptr pbkdf::create(const char* name)
	{
		// MD
		if (boost::equals(name, pbkdf_name::md5))
			return pbkdf_md5::create();

		// SHA-1
		if (boost::equals(name, pbkdf_name::sha1))
			return pbkdf_sha1::create();

		// SHA-2
		if (boost::equals(name, pbkdf_name::sha224))
			return pbkdf_sha224::create();

		if (boost::equals(name, pbkdf_name::sha256))
			return pbkdf_sha256::create();

		if (boost::equals(name, pbkdf_name::sha384))
			return pbkdf_sha384::create();

		if (boost::equals(name, pbkdf_name::sha512))
			return pbkdf_sha512::create();

		// Default
		return pbkdf_sha1::create();
	}
}
