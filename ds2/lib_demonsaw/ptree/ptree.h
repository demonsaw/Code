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

#ifndef _EJA_PTREE_
#define _EJA_PTREE_

#include <iostream>
#include <boost/property_tree/ptree.hpp>

#include "ptree.h"
#include "object/object.h"
#include "system/type.h"

namespace eja
{
	// Empty
	inline const boost::property_tree::iptree& empty_ptree()
	{
		static boost::property_tree::iptree s_tree;
		return s_tree;
	}

	class ptree : public type
	{
	protected:
		boost::property_tree::iptree m_tree;

	public:
		using ptr = std::shared_ptr<ptree>;

	protected:
		ptree() { }
		ptree(const ptree&) = delete;
		virtual ~ptree() override { }

		// Utility
		void print(std::ostream& os) const { ptree::print(os, m_tree); }

	public:
		// Interface
		virtual void clear() { m_tree.clear(); }

		// Operator		
		friend std::ostream& operator<<(std::ostream& os, const ptree& base);

		// Utility
		virtual bool valid() const override { return has_tree(); }
		bool has_tree() const { return !m_tree.empty(); }

		// Static
		static void print(std::ostream& os, const boost::property_tree::iptree& tree, int level = 0);
	};
}

#endif
