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

#ifndef _EJA_APPLICATION_H_
#define _EJA_APPLICATION_H_

#include <memory>
#include <string>
#include <type_traits>
#include <boost/filesystem.hpp>

#include "config/cpptoml.h"
#include "system/type.h"

namespace eja
{
	class application
	{
	protected:
		boost::filesystem::path m_path;

	protected:
		// Read		
		template <typename T, typename U>
		void read(const std::shared_ptr<cpptoml::table>& table, const char* name, U u) const;

		template <typename T, typename U>
		void read_ex(const std::shared_ptr<cpptoml::table>& table, const char* name, U u) const;

		// Write
		template <typename T, typename U>
		void write(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t, U u) const;

		template <typename T>
		void write(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t) const { return write(table, name, t, []() { return true; }); }

		template <typename T, typename U>
		void write_ex(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t, U u) const;

		template <typename T>
		void write_ex(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t) const { return write_ex(table, name, t, []() { return true; }); }

	public:
		// WARNING:	The data referred to by argc and argv must stay valid for the entire lifetime of the QApplication object. 
		// In addition, argc must be greater than zero and argv must contain at least one valid character string.
		application(int& argc, char* argv[]);
		virtual ~application() { }

		// Interface
		virtual void init() { }
		virtual void update();
		virtual void shutdown() { }
		virtual void clear() { }

		// Utility
		virtual bool empty() const { return m_path.empty(); }

		// Read
		virtual void read() = 0;

		// Write
		virtual void write() { }

		// Has
		bool has_path() const { return !m_path.empty(); }

		// Set
		void set_path(const std::wstring& path) { m_path = path; }
		void set_path(const std::string& path) { m_path = path; }

		// Get
		const boost::filesystem::path& get_path() const { return m_path; }
	};

	// Read
	template <typename T, typename U>
	inline void application::read(const std::shared_ptr<cpptoml::table>& table, const char* name, U u) const
	{
		if (table->contains(name))
		{
			const auto value = table->get_as<T>(name);
			if (value)
				u(*value);
		}
	}

	template <typename T, typename U>
	inline void application::read_ex(const std::shared_ptr<cpptoml::table>& table, const char* name, U u) const
	{
		if (table->contains(name))
		{
			const auto value = table->get_as<int64_t>(name);
			if (value)
				u(static_cast<T>(*value));
		}
	}

	// Write
	template <typename T, typename U>
	inline void application::write(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t, U u) const
	{
		if (u())
			table->insert(name, t);
	}

	template <typename T, typename U>
	inline void application::write_ex(const std::shared_ptr<cpptoml::table>& table, const char* name, const T& t, U u) const
	{
		if (u())
			table->insert(name, static_cast<int64_t>(t));
	}
}

#endif
