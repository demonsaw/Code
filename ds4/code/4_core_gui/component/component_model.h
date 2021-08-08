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

#ifndef _EJA_COMPONENT_MODEL_H_
#define _EJA_COMPONENT_MODEL_H_

#include <QObject>

#include "component/component_object.h"

namespace eja
{
	template <typename T>
	class component_model : public component_object
	{
	protected:
		T* m_model = nullptr;

	protected:
		component_model(QObject* parent = nullptr) : component_object(parent), m_model(new T(parent)) { }

	public:
		// Interface
		virtual void init() override;

		// Utility
		bool empty() const { return m_model->empty(); }
		size_t size() const { return m_model->size(); }

		// Get
		const T* get_model() const { return m_model; }
		T* get_model() { return m_model; }
	};

	// Interface
	template <typename T>
	void component_model<T>::init()
	{
		const auto owner = get_owner();
		m_model->set_entity(owner);
	}
}

#endif