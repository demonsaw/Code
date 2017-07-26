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
#include <iomanip>
#include <locale>
#include <sstream>

#include "file_util.h"
#include "security/hex.h"
#include "security/md.h"

namespace eja
{
	// Utility
	bool file_util::create(const boost::filesystem::path& path, const u64 bytes /*= 0*/)
	{
		if (exists(path))
		{
			const auto sys_size = boost::filesystem::file_size(path);
			if (sys_size >= bytes)
			{
				if (sys_size > bytes)
				{
					boost::system::error_code error;
					boost::filesystem::resize_file(path, bytes, error);
					return !error;
				}					

				return true;
			}
		}			

		std::ofstream ofs(path.string(), std::ios::binary | std::ios::out);

		if (ofs.good())
		{
			if (bytes)
			{
				ofs.seekp(bytes - 1, std::ios::beg);
				ofs.put('\0');
			}

			ofs.close();
			
			return true;
		}

		return false;
	}

	// Utility
	std::string file_util::read(const boost::filesystem::path& path, u64 input_size /*= 0*/, u64 offset /*= 0*/)
	{
		std::string input;
		read(input, input_size, offset);
		return input;
	}

	u64 file_util::read(const boost::filesystem::path& path, std::string& input, u64 input_size /*= 0*/, u64 offset /*= 0*/)
	{
		std::ifstream ifs(path.string(), std::ios::binary | std::ios::in | std::ios::ate);

		if (ifs.good())
		{
			const u64 size = ifs.tellg();
			if (offset > size)
				offset = 0;

			if ((input_size == 0) || (input_size + offset) > size)
				input_size = (size - offset);

			input.resize(static_cast<size_t>(input_size));
			ifs.seekg(offset, std::ios::beg);
			ifs.read(&input[0], input.size());
			ifs.close();

			return ifs.gcount();
		}

		return 0;
	}

	u64 file_util::write(const boost::filesystem::path& path, const byte* output, const u64 output_size, u64 offset /*= 0*/)
	{
		u64 status = 0; 
		std::ofstream ofs(path.string(), std::ios::binary | std::ios::in | std::ios::out);

		if (ofs.good())
		{
			ofs.seekp(offset, std::ios::beg);
			ofs.write(reinterpret_cast<const char*>(output), output_size);
			status = static_cast<u64>(ofs.tellp()) - offset; 
			ofs.close();
		}

		return status;
	}

	u64 file_util::append(const boost::filesystem::path& path, const byte* output, const u64 output_size)
	{
		u64 status = 0;
		std::ofstream ofs(path.string(), std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);

		if (ofs.good())
		{
			const auto offset = ofs.tellp();
			ofs.write(reinterpret_cast<const char*>(output), output_size);
			status = static_cast<u64>(ofs.tellp()) - offset;
			ofs.close();
		}

		return status;
	}

	// Type
	bool file_util::is_valid(const file_filter filter, const std::string& ext)
	{
		switch (filter)
		{
			case file_filter::audio:	return is_audio(ext);
			case file_filter::image:	return is_image(ext);
			case file_filter::video:	return is_video(ext);
			default:					return true;
		}
	}

	bool file_util::is_audio(const std::string& ext)
	{
		static const char* exts[] = { "aac", "flac", "m4a", "mp3", "wav", "wma", "cue" };
		for (const auto& item : exts)
		{
			if (strstr(item, ext.c_str()))
				return true;
		}

		return false;
	}

	bool file_util::is_image(const std::string& ext)
	{
		static const char* exts[] = { "bmp", "gif", "ico", "jpg", "jpeg", "png", "tif", "tiff" };
		for (const auto& item : exts)
		{
			if (strstr(item, ext.c_str()))
				return true;
		}

		return false;
	}

	bool file_util::is_video(const std::string& ext)
	{
		static const char* exts[] = { "avi", "mov", "mpe", "mpg", "mpeg", "mkv", "ogg", "wmv", "mp4" };
		for (const auto& item : exts)
		{
			if (strstr(item, ext.c_str()))
				return true;
		}

		return false;
	}

	// Accessor
	std::string file_util::get_id(const file::ptr file, const hash::ptr hash)
	{
		const auto id = hash->compute(file->get_name() + boost::lexical_cast<std::string>(file->get_size()));
		return hex::encode(id);
	}
	
	std::string file_util::get_size(const u64 bytes)
	{
		if (!bytes)
			return "0 B";

		static const u64 limits[] {1024 * 1024 * 1024, 1024 * 1024, 1024};
		static const char* units[] {" GB", " MB", " KB"};
		enum unit_type { gb, mb, kb, max };
		
		for (int i = 0; i < unit_type::max; ++i)
		{
			if (bytes >= limits[i])
			{
				const auto value = static_cast<double>(bytes) / static_cast<double>(limits[i]);

				std::stringstream ss;
				ss.imbue(std::locale(""));
				ss << std::fixed << std::setfill('0') << std::setprecision(2) << value << units[i];
				return ss.str();
			}
		}

		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << std::setfill('0') << std::setprecision(2) << bytes << " B";
		return ss.str();
	}

	std::string file_util::get_speed(const u64 bytes)
	{
		if (!bytes)
			return "0 B/s";

		static const u64 limits[] { 1000 * 1000 * 1000, 1000 * 1000, 1000 };
		static const char* units[] { " GB/s",  " MB/s", " KB/s" };
		enum unit_type { gb, mb, kb, max };

		for (int i = 0; i < unit_type::max; ++i)
		{
			if (bytes >= limits[i])
			{
				const auto value = static_cast<double>(bytes) / static_cast<double>(limits[i]);

				std::stringstream ss;
				ss.imbue(std::locale(""));
				ss << std::fixed << std::setfill('0') << std::setprecision(2) << value << units[i];
				return ss.str();
			}
		}

		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << std::setfill('0') << std::setprecision(2) << bytes << " B/s";
		return ss.str();
	}
}
