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

#ifndef _EJA_VALUE_CONTROLLER_
#define _EJA_VALUE_CONTROLLER_

#include "controller.h"
#include "system/mutex/mutex_value.h"

namespace eja
{
	template <typename T>
	class value_controller : public mutex_value<T>, public controller
	{
	public:
		using ptr = std::shared_ptr<value_controller<T>>;

	public:
		value_controller() { }
		value_controller(const typename mutex_value<T>::value_type value) : mutex_value<T>(value) { }
		virtual ~value_controller() override { }

		// Interface
		virtual void init() override;
		virtual void update() override;
		virtual void shutdown() override;
		virtual void clear() override;

		// Static
		static ptr create() { return std::make_shared<value_component<T>>(); }
		static ptr create(const typename mutex_value<T>::value_type value) { return std::make_shared<value_component<T>>(value); }
	};

	// Interface
	template <typename T>
	void value_controller<T>::init()
	{
		auto_lock();
		mutex_value<T>::m_value->init();

		controller::init();
	}

	template <typename T>
	void value_controller<T>::update()
	{
		auto_lock();
		mutex_value<T>::m_value->update();

		controller::update();
	}

	template <typename T>
	void value_controller<T>::shutdown()
	{
		auto_lock();
		mutex_value<T>::m_value->shutdown();

		controller::shutdown();
	}

	template <typename T>
	void value_controller<T>::clear()
	{
		auto_lock();
		mutex_value<T>::m_value->clear();

		controller::clear();
	}
}

#endif
