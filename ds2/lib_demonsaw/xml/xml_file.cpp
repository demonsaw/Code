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
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "xml_file.h"

namespace eja
{	
	// Interface
	bool xml_file::read(const char* path)
	{
		assert(path);

		clear();

		if (boost::filesystem::exists(path) && !boost::filesystem::is_directory(path))
		{			
			boost::property_tree::xml_parser::read_xml(path, m_tree, boost::property_tree::xml_parser::trim_whitespace | boost::property_tree::xml_parser::no_comments);
			return true;
		}

		return false;
	}

	bool xml_file::write(const char* path)
	{
		assert(path);

		if (!boost::filesystem::is_directory(path))
		{
			boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
			boost::property_tree::xml_parser::write_xml(path, m_tree, std::locale(), settings);
			return true;
		}

		return false;
	}

	// Utility
	std::string xml_file::str() const
	{
		std::ostringstream sstream;
		boost::property_tree::xml_parser::write_xml(sstream, m_tree);

		return sstream.str();
	}
}
