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

#include <QApplication>
#include <QComboBox>
#include <QFont>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QSize>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>

#include "component/client/client_component.h"
#include "component/session/session_option_component.h"

#include "font/font_awesome.h"
#include "resource/resource.h"
#include "security/algorithm/diffie_hellman.h"
#include "security/cipher/cipher.h"
#include "security/hash/hash.h"
#include "utility/value.h"
#include "widget/help_dialog.h"
#include "widget/option/router_option_widget.h"

Q_DECLARE_METATYPE(eja::cipher_bits);

namespace eja
{
	router_option_widget::router_option_widget(const entity::ptr& entity, QWidget* parent /* = nullptr */) : option_pane(entity, parent)
	{
		m_layout = resource::get_vbox_layout(this);

		layout();
		signal();

		setLayout(m_layout);
	}

	void router_option_widget::layout()
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

		// Router
		{
			// Title
			{
				auto label = new QLabel("Session", this);
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

			// Router
			{
				const auto hlayout = resource::get_hbox_layout();
				hlayout->setSpacing(resource::get_horizontal_spacing() * 2);

				// Prime
				{
					const auto vlayout = resource::get_vbox_layout();
					vlayout->setSpacing(resource::get_vertical_spacing() / 2);

					auto label = new QLabel("Prime Size", this);
					label->setFont(label_font);
					vlayout->addWidget(label);
					vlayout->addSpacing(resource::get_vertical_spacing());

					m_prime_size = new QSpinBox(this);
					m_prime_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_prime_size->setFixedHeight(resource::get_combo_box_height());
					m_prime_size->setFixedWidth(resource::get_combo_box_width());
					m_prime_size->setSingleStep(128);
					m_prime_size->setRange(128, 4096);
					vlayout->addWidget(m_prime_size);
					vlayout->addSpacing(resource::get_vertical_spacing());

					// Data
					const auto session_option = m_entity->get<session_option_component>();
					m_prime_size->setValue(session_option->get_prime_size());

					hlayout->addLayout(vlayout);
				}

				// Cipher
				{
					const auto vvlayout = resource::get_vbox_layout();
					vvlayout->setSpacing(resource::get_vertical_spacing() / 2);

					auto label = new QLabel("Cipher", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					m_session_cipher = new QComboBox(this);
					m_session_cipher->setObjectName("options");
					m_session_cipher->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_session_cipher->setFixedHeight(resource::get_combo_box_height());
					m_session_cipher->setFixedWidth(resource::get_combo_box_width());

					m_session_cipher->addItem(cipher_name::aes);
					m_session_cipher->addItem(cipher_name::mars);
					m_session_cipher->addItem(cipher_name::rc6);
					m_session_cipher->addItem(cipher_name::serpent);
					m_session_cipher->addItem(cipher_name::twofish);

					for (int i = 0; i < m_session_cipher->count(); ++i)
					{
						auto model = m_session_cipher->model();
						model->setData(model->index(i, 0), QSize(resource::get_combo_box_width(), resource::get_combo_box_height()), Qt::SizeHintRole);
					}

					// Data
					const auto session_option = m_entity->get<session_option_component>();
					m_session_cipher->setCurrentText(QString::fromStdString(session_option->get_cipher()));

					vvlayout->addWidget(m_session_cipher);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					hlayout->addLayout(vvlayout);
				}

				// Key Size
				{
					const auto vvlayout = resource::get_vbox_layout();
					vvlayout->setSpacing(resource::get_vertical_spacing() / 2);

					auto label = new QLabel("Key Size", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					m_session_key_size = new QComboBox(this);
					m_session_key_size->setObjectName("options");
					m_session_key_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_session_key_size->setFixedHeight(resource::get_combo_box_height());
					m_session_key_size->setFixedWidth(resource::get_combo_box_width());

					m_session_key_size->addItem("128", QVariant::fromValue(cipher_bits::small));
					m_session_key_size->addItem("192", QVariant::fromValue(cipher_bits::medium));
					m_session_key_size->addItem("256", QVariant::fromValue(cipher_bits::large));

					for (int i = 0; i < m_session_key_size->count(); ++i)
					{
						auto model = m_session_key_size->model();
						model->setData(model->index(i, 0), QSize(resource::get_combo_box_width(), resource::get_combo_box_height()), Qt::SizeHintRole);
					}

					// Data
					const auto session_option = m_entity->get<session_option_component>();
					const auto index = m_session_key_size->findData(QVariant::fromValue<uint>(session_option->get_key_size()));
					m_session_key_size->setCurrentIndex(index >= 0 ? index : 0);

					vvlayout->addWidget(m_session_key_size);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					hlayout->addLayout(vvlayout);
				}

				// Hash
				{
					const auto vvlayout = resource::get_vbox_layout();
					vvlayout->setSpacing(resource::get_vertical_spacing() / 2);

					auto label = new QLabel("Hash", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					m_session_hash = new QComboBox(this);
					m_session_hash->setObjectName("options");
					m_session_hash->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_session_hash->setFixedHeight(resource::get_combo_box_height());
					m_session_hash->setFixedWidth(resource::get_combo_box_width());

					m_session_hash->addItem(hash_name::md5);
					m_session_hash->addItem(hash_name::sha1);
					m_session_hash->addItem(hash_name::sha224);
					m_session_hash->addItem(hash_name::sha256);
					m_session_hash->addItem(hash_name::sha384);
					m_session_hash->addItem(hash_name::sha512);

					for (int i = 0; i < m_session_hash->count(); ++i)
					{
						auto model = m_session_hash->model();
						model->setData(model->index(i, 0), QSize(resource::get_combo_box_width(), resource::get_combo_box_height()), Qt::SizeHintRole);
					}

					// Data
					const auto session_option = m_entity->get<session_option_component>();
					m_session_hash->setCurrentText(QString::fromStdString(session_option->get_hash()));

					vvlayout->addWidget(m_session_hash);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					hlayout->addLayout(vvlayout);
				}

				// Iterations
				{
					const auto vvlayout = resource::get_vbox_layout();
					vvlayout->setSpacing(resource::get_vertical_spacing() / 2);

					auto label = new QLabel("Iterations", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					m_session_iterations = new QSpinBox(this);
					m_session_iterations->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_session_iterations->setFixedHeight(resource::get_combo_box_height());
					m_session_iterations->setFixedWidth(resource::get_combo_box_width());
					m_session_iterations->setRange(1, 1 << 17);

					// Data
					const auto session_option = m_entity->get<session_option_component>();
					m_session_iterations->setValue(session_option->get_iterations());

					vvlayout->addWidget(m_session_iterations);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					hlayout->addLayout(vvlayout);
				}

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);
			}

			// Salt
			{
				const auto vvlayout = resource::get_vbox_layout();
				vvlayout->setSpacing(resource::get_small_top_margin());
				vvlayout->addSpacing(resource::get_large_top_margin());

				auto label = new QLabel("Salt", this);
				label->setFont(label_font);
				vvlayout->addWidget(label);
				vvlayout->addSpacing(resource::get_vertical_spacing() / 2);

				m_session_salt = new QTextEdit(this);
				m_session_salt->setPlaceholderText("None");
				m_session_salt->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_session_salt->setFixedWidth(resource::get_text_edit_width() + (int)(resource::get_font_size() / 17) * 41);
				m_session_salt->setFixedHeight(resource::get_text_edit_height());

				// Data
				const auto session_option = m_entity->get<session_option_component>();
				m_session_salt->setText(QString::fromStdString(session_option->get_salt()));

				vvlayout->addWidget(m_session_salt);
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
		m_router = new QGroupBox(this);
		m_router->setFlat(true);
		m_router->setLayout(layout);
		m_layout->addWidget(m_router);
	}

	// Utility
	void router_option_widget::help()
	{
		const char* text = "<b>Overview</b><br>" \
			"This tab manages the way your user connects to the router. &nbsp;You can increase or decrease the transmission security level.<br><br>" \
			"<b>Prime Size</b><br>The number of bits in the 'safe prime' number used to secure the communications channel. &nbsp;Larger primes are more secure, but take longer to generate.<br><br>" \
			"<b>Cipher</b><br>The cryptographic primitive used to encrypt data sent between client and router. &nbsp;All five finalists from the NIST competition are available.<br><br>" \
			"<b>Key Size</b><br>The number of bits in the cipher's key space.<br><br>" \
			"<b>Hash</b><br>The hash function primitive used in message authentication between client and router.<br><br>" \
			"<b>Iterations</b><br>The number of times the hash function is applied via the PBKDF algorithm.<br><br>" \
			"<b>Salt</b><br>A string appended to the inputs of the hash function. &nbsp;Enter anything or leave it blank." \
			"<br>";

		help_dialog dlg(text, this);
		dlg.setWindowTitle("Router Help");
		dlg.exec();
	}

	void router_option_widget::reset()
	{
		const auto prime_size = diffie_hellman::get_min_size();
		m_prime_size->setValue(prime_size);
		emit m_prime_size->valueChanged(prime_size);

		QString qcipher(cipher_name::aes);
		m_session_cipher->setCurrentText(qcipher);
		emit m_session_cipher->currentTextChanged(qcipher);

		const auto key_size = cipher_bits::large;
		m_session_key_size->setCurrentIndex(2);
		emit m_session_key_size->currentIndexChanged(2);

		QString qhash(hash_name::sha256);
		m_session_hash->setCurrentText(qhash);
		emit m_session_hash->currentTextChanged(qhash);

		const auto iterations = 1;
		m_session_iterations->setValue(iterations);
		emit m_session_iterations->valueChanged(iterations);

		QString qsalt;
		m_session_salt->setText(qsalt);
		emit m_session_salt->textChanged();
	}

	// Signal
	void router_option_widget::signal()
	{
		// Prime
		connect(m_prime_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto session_option = m_entity->get<session_option_component>();
			const auto size = session_option->get_prime_size();
			const auto qsize = static_cast<size_t>(value);

			if (size != qsize)
			{
				session_option->set_prime_size(qsize);
				emit prime_changed();
			}
		});

		connect(m_session_cipher, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [=](const QString& str)
		{
			const auto session_option = m_entity->get<session_option_component>();
			const auto& cipher = session_option->get_cipher();
			const auto qcipher = str.toStdString();

			if (cipher != qcipher)
			{
				session_option->set_cipher(qcipher);
				emit cipher_changed();
			}
		});

		connect(m_session_key_size, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
		{
			const auto variant = m_session_key_size->itemData(index);
			if (!variant.isValid())
				return;

			const auto session_option = m_entity->get<session_option_component>();
			const auto key_size = session_option->get_key_size();
			const auto qkey_size = variant.toUInt();

			if (key_size != qkey_size)
			{
				session_option->set_key_size(qkey_size);
				emit key_changed();
			}
		});

		connect(m_session_hash, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [=](const QString& str)
		{
			const auto session_option = m_entity->get<session_option_component>();
			const auto& hash = session_option->get_hash();
			const auto qhash = str.toStdString();

			if (hash != qhash)
			{
				session_option->set_hash(qhash);
				emit hash_changed();
			}
		});

		connect(m_session_iterations, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto session_option = m_entity->get<session_option_component>();
			const auto iterations = session_option->get_iterations();
			const auto qiterations = static_cast<size_t>(value);

			if (iterations != qiterations)
			{
				session_option->set_iterations(qiterations);
				emit iterations_changed();
			}
		});

		connect(m_session_salt, &QTextEdit::textChanged, [=]()
		{
			auto qsalt = m_session_salt->toPlainText().trimmed();

			if (qsalt.length() > io::max_salt_size)
			{
				qsalt.chop(qsalt.length() - io::max_salt_size);
				m_session_salt->setText(qsalt);

				QTextCursor cursor(m_session_salt->textCursor());
				cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
				m_session_salt->setTextCursor(cursor);
			}
			else
			{
				const auto session_option = m_entity->get<session_option_component>();
				const auto& salt = qsalt.toStdString();

				if (session_option->get_salt() != salt)
				{
					session_option->set_salt(salt);
					emit salt_changed();
				}
			}
		});
	}
}
