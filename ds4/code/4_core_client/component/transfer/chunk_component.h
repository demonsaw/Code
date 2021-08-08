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

#ifndef _EJA_CHUNK_COMPONENT_H_
#define _EJA_CHUNK_COMPONENT_H_

#include <memory>

#include "component/component.h"
#include "data/chunk_data.h"
#include "system/type.h"

namespace eja
{
	class chunk_component final : public component
	{
		make_factory(chunk_component);

	private:
		std::shared_ptr<chunk_data> m_chunk;

	public:
		chunk_component();

		// Interface
		virtual void clear() override;
		virtual bool valid() const override { return has_sequence() && has_data(); }

		// Utility
		void reset() { m_chunk->reset(); }

		// Has
		bool has_size() const { return m_chunk->has_size(); }
		bool has_data() const { return m_chunk->has_data(); }
		bool has_sequence() const { return m_chunk->has_sequence(); }

		// Set
		void set_chunk(const std::shared_ptr<chunk_data>& chunk) { m_chunk = chunk; }
		void set_data(std::string&& data) { m_chunk->set_data(std::move(data)); }
		void set_data(const std::string& data) { m_chunk->set_data(data); }
		void set_sequence(const size_t sequence) { m_chunk->set_sequence(sequence); }

		// Get
		std::string& get_data() { return m_chunk->get_data(); }
		const std::string& get_data() const { return m_chunk->get_data(); }		

		std::shared_ptr<chunk_data> get_chunk() const { return m_chunk; }		
		size_t get_sequence() const { return m_chunk->get_sequence(); }
		size_t get_size() const { return m_chunk->get_size(); }
	};
}

#endif
