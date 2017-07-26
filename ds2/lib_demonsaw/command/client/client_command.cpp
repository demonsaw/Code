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

#include "client_command.h"
#include "security/base.h"

namespace eja
{
	// Accessor
	json_data::ptr client_command::get_data(const json_data::ptr data)
	{
		if (!data->empty())
		{
			// v1.0
			//////////////////////////////////////////////////////////////
			const auto security = get_client_security();

			if (!security->empty())
			{
				// Request
				const auto json_encoded = data->get();
				const auto json_encrypted = base64::decode(json_encoded);
				const auto json = security->decrypt(json_encrypted);

				// Response				
				const auto response_data = json_data::create(json);

				return response_data;
			}
			else
			{
				// Request
				const auto json = data->get();

				// Response
				const auto response_data = json_data::create(json);

				return response_data;
			}
			//////////////////////////////////////////////////////////////

			// v2.0
			//////////////////////////////////////////////////////////////
			/*if (has_client_security())
			{
				// ...
			}*/
			//////////////////////////////////////////////////////////////
		}
		
		return json_data::create();
	}

	json_data::ptr client_command::get_data(const json_packet::ptr packet)
	{
		const auto data = json_data::create(packet);

		return get_data(data);
	}
}