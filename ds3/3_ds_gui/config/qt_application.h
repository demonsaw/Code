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

#ifndef _EJA_QT_APPLICATION_H_
#define _EJA_QT_APPLICATION_H_

#include <memory>

#include <QApplication>

#include "config/application.h"
#include "system/type.h"

namespace eja
{	
	class entity;

	class qt_application final : public application, public QApplication
	{
	public:
		using ptr = std::shared_ptr<qt_application>;

	private:
		// Utility
		void init_client(const std::shared_ptr<entity> entity);
		void init_group(const std::shared_ptr<entity> entity);

	public:
		qt_application(int argc, char* argv[]);

		// Interface
		virtual void init() override;
		virtual void update() override;
		virtual void shutdown() override;
		virtual void clear() override;

		// Utility
		using application::load;
		virtual void load(const char* psz) override;

		// Set
		virtual void set_theme() override;
		virtual void set_theme(const std::string& theme) override;

		// Static
		static ptr create(int argc, char* argv[]) { return ptr(new qt_application(argc, argv)); }
	};
}

#endif
