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

#include <QAbstractItemView>
#include <QApplication>
#include <QComboBox>
#include <QFont>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QSize>
#include <QTextEdit>
#include <QVBoxLayout>

#include "component/client/client_component.h"

#include "font/font_awesome.h"
#include "resource/resource.h"
#include "security/hash/hash.h"
#include "utility/value.h"
#include "widget/help_dialog.h"
#include "widget/option/system_option_widget.h"

namespace eja
{
	system_option_widget::system_option_widget(const entity::ptr& entity, QWidget* parent /* = nullptr */) : option_pane(entity, parent)
	{
		m_layout = resource::get_vbox_layout(this);

		layout();
		signal();

		setLayout(m_layout);
	}

	void system_option_widget::layout()
	{
		// Font
		QFont awesome(software::font_awesome);
		awesome.setPixelSize(resource::get_icon_size());

		// Layout
		const auto layout = resource::get_vbox_layout();

		// Label Font
		QFont label_font = QApplication::font();
		label_font.setFamily(font_family::main);
		label_font.setPixelSize(resource::get_label_font_size());

		// File/Folder
		{
			// Title
			{
				auto label = new QLabel("File/Folder", this);
				label->setFont(label_font);

				const auto hlayout = resource::get_hbox_layout();
				hlayout->setSpacing(resource::get_horizontal_spacing());
				hlayout->addWidget(label, 0, Qt::AlignBottom);
				hlayout->addStretch(1);

				layout->addLayout(hlayout);
				layout->addSpacing(resource::get_vertical_spacing());
			}

			const auto hlayout = resource::get_hbox_layout();
			hlayout->setSpacing(resource::get_top_margin());

			// Hash
			{
				const auto vvlayout = resource::get_vbox_layout();
				vvlayout->setSpacing(resource::get_vertical_spacing() / 2);

				auto label = new QLabel("Hash", this);
				label->setFont(label_font);
				vvlayout->addWidget(label);
				vvlayout->addSpacing(resource::get_vertical_spacing());

				m_io_hash = new QComboBox(this);
				m_io_hash->setObjectName("options");
				m_io_hash->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_io_hash->setFixedHeight(resource::get_combo_box_height());
				m_io_hash->setFixedWidth(resource::get_combo_box_width());

				m_io_hash->addItem(hash_name::md5);
				m_io_hash->addItem(hash_name::sha1);
				m_io_hash->addItem(hash_name::sha224);
				m_io_hash->addItem(hash_name::sha256);
				m_io_hash->addItem(hash_name::sha384);
				m_io_hash->addItem(hash_name::sha512);

				for (int i = 0; i < m_io_hash->count(); ++i)
				{
					auto model = m_io_hash->model();
					model->setData(model->index(i, 0), QSize(resource::get_combo_box_width(), resource::get_combo_box_height()), Qt::SizeHintRole);
				}

				// Data
				const auto client = m_entity->get<client_component>();
				m_io_hash->setCurrentText(QString::fromStdString(client->get_hash()));

				vvlayout->addWidget(m_io_hash);
				vvlayout->addSpacing(resource::get_vertical_spacing());

				hlayout->addLayout(vvlayout);
			}

			hlayout->addStretch(1);

			const auto vlayout = resource::get_vbox_layout();
			vlayout->setMargin(resource::get_group_box_margin());
			vlayout->addLayout(hlayout);

			// Salt
			{
				const auto vvlayout = resource::get_vbox_layout();
				vvlayout->setSpacing(resource::get_small_top_margin());
				vvlayout->addSpacing(resource::get_large_top_margin());

				auto label = new QLabel("Salt", this);
				label->setFont(label_font);
				vvlayout->addWidget(label);
				vvlayout->addSpacing(resource::get_vertical_spacing() / 2);

				m_io_salt = new QTextEdit(this);
				m_io_salt->setPlaceholderText("None");
				m_io_salt->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_io_salt->setFixedWidth(resource::get_text_edit_width() + (int)(resource::get_font_size() / 17) * 41);
				m_io_salt->setFixedHeight(resource::get_text_edit_height());

				// Data
				const auto client = m_entity->get<client_component>();
				m_io_salt->setText(QString::fromStdString(client->get_salt()));

				vvlayout->addWidget(m_io_salt);
				vvlayout->addSpacing(resource::get_vertical_spacing());

				vlayout->addLayout(vvlayout);
			}

			const auto widget = new QWidget(this);
			widget->setObjectName("border");
			widget->setLayout(vlayout);

			const auto hhlayout = resource::get_hbox_layout();
			hhlayout->addWidget(widget);

			layout->addLayout(hhlayout);
			layout->addSpacing(resource::get_large_top_margin());
		}

		// Group
		m_file = new QGroupBox(this);
		m_file->setFlat(true);
		m_file->setLayout(layout);
		m_layout->addWidget(m_file);
	}

	// Utility
	void system_option_widget::help()
	{
		const char* text = "<b>Overview</b><br>" \
			"This tab manages how the files/folders are uniquely represented to the rest of the network. &nbsp;Swarming (downloading a file from multiple clients simultaneously) will only work if clients have identical settings in this tab.<br><br>" \
			"<b>Hash</b><br>The hash function primitive used to uniquely identify files/folders.<br><br>" \
			"<b>Salt</b><br>A string appended to the inputs of the hash function. &nbsp;Enter anything or leave it blank." \
			"<br>";

		help_dialog dlg(text, this);
		dlg.setWindowTitle("System Help");
		dlg.exec();
	}

	void system_option_widget::reset()
	{
		QString qhash(hash_name::md5);
		m_io_hash->setCurrentText(qhash);
		emit m_io_hash->currentTextChanged(qhash);

		QString qsalt;
		m_io_salt->setText(qsalt);
		emit m_io_salt->textChanged();
	}

	// Signal
	void system_option_widget::signal()
	{
		connect(m_io_hash, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [=](const QString& str)
		{
			const auto client = m_entity->get<client_component>();
			const auto& hash = client->get_hash();
			const auto qhash = str.toStdString();

			if (hash != qhash)
			{
				client->set_hash(qhash);
				emit hash_changed();
			}
		});

		connect(m_io_salt, &QTextEdit::textChanged, [=]()
		{			
			auto qsalt = m_io_salt->toPlainText().trimmed();

			if (qsalt.length() > io::max_salt_size)
			{
				qsalt.chop(qsalt.length() - io::max_salt_size);
				m_io_salt->setText(qsalt);

				QTextCursor cursor(m_io_salt->textCursor());
				cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
				m_io_salt->setTextCursor(cursor);
			}
			else
			{
				const auto client = m_entity->get<client_component>();
				const auto& salt = qsalt.toStdString();

				if (client->get_salt() != salt)
				{
					client->set_salt(salt);
					emit salt_changed();
				}
			}
		});
	}
}
