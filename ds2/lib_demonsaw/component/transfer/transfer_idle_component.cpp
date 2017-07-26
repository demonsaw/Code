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

#include <set>

#include "transfer_idle_component.h"
#include "component/client/client_machine_component.h"
#include "component/client/client_option_component.h"
#include "component/io/file_component.h"
#include "component/transfer/download_component.h"
#include "component/transfer/download_thread_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_component.h"
#include "component/transfer/upload_thread_component.h"
#include "entity/entity.h"
#include "utility/boost.h"
#include "utility/default_value.h"

namespace eja
{
	// Callback
	bool transfer_idle_component::on_run()
	{
		try
		{
			const auto owner = get_entity();
			const auto machine = owner->get<client_machine_component>();
			if (machine && machine->running() && machine->success())
			{				
				on_download();
				on_upload();				
			}
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}

		return idle_component::on_run();
	}

	// Utility
	void transfer_idle_component::on_download()
	{
		// Option
		const auto owner = get_entity();
		const auto option = owner->get<client_option_component>();
		if (!option)
			return;

		// Download
		const auto download_vector = owner->get<download_entity_vector_component>();
		if (!download_vector)
			return;
		
		// Remaining
		const auto max_downloads = option->get_max_downloads();
		const auto remaining = max_downloads - std::min(max_downloads, download_vector->size());
		if (!remaining)
			return;

		// Transfer
		std::list<entity::ptr> list;
		const auto transfer_vector = owner->get<transfer_entity_vector_component>();
		if (!transfer_vector)
			return;

		{
			auto_lock_ptr(transfer_vector);

			for (const auto& entity : *transfer_vector)
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer->is_download())
				{
					list.push_back(entity);
					if (list.size() == remaining)
						break;
				}
			}
		}

		for (const auto& entity : list)
		{
			call(function_type::transfer, function_action::remove, entity);
			call(function_type::download, function_action::add, entity);
		}
	}

	void transfer_idle_component::on_upload()
	{
		// Option
		const auto owner = get_entity();
		const auto option = owner->get<client_option_component>();
		if (!option)
			return;

		// Upload
		const auto upload_vector = owner->get<upload_entity_vector_component>();
		if (!upload_vector)
			return;


		// Remaining
		const auto max_uploads = option->get_max_uploads();
		const auto remaining = max_uploads - std::min(max_uploads, upload_vector->size());
		if (!remaining)
			return;

		// Transfer
		std::list<entity::ptr> list;
		const auto transfer_vector = owner->get<transfer_entity_vector_component>();
		if (!transfer_vector)
			return;

		if (option->has_priority_size() || option->has_priority_ext())
		{
			auto_lock_ptr(transfer_vector);

			// TODO: Need to optimize later
			//
			//
			std::set<std::string> exts;
			boost::string_tokenizer tokens(option->get_priority_ext(), boost::char_separator<char>(" ,.|:;"));
			for (const std::string& token : tokens)
				exts.insert(token);

			std::set<std::string> files;

			// Priority
			for (const auto& entity : *transfer_vector)
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer->is_upload())
				{
					const auto file = entity->get<file_component>();
					if (file)
					{
						if ((file->get_size() <= option->get_priority_size()) || (exts.find(file->get_extension()) != exts.end()))
						{
							files.insert(file->get_id());
							list.push_back(entity);
							if (list.size() == remaining)
								break;
						}
					}
				}
			}

			if (list.size() < remaining)
			{
				// Non-priority
				for (const auto& entity : *transfer_vector)
				{
					const auto transfer = entity->get<transfer_component>();
					if (transfer->is_upload())
					{
						const auto file = entity->get<file_component>();
						if (file)
						{
							const auto it = files.find(file->get_id());
							if (it == files.end())
							{
								list.push_back(entity);
								if (list.size() == remaining)
									break;
							}
						}
					}
				}
			}
		}
		else
		{
			auto_lock_ptr(transfer_vector);

			for (const auto& entity : *transfer_vector)
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer->is_upload())
				{
					list.push_back(entity);
					if (list.size() == remaining)
						break;
				}
			}
		}

		for (const auto& entity : list)
		{
			call(function_type::transfer, function_action::remove, entity);
			call(function_type::upload, function_action::add, entity);
		}
	}
}
