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

#ifndef _EJA_ERROR_
#define _EJA_ERROR_

#include <exception>
#include <string>
#include <memory>

#include "object/object.h"

namespace eja
{
	class error : public object
	{
	protected:
		std::string m_message;

	public:
		using ptr = std::shared_ptr<error>;

	public:
		error() { }
		error(const eja::error& error) : object(error), m_message(error.m_message) { }
		error(const std::string& id) : object(id) { }
		error(const char* id) : object(id) { }
		virtual ~error() override { }

		// Interface
		virtual void clear() override;

		// Utility		
		virtual void write(const std::string& str) { }
		virtual void write(const char* psz) { write(std::string(psz)); }
		virtual void write(const std::exception& e) { write(e.what()); }

		bool message() const { return !m_message.empty(); }

		// Mutator
		void set_message(const std::string& message) { m_message = message; }

		// Accessor
		const std::string& get_message() const { return m_message; }

		// Static
		static ptr create() { return std::make_shared<error>(); }
		static ptr create(const eja::error& error) { return std::make_shared<eja::error>(error); }
		static ptr create(const error::ptr error) { return std::make_shared<eja::error>(*error); }
		static ptr create(const std::string& id) { return std::make_shared<error>(id); }
		static ptr create(const char* id) { return std::make_shared<error>(id); }
	};
}

#endif
