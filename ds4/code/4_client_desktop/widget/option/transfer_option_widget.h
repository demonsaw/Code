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

#ifndef _EJA_TRANSFER_OPTION_WIDGET_H_
#define _EJA_TRANSFER_OPTION_WIDGET_H_

#include "entity/entity_widget.h"
#include "widget/option/option_pane.h"

class QGroupBox;
class QPushButton;
class QSpinBox;
class QVBoxLayout;

namespace eja
{
	class transfer_option_widget final : public option_pane
	{
		Q_OBJECT

	private:		
		QVBoxLayout* m_layout = nullptr;		

		// Download		
		QGroupBox* m_download_group = nullptr;
		QSpinBox* m_buffer = nullptr;
		QSpinBox* m_chunk = nullptr;
		QSpinBox* m_max_downloads = nullptr;		

		// Uploads
		QGroupBox* m_upload_group = nullptr;
		QSpinBox* m_max_uploads = nullptr;

	private:
		void layout();
		void signal();

	public:
		transfer_option_widget(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr);

		// Utility
		virtual void help();
		virtual void reset();

	signals:
		void max_downloads_changed();
		void max_uploads_changed();
	};
}

#endif
