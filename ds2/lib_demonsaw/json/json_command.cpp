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

#include "json_command.h"
#include "utility/default_value.h"

namespace eja
{
	// Interface
	void json_command::clear()
	{
		entity_command::clear();

		m_id.clear();
	}

	void json_async_command::clear()
	{
		entity_command::clear();

		m_command->clear();
	}

	void json_async_command::execute(const entity::ptr entity)
	{
		clear();

		const auto self = shared_from_this();
		const auto command = self->get_command();
		if (!command)
			return;

		std::thread thread([self, entity, command]()
		{
			try
			{
				command->execute(entity);
			}
			catch (std::exception& e)
			{
				command->log(e);
			}
			catch (...)
			{
				command->log();
			}
		});

		thread.detach();
	}

	void json_async_command::execute(const entity::ptr entity, const json_message::ptr message)
	{
		clear();

		const auto self = shared_from_this();
		const auto command = self->get_command();
		if (!command)
			return;

		std::thread thread([self, entity, message, command]()
		{
			try
			{				
				command->execute(entity, message);
			}
			catch (std::exception& e)
			{
				command->log(e);
			}
			catch (...)
			{
				command->log();
			}
		});

		thread.detach();
	}

	void json_async_command::execute(const entity::ptr entity, const json_data::ptr data)
	{
		clear();

		const auto self = shared_from_this();
		const auto command = self->get_command();
		if (!command)
			return;

		std::thread thread([self, entity, data, command]()
		{			
			try
			{				
				command->execute(entity, data);
			}
			catch (std::exception& e)
			{
				command->log(e);
			}
			catch (...)
			{
				command->log();
			}
		});

		thread.detach();
	}

	// Utility
	std::shared_ptr<json_async_command> json_command::make_async() const
	{ 
		return std::shared_ptr<json_async_command>(new json_async_command(std::const_pointer_cast<json_command>(shared_from_this()))); 
	}
	
	void json_command::log(const std::exception& e)
	{
		m_entity->log(e);
	}

	void json_command::log(const std::string& str)
	{
		m_entity->log(str);
	}

	void json_command::log(const char* psz /*= default_error::unknown*/)
	{
		m_entity->log(psz);
	}
}
