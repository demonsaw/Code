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

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/predef.h>

#include "file.h"
#include "system/api.h"
#include "utility/default_value.h"

namespace eja
{
	// Operator
	file& file::operator=(const file& file)
	{
		if (this != &file)
			archive::operator=(file);

		return *this;
	}

	file& file::operator=(const boost::filesystem::path& path)
	{
		m_path = path;
		set_size();

		return *this;
	}

	// Utility
	bool file::is_hidden() const
	{
		if (empty())
			return false;

#if _WIN32
		const auto path = m_path.string();
		DWORD attr = GetFileAttributesA(path.c_str());
		return FILE_ATTRIBUTE_HIDDEN == (FILE_ATTRIBUTE_HIDDEN & attr);
#else
		const auto path = m_path.string();
		return boost::starts_with(path, ".");
#endif
	}

	bool file::is_config() const
	{
		if (empty())
			return false;

		const auto& name = get_name();
		return boost::starts_with(name, default_file::config);
	}

	std::string file::read(u64 input_size /*= 0*/, u64 offset /*= 0*/) const
	{
		std::string input;
		read(input, input_size, offset);
		return input;
	}

	u64 file::read(std::string& input, u64 input_size /*= 0*/, u64 offset /*= 0*/) const
	{
		const auto path = m_path.string();
		std::ifstream ifs(path, std::ios::binary | std::ios::in);

		if (ifs.good())
		{
			if (offset > m_size)
				offset = 0;

			if ((input_size == 0) || (input_size + offset) > m_size)
				input_size = (m_size - offset);

			input.resize(static_cast<size_t>(input_size));
			ifs.seekg(offset, std::ios::beg);			
			ifs.read(&input[0], input.size());
			ifs.close();

			return ifs.gcount();
		}

		return 0;
	}

	u64 file::write(const byte* output, const u64 output_size, u64 offset /*= 0*/) const
	{		
		u64 status = 0;
		const auto path = m_path.string();
		std::ofstream ofs(path, std::ios::binary | std::ios::in | std::ios::out);		

		if (ofs.good())
		{
			ofs.seekp(offset, std::ios::beg);
			ofs.write(reinterpret_cast<const char*>(output), output_size);
			status = static_cast<u64>(ofs.tellp()) - offset;
			ofs.close();
		}

		return status;
	}

	u64 file::append(const byte* output, const u64 output_size)
	{
		u64 status = 0;
		const auto path = m_path.string();
		std::ofstream ofs(path, std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);

		if (ofs.good())
		{
			const auto offset = ofs.tellp();
			ofs.write(reinterpret_cast<const char*>(output), output_size);			
			status = static_cast<u64>(ofs.tellp()) - offset;
			ofs.close();

			m_size = boost::filesystem::file_size(m_path);
		}

		return status;
	}

	// Accessor
	std::string file::get_description() const
	{
#if _WIN32
		SHFILEINFOA shFileInfo;
		const auto ext = m_path.extension().string();
		const auto result = ::SHGetFileInfoA(ext.c_str(), 0, &shFileInfo, sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
		if (result)
			return shFileInfo.szTypeName;
#endif

		return get_extension();
	}

	// Accessor
	std::string file::get_extension() const
	{
		const auto ext = m_path.extension().string();
		return !ext.empty() ? ext.substr(1) : ext;
	}
}
