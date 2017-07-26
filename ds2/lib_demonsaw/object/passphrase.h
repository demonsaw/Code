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

#ifndef _EJA_PASSPHRASE_
#define _EJA_PASSPHRASE_

#include <memory>
#include <string>

#include "cipher.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class passphrase : public cipher
	{
	private:
		const char* default_passphrase = "z`PG4)r!=986m20O$2.Ih5[0@|)w,(3{L^F53+_T3L7KH271nI,&t.f8Ab6A%j<";

	protected:
		std::string m_passphrase;
		std::string m_salt = default_passphrase;

	public:
		using ptr = std::shared_ptr<passphrase>;

	public:
		passphrase() { set_passphrase(); }
		passphrase(const eja::passphrase& passphrase) : cipher(passphrase), m_passphrase(passphrase.m_passphrase), m_salt(passphrase.m_salt) { }
		passphrase(const std::string& id) : cipher(id) { set_passphrase(); }
		passphrase(const char* id) : cipher(id) { set_passphrase(); }
		virtual ~passphrase() override { }

		// Interface
		virtual void clear() override;

		// Utility
		bool has_passphrase() const { return !m_passphrase.empty(); }
		bool has_salt() const { return !m_salt.empty(); }

		// Mutator
		void set_passphrase() { set_passphrase(nullptr); }
		void set_passphrase(const std::string& passphrase) { set_passphrase(passphrase.c_str()); }
		void set_passphrase(const char* passphrase);

		void set_salt(const std::string& salt) { m_salt = salt; }
		void set_salt(const char* salt) { m_salt = salt; }

		// Accessor
		const std::string& get_passphrase() const { return m_passphrase; }
		const std::string& get_salt() const { return m_salt; }

		// Static
		static ptr create() { return std::make_shared<passphrase>(); }
		static ptr create(const eja::passphrase& passphrase) { return std::make_shared<eja::passphrase>(passphrase); }
		static ptr create(const passphrase::ptr passphrase) { return std::make_shared<eja::passphrase>(*passphrase); }
		static ptr create(const std::string& id) { return std::make_shared<passphrase>(id); }
		static ptr create(const char* id) { return std::make_shared<passphrase>(id); }
	};

	// Container
	derived_type(passphrase_list, mutex_list<passphrase>);
	derived_type(passphrase_map, mutex_map<std::string, passphrase>);
	derived_type(passphrase_queue, mutex_queue<passphrase>);
	derived_type(passphrase_vector, mutex_vector<passphrase>);
}

#endif
