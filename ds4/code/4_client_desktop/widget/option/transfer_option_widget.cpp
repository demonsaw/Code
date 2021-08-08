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

#include <cassert>

#include <QApplication>
#include <QFont>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include "component/client/client_component.h"
#include "component/transfer/transfer_service_component.h"

#include "font/font_awesome.h"
#include "object/download.h"
#include "object/upload.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "widget/help_dialog.h"
#include "widget/option/transfer_option_widget.h"

namespace eja
{
	transfer_option_widget::transfer_option_widget(const entity::ptr& entity, QWidget* parent /* = nullptr */) : option_pane(entity, parent)
	{
		m_layout = resource::get_vbox_layout(this);

		layout();
		signal();

		setLayout(m_layout);
	}

	void transfer_option_widget::layout()
	{
		// Font
		QFont awesome(software::font_awesome);
		awesome.setPixelSize(resource::get_icon_size());

		QFont label_font = QApplication::font();
		label_font.setFamily(font_family::main);
		label_font.setPixelSize(resource::get_label_font_size());

		const auto layout = resource::get_vbox_layout();

		// Download
		{			
			// Title
			{
				auto label = new QLabel("Downloads", this);
				label->setFont(label_font);

				const auto hlayout = resource::get_hbox_layout();
				hlayout->setSpacing(resource::get_horizontal_spacing());
				hlayout->addWidget(label);
				hlayout->addStretch(1);
				 
				layout->addLayout(hlayout);
				layout->addSpacing(resource::get_vertical_spacing());
			}

			const auto vlayout = resource::get_vbox_layout();
			vlayout->setMargin(resource::get_group_box_margin());

			// Download Options
			{
				const auto hlayout = resource::get_hbox_layout();
				hlayout->setSpacing(resource::get_horizontal_spacing() * 2);

				// Total # Downloads
				{
					const auto vvlayout = resource::get_vbox_layout();

					auto label = new QLabel("Total #", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					m_max_downloads = new QSpinBox(this);
					m_max_downloads->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_max_downloads->setFixedHeight(resource::get_combo_box_height());
					m_max_downloads->setFixedWidth(resource::get_combo_box_width());
					m_max_downloads->setRange(download::min_threads, download::max_threads);

					// Data
					const auto client = m_entity->get<client_component>();
					m_max_downloads->setValue(client->get_downloads());

					vvlayout->addWidget(m_max_downloads);
					hlayout->addLayout(vvlayout);
				}

				// Buffer #
				{
					const auto vvlayout = resource::get_vbox_layout();

					auto label = new QLabel("Buffer #", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					m_buffer = new QSpinBox(this);
					m_buffer->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_buffer->setFixedHeight(resource::get_combo_box_height());
					m_buffer->setFixedWidth(resource::get_combo_box_width());
					m_buffer->setRange(network::min_buffer_size, network::max_buffer_size);

					// Data
					const auto client = m_entity->get<client_component>();
					m_buffer->setValue(client->get_buffer_size());

					vvlayout->addWidget(m_buffer);
					hlayout->addLayout(vvlayout);
				}

				// Chunk
				{
					const auto vvlayout = resource::get_vbox_layout();

					auto label = new QLabel("Chunk Size", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					m_chunk = new QSpinBox(this);
					m_chunk->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_chunk->setFixedHeight(resource::get_combo_box_height());
					m_chunk->setFixedWidth(resource::get_combo_box_width());
					m_chunk->setRange(b_to_kb(network::min_chunk_size), b_to_kb(network::max_chunk_size));
					m_chunk->setSingleStep(4);
					m_chunk->setSuffix(" KB");

					// Data
					const auto client = m_entity->get<client_component>();
					m_chunk->setValue(b_to_kb(client->get_chunk_size()));

					vvlayout->addWidget(m_chunk);
					hlayout->addLayout(vvlayout);
				}

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);

				const auto widget = new QWidget(this);
				widget->setObjectName("border");
				widget->setLayout(vlayout);

				const auto hhlayout = resource::get_hbox_layout();
				hhlayout->addWidget(widget);

				layout->addLayout(hhlayout);
				layout->addSpacing(resource::get_top_margin() * 2);
			}
		}

		// Upload
		{
			// Title
			{
				auto label = new QLabel("Uploads", this);
				label->setFont(label_font);

				const auto hlayout = resource::get_hbox_layout();
				hlayout->addWidget(label);
				hlayout->addStretch(1);

				layout->addLayout(hlayout);
				layout->addSpacing(resource::get_vertical_spacing());
			}

			const auto vlayout = resource::get_vbox_layout();
			vlayout->setMargin(resource::get_group_box_margin());

			// Download Options
			{
				const auto hlayout = resource::get_hbox_layout();
				hlayout->setSpacing(resource::get_horizontal_spacing());

				// Total # Uploads
				{
					const auto vvlayout = resource::get_vbox_layout();

					auto label = new QLabel("Total #", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					m_max_uploads = new QSpinBox(this);
					m_max_uploads->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_max_uploads->setFixedHeight(resource::get_combo_box_height());
					m_max_uploads->setFixedWidth(resource::get_combo_box_width());
					m_max_uploads->setRange(upload::min_threads, upload::max_threads);

					// Data
					const auto client = m_entity->get<client_component>();
					m_max_uploads->setValue(client->get_uploads());

					vvlayout->addWidget(m_max_uploads);
					hlayout->addLayout(vvlayout);
				}
				
				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);

				const auto widget = new QWidget(this);
				widget->setObjectName("border");
				widget->setLayout(vlayout);

				const auto hhlayout = resource::get_hbox_layout();
				hhlayout->addWidget(widget);

				layout->addLayout(hhlayout);
				layout->addSpacing(resource::get_large_top_margin());
			}
		}

		// Group
		m_download_group = new QGroupBox(this);
		m_download_group->setFlat(true);
		m_download_group->setLayout(layout);

		m_layout->addWidget(m_download_group);
	}

	// Utility
	void transfer_option_widget::help()
	{
		const char* text = "<b>Overview</b><br>" \
			"This tab manages file upload/download settings. &nbsp;Please note that improper settings can seriously degrade transfer performance.<br><br>" \
			"<b>Total #</b><br>The maximum number of simultaneous downloads/uploads your client will perform. &nbsp;Reduce this if your connection is slow or you want to minimize concurrent uploads.<br><br>" \
			"<b>Buffer #</b><br>The number of chunks your client requests to be buffered by the router. &nbsp;The only time you might need to change this setting is if downloading from a large swarm of uploaders over a small network of transfer routers.<br><br>" \
			"<b>Chunk Size</b><br>The size of the file chunk your client requests from uploaders. &nbsp;For LAN-based networks increasing this value will also increase transfer speeds. &nbsp;If your connection is slow or intermittent, you should reduce chunk size to 256, 128, 64, or even lower in serious cases." \
			"<br>";

		help_dialog dlg(text, this);
		dlg.setWindowTitle("Transfer Help");
		dlg.exec();
	}

	void transfer_option_widget::reset()
	{
		const auto buffer_size = network::num_buffer_size;
		m_buffer->setValue(buffer_size);
		emit m_buffer->valueChanged(buffer_size);

		const auto chunk_size = b_to_kb(network::num_chunk_size);
		m_chunk->setValue(chunk_size);
		emit m_chunk->valueChanged(chunk_size);

		const auto max_download = download::num_threads;
		m_max_downloads->setValue(max_download);
		emit m_max_downloads->valueChanged(max_download);

		const auto max_uploads = upload::num_threads;
		m_max_uploads->setValue(max_uploads);
		emit m_max_uploads->valueChanged(max_uploads);
	}

	// Signal
	void transfer_option_widget::signal()
	{
		// Network
		connect(m_buffer, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			assert(thread_info::main());

			const auto qwindow = static_cast<size_t>(value);
			const auto client = m_entity->get<client_component>();
			client->set_buffer_size(qwindow);
		});

		connect(m_chunk, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			assert(thread_info::main());

			const auto qchunk = kb_to_b(static_cast<size_t>(value));
			const auto client = m_entity->get<client_component>();
			client->set_chunk_size(qchunk);
		});

		connect(m_max_downloads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			assert(thread_info::main());

			const auto qdownloads = static_cast<size_t>(value);
			const auto client = m_entity->get<client_component>();
			client->set_downloads(qdownloads);

			const auto download_list = m_entity->get<download_list_component>();			
			if (download_list->empty())
			{
				const auto upload_list = m_entity->get<upload_list_component>();
				if (upload_list->empty())
				{
					client->set_update(true);
					emit max_downloads_changed();
				}				
			}
		});

		connect(m_max_uploads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			assert(thread_info::main());

			const auto quploads = static_cast<size_t>(value);
			const auto client = m_entity->get<client_component>();
			client->set_uploads(quploads);

			const auto download_list = m_entity->get<download_list_component>();
			if (download_list->empty())
			{
				const auto upload_list = m_entity->get<upload_list_component>();
				if (upload_list->empty())
				{
					client->set_update(true);
					emit max_uploads_changed();
				}
			}			
		});
	}
}
