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

#ifndef _EJA_TRANSFER_
#define _EJA_TRANSFER_

#include <memory>

#include "object/object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	// Type
	enum class transfer_type { none, upload, download };

	// Object
	class transfer : public object
	{
	protected:
		bool m_updated = false;
		size_t m_retries = 0;
		transfer_type m_type = transfer_type::none;
		u64 m_size = 0;
		u64 m_offset = 0;

	public:
		using ptr = std::shared_ptr<transfer>;

	public:
		transfer() { }
		transfer(const eja::transfer& transfer) : object(transfer), m_updated(transfer.m_updated), m_type(transfer.m_type), m_size(transfer.m_size), m_offset(transfer.m_offset) { }
		transfer(const transfer_type type) : m_type(type) { }
		transfer(const std::string& id) : object(id) { }
		transfer(const char* id) : object(id) { }
		virtual ~transfer() override { }

		// Interface
		virtual void clear() override;
		virtual void update(const u64 size);
		virtual void reset();

		// Utility
		bool empty() const { return !m_size; }
		bool updated() const { return m_updated; }
		bool is_download() const { return m_type == transfer_type::download; }
		bool is_upload() const { return m_type == transfer_type::upload; }

		void add_retry() { m_retries++; }

		// Mutator
		void set_type(const transfer_type type) { m_type = type; }
		void set_retries(const size_t retries) { m_retries = retries; }		
		void set_size(const u64 size);

		// Accessor
		transfer_type get_type() const { return m_type; }
		size_t get_retries() const { return m_retries; }
		u64 get_size() const { return m_size; }

		// Static
		static ptr create() { return std::make_shared<transfer>(); }
		static ptr create(const eja::transfer& transfer) { return std::make_shared<eja::transfer>(transfer); }
		static ptr create(const transfer::ptr transfer) { return std::make_shared<eja::transfer>(*transfer); }
		static ptr create(const transfer_type type) { return std::make_shared<transfer>(type); }
		static ptr create(const std::string& id) { return std::make_shared<transfer>(id); }
		static ptr create(const char* id) { return std::make_shared<transfer>(id); }		
	};

	// Container
	derived_type(transfer_list, mutex_list<transfer>);
	derived_type(transfer_map, mutex_map<std::string, transfer>);
	derived_type(transfer_queue, mutex_queue<transfer>);
	derived_type(transfer_vector, mutex_vector<transfer>);
}

#endif
