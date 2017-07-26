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

#include <QHBoxLayout>

#include "status_window.h"

namespace eja
{
	// Constructor
	status_window::status_window(const size_t size, QWidget* parent /*= 0*/) : QWidget(parent)
	{
		// Create
		m_size = size;
		m_panes = new status_pane*[m_size];

		// Layout
		QHBoxLayout* layout = new QHBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(4);
		layout->setMargin(0);

		for (size_t i = 0; i < m_size; ++i)
		{
			m_panes[i] = new status_pane(this);
			layout->addWidget(m_panes[i]->get_image());
			layout->addWidget(m_panes[i]->get_text());
		}

		setLayout(layout);
	}

	status_window::~status_window()
	{
		for (size_t i = 0; i < m_size; ++i)
			delete m_panes[i];

		delete m_panes;
	}
}
