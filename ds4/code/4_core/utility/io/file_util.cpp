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

#include <cassert>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>

#include "security/filter/hex.h"
#include "security/hash/md.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Utility
	bool file_util::exists(const boost::filesystem::path& path)
	{
		boost::system::error_code error;
		return boost::filesystem::exists(path, error) && !boost::filesystem::is_directory(path, error);
	}

	void file_util::create(const boost::filesystem::path& path, const u64 bytes /*= 0*/)
	{
		std::ofstream ofs;
		ofs.exceptions(std::ofstream::failbit);
		ofs.open(path.string(), std::ios::binary | std::ios::out);

		if (bytes)
		{
			try
			{
				ofs.seekp(bytes - 1, std::ios::beg);
				ofs.put('\0');
			}
			catch (...)
			{
				ofs.close();

				throw;
			}
		}

		ofs.close();
	}

	std::string file_util::read(const boost::filesystem::path& path, u64 input_size /*= 0*/, u64 offset /*= 0*/)
	{
		std::string input;
		read(path, input, input_size, offset);
		return input;
	}

	u64 file_util::read(const boost::filesystem::path& path, std::string& input, u64 input_size /*= 0*/, u64 offset /*= 0*/)
	{
		std::ifstream ifs;
		ifs.exceptions(std::ifstream::failbit);
		ifs.open(path.string(), std::ios::binary | std::ios::in | std::ios::ate);

		u64 bytes = 0;
		const u64 size = ifs.tellg();
		if (offset < size)
		{
			if ((input_size == 0) || (input_size + offset) > size)
				input_size = (size - offset);

			input.resize(static_cast<size_t>(input_size));
			ifs.seekg(offset, std::ios::beg);
			ifs.read(&input[0], input.size());
			bytes = ifs.gcount();
		}

		ifs.close();

		return bytes;
	}

	u64 file_util::size(const boost::filesystem::path& path)
	{
		boost::system::error_code error;
		return boost::filesystem::file_size(path, error);
	}

	u64 file_util::write(const boost::filesystem::path& path, const byte* output, const u64 output_size, u64 offset /*= 0*/)
	{
		// TODO: Add back when real streaming is available
		//
		//
		/*const auto size = file_util::size(path);
		const auto total_size = offset + output_size;
		if (total_size > size)
			resize(path, total_size);*/

		std::ofstream ofs;
		ofs.exceptions(std::ofstream::failbit);
		ofs.open(path.string(), std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);

		u64 bytes = 0;

		try
		{
			const u64 size = ofs.tellp();

			if ((offset + output_size) <= size)
			{
				ofs.seekp(offset, std::ios::beg);
				ofs.write(reinterpret_cast<const char*>(output), output_size);
				bytes = static_cast<u64>(ofs.tellp()) - offset;
			}
		}
		catch (...)
		{
			ofs.close();

			throw;
		}

		ofs.close();

		return bytes;
	}

	u64 file_util::append(const boost::filesystem::path& path, const byte* output, const u64 output_size)
	{
		std::ofstream ofs;
		ofs.exceptions(std::ofstream::failbit);
		ofs.open(path.string(), std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);

		u64 bytes = 0;

		try
		{
			const u64 offset = ofs.tellp();
			ofs.write(reinterpret_cast<const char*>(output), output_size);
			bytes = static_cast<u64>(ofs.tellp()) - offset;
		}
		catch (...)
		{
			ofs.close();

			throw;
		}

		ofs.close();

		return bytes;
	}

	// Accessor
	std::string file_util::get_id(const file_component::ptr file, const hash::ptr hash)
	{
		std::ifstream ifs;
		ifs.exceptions(std::ifstream::failbit);
		ifs.open(file->get_data().string(), std::ios::binary | std::ios::in | std::ios::ate);

		// Size
		const u64 file_size = ifs.tellg();
		hash->update(&file_size, sizeof(u64));

		// TODO: Make this configurable
		//
		//
		// Data
		const auto input_size = 4 << 10;

		if (file_size <= (input_size * 3))
		{
			char* const input = static_cast<char*>(alloca(file_size));

			// Read
			ifs.seekg(0, std::ios::beg);
			ifs.read(input, file_size);
			if (ifs.gcount() != file_size)
				return "";

			// Hash
			hash->update(input, ifs.gcount());
		}
		else
		{
			// Data
			char* const input = static_cast<char*>(alloca(input_size * 3));
			const u64 input_offset[] = { 0, ((file_size >> 1) - (input_size >> 1)), (file_size - input_size) };

			for (auto i = 0; i < 3; ++i)
			{
				// Read
				ifs.seekg(input_offset[i], std::ios::beg);
				ifs.read(input + (input_size * i), input_size);
				if (ifs.gcount() != input_size)
					return "";
			}

			// Hash
			hash->update(input, ifs.gcount());
		}

		ifs.close();

		return hash->finish();
	}

	std::string file_util::get_size(const u64 bytes)
	{
		if (!bytes)
			return "0 B";

		static const u64 limits[] { 1024ull * 1024ull * 1024ull * 1024ull, 1024ull * 1024ull * 1024ull, 1024ull * 1024ull, 1024ull };
		static const char* units[] {" TB", " GB", " MB", " KB"};
		enum unit_type { tb, gb, mb, kb, max };

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
			return "0 bps";

		const auto bits = bytes << 3;
		static const u64 limits[] { (1000 * 1000 * 1000), (1000 * 1000), 1000 };
		static const char* units[] { " Gbps",  " Mbps", " Kbps" };
		enum unit_type { gbps, mbps, kbps, max };

		for (int i = 0; i < unit_type::max; ++i)
		{
			if (bits >= limits[i])
			{
				const auto value = static_cast<double>(bits) / static_cast<double>(limits[i]);

				std::stringstream ss;
				ss.imbue(std::locale(""));
				ss << std::fixed << std::setfill('0') << std::setprecision(2) << value << units[i];
				return ss.str();
			}
		}

		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << std::setfill('0') << std::setprecision(2) << bits << " bps";
		return ss.str();
	}

	//////////////////////////////////////////////////////////////////////////
	// CODE TO SCAN ENTIRE FILE
	//
	// TODO: This is too slow!
	//
	//
	// Size
	//const u64 file_size = ifs.tellg();
	//ifs.seekg(0, std::ios::beg);

	//// Data
	//const auto input_size = 128 << 10;
	//char input[input_size];

	//for (auto i = 0; i < file_size; i += input_size)
	//{
	//	// Read
	//	ifs.read(input, input_size);
	//	assert(ifs.gcount());

	//	// Hash
	//	hash->update(input, ifs.gcount());
	//}
	//
	//////////////////////////////////////////////////////////////////////////
}
