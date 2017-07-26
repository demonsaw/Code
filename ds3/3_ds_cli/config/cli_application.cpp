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

#ifdef _MSC_VER
#pragma warning(disable: 4005 4075)
#endif

// HACK: There is a FOREACH conflict in boost and Qt
#include <boost/network/uri.hpp>

#include "config/cli_application.h"
#include "security/filter/base.h"

namespace eja
{
	// Constructor
	cli_application::cli_application(int argc, char* argv[]) : application(argc, argv)
	{		
		// HACK: Libraries have static initialization (not thread-safe)
		boost::network::uri::uri uri("");
		base64::encode("");
		base64::decode("");

		m_saving_enabled = false;
		m_clients_enabled = false;
	}

	// Interface
	void cli_application::init()
	{
		application::init();

		// TODO
		//
		//
	}

	void cli_application::shutdown()
	{
		application::shutdown();

		// TODO
		//
	}

	void cli_application::update()
	{
		application::update();

		// TODO
		//
	}

	void cli_application::clear()
	{
		application::clear();

		// TODO
		//
	}

	// Utility
	void cli_application::load(const char* psz)
	{
		application::load(psz);

		// TODO
		//
	}
}
