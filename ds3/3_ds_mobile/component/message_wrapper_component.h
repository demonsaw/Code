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

#ifndef _EJA_MESSAGE_WRAPPER_COMPONENT_H_
#define _EJA_MESSAGE_WRAPPER_COMPONENT_H_

#include <memory>

#include <QObject>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class message_model;

	class message_wrapper_component : public QObject, public component
	{
		make_default_factory(message_wrapper_component);

	private:
		message_model* m_model = nullptr;

	public:
		message_wrapper_component(QObject* parent = nullptr);
		message_wrapper_component(const message_wrapper_component& comp) = delete;

		// Operator
		message_wrapper_component& operator=(const message_wrapper_component& comp) = delete;

		// Interface
		virtual void init() override;

		// Get
		const message_model* get_model() const { return m_model; }
		message_model* get_model() { return m_model; }
	};
}

#endif