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

#include <QComboBox>
#include <QGroupBox>
#include <QListView>
#include <QTextEdit>
#include <QToolButton>
#include <QVariant>
#include <QVBoxLayout>

#include "client_security_pane.h"
#include "component/client/client_component.h"
#include "component/client/client_machine_component.h"
#include "component/client/client_security_component.h"
#include "object/client/client_security.h"
#include "pane/pane.h"
#include "security/hash.h"
#include "security/hmac.h"

namespace eja
{
	/*
		<64	<768	<128	INSECURE	Considered to be insecure
		64	768	128	VERY WEAK	Short term protection against individuals
		72	1008	160	WEAK	Short term protection against small organizations
		80	1024	160	LOW	Very short term protection against agencies (corresponds to ENISA legacy level)
		96	1776	192	LEGACY	Legacy standard level
		112	2048	224	MEDIUM	Medium-term protection
		128	3072	256	HIGH	Long term protection (corresponds to ENISA future level)
		256	15424	512	ULTRA	Foreseeable future

		Security Bits 	Symmetric Key 	FF 	IF 	EC
		Table 1: Comparable Algorithm Strengths 80 	2TDEA† 	L=1024, N=160 	k=1024 	f=160-223
		112 	3TDEA† 	L=2048, N=224 	k=2048 	f=224-255
		128 	AES 	L=3072, N=256 	k=3072 	f=256-383
		192 	AES 	L=7680, N=384 	k=7680 	f=384-511
		256 	AES 	L=15360, N=511 	k=15360 	f=512+
	*/
	 
	// Constructor
	client_security_pane::client_security_pane(entity::ptr entity, QWidget* parent /*= 0*/) : entity_pane(entity, parent)
	{
		create();
		layout();
		signal();
		init();
	}

	// Interface
	void client_security_pane::create()
	{
		// Message Router
		m_message_key_size = new QComboBox(this);
		m_message_key_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_message_key_size->setObjectName("pane");
		m_message_key_size->setFixedWidth(112);

		QListView* message_key_view = new QListView(this);
		message_key_view->setObjectName("pane");
		m_message_key_size->setView(message_key_view);

		m_message_key_size->addItem("128 bits", 128);
		m_message_key_size->addItem("192 bits", 192);
		m_message_key_size->addItem("256 bits", 256);

		m_message_key_size_refresh = new QToolButton(this);
		m_message_key_size_refresh->setObjectName("pane");
		m_message_key_size_refresh->setIcon(QIcon(resource::menu::reset));
		m_message_key_size_refresh->setToolTip("Reset");

		m_message_prime_size = new QComboBox(this);
		m_message_prime_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_message_prime_size->setObjectName("pane");
		m_message_prime_size->setFixedWidth(112);

		QListView* message_prime_view = new QListView(this);
		message_prime_view->setObjectName("pane");
		m_message_prime_size->setView(message_prime_view);

		m_message_prime_size->addItem("128 bits", 128);
		m_message_prime_size->addItem("768 bits", 768);
		m_message_prime_size->addItem("1024 bits", 1024);
		m_message_prime_size->addItem("1776 bits", 1776);
		m_message_prime_size->addItem("2048 bits", 2048);
		m_message_prime_size->addItem("3072 bits", 3072);
		m_message_prime_size->addItem("4096 bits", 4096);

		m_message_prime_size_refresh = new QToolButton(this);
		m_message_prime_size_refresh->setObjectName("pane");
		m_message_prime_size_refresh->setIcon(QIcon(resource::menu::reset));
		m_message_prime_size_refresh->setToolTip("Reset");

		// Transfer Router
		m_transfer_key_size = new QComboBox(this);
		m_transfer_key_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_transfer_key_size->setObjectName("pane");
		m_transfer_key_size->setFixedWidth(112);

		QListView* transfer_key_view = new QListView(this);
		transfer_key_view->setObjectName("pane");
		m_transfer_key_size->setView(transfer_key_view);

		m_transfer_key_size->addItem("128 bits", 128);
		m_transfer_key_size->addItem("192 bits", 192);
		m_transfer_key_size->addItem("256 bits", 256);

		m_transfer_key_size_refresh = new QToolButton(this);
		m_transfer_key_size_refresh->setObjectName("pane");
		m_transfer_key_size_refresh->setIcon(QIcon(resource::menu::reset));
		m_transfer_key_size_refresh->setToolTip("Reset");

		m_transfer_prime_size = new QComboBox(this);
		m_transfer_prime_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_transfer_prime_size->setObjectName("pane");
		m_transfer_prime_size->setFixedWidth(112);

		QListView* transfer_prime_view = new QListView(this);
		transfer_prime_view->setObjectName("pane");
		m_transfer_prime_size->setView(transfer_prime_view);

		m_transfer_prime_size->addItem("128 bits", 128);
		m_transfer_prime_size->addItem("768 bits", 768);
		m_transfer_prime_size->addItem("1024 bits", 1024);
		m_transfer_prime_size->addItem("1776 bits", 1776);
		m_transfer_prime_size->addItem("2048 bits", 2048);
		m_transfer_prime_size->addItem("3072 bits", 3072);
		m_transfer_prime_size->addItem("4096 bits", 4096);

		m_transfer_prime_size_refresh = new QToolButton(this);
		m_transfer_prime_size_refresh->setObjectName("pane");
		m_transfer_prime_size_refresh->setIcon(QIcon(resource::menu::reset));
		m_transfer_prime_size_refresh->setToolTip("Reset");

		// File/Folder
		m_hash = new QComboBox(this);
		m_hash->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_hash->setObjectName("pane");
		m_hash->setFixedWidth(112);

		QListView* hash_view = new QListView(this);
		hash_view->setObjectName("pane");
		m_hash->setView(hash_view);

		m_hash->addItem("MD5", static_cast<int>(client_hash_type::md5));
		m_hash->addItem("SHA1", static_cast<int>(client_hash_type::sha1));
		m_hash->addItem("SHA224", static_cast<int>(client_hash_type::sha224));
		m_hash->addItem("SHA256", static_cast<int>(client_hash_type::sha256));
		m_hash->addItem("SHA384", static_cast<int>(client_hash_type::sha384));
		m_hash->addItem("SHA512", static_cast<int>(client_hash_type::sha512));
		m_hash->addItem("SHA3 224", static_cast<int>(client_hash_type::sha3_224));
		m_hash->addItem("SHA3 256", static_cast<int>(client_hash_type::sha3_256));
		m_hash->addItem("SHA3 384", static_cast<int>(client_hash_type::sha3_384));
		m_hash->addItem("SHA3 512", static_cast<int>(client_hash_type::sha3_512));

		m_hash_refresh = new QToolButton(this);
		m_hash_refresh->setObjectName("pane");
		m_hash_refresh->setIcon(QIcon(resource::menu::reset));
		m_hash_refresh->setToolTip("Reset");

		m_salt = new QTextEdit(this);
		m_salt->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_salt->setFixedWidth(230);
		m_salt->setFixedHeight(55);

		m_salt_clear = new QToolButton(this);
		m_salt_clear->setObjectName("pane");
		m_salt_clear->setIcon(QIcon(resource::menu::clear));
		m_salt_clear->setToolTip("Clear");

		// Default
		const auto security = m_entity->get<client_security_component>();
		if (security)
		{
			// Message Router
			const auto message_key_size = security->get_message_key_size();
			const auto message_qkey_size = QString::number(message_key_size);
			const auto message_key_size_index = m_message_key_size->findData(message_qkey_size);
			m_message_key_size->setCurrentIndex((message_key_size_index >= 0) ? message_key_size_index : 0);

			const auto message_prime_size = security->get_message_prime_size();
			const auto message_qprime_size = QString::number(message_prime_size);
			const auto message_prime_size_index = m_message_prime_size->findData(message_qprime_size);
			m_message_prime_size->setCurrentIndex((message_prime_size_index >= 0) ? message_prime_size_index : 0);

			// Transfer Router
			const auto transfer_key_size = security->get_transfer_key_size();
			const auto transfer_qkey_size = QString::number(transfer_key_size);
			const auto transfer_key_size_index = m_transfer_key_size->findData(transfer_qkey_size);
			m_transfer_key_size->setCurrentIndex((transfer_key_size_index >= 0) ? transfer_key_size_index : 0);

			const auto transfer_prime_size = security->get_transfer_prime_size();
			const auto transfer_qprime_size = QString::number(transfer_prime_size);
			const auto transfer_prime_size_index = m_transfer_prime_size->findData(transfer_qprime_size);
			m_transfer_prime_size->setCurrentIndex((transfer_prime_size_index >= 0) ? transfer_prime_size_index : 0);

			// File/Folder
			const auto hash_type = security->get_hash_type();
			const auto hash_index = m_hash->findData(static_cast<int>(hash_type));
			m_hash->setCurrentIndex((hash_index >= 0) ? hash_index : 0);

			m_salt->setText(QString::fromStdString(security->get_salt()));
		}
	}

	void client_security_pane::layout()
	{
		// Message Router
		QVBoxLayout* message_layout = new QVBoxLayout(this);
		message_layout->setSpacing(0);
		message_layout->setMargin(6);

		pane::add_row(message_layout, "Key Size", m_message_key_size, m_message_key_size_refresh, "Prime Size", m_message_prime_size, m_message_prime_size_refresh);
		pane::add_spacing(message_layout, 2);

		QGroupBox* message_group = new QGroupBox("Message Router", this);
		message_group->setLayout(message_layout);

		// Transfer Router
		QVBoxLayout* transfer_layout = new QVBoxLayout(this);
		transfer_layout->setSpacing(0);
		transfer_layout->setMargin(6);

		pane::add_row(transfer_layout, "Key Size", m_transfer_key_size, m_transfer_key_size_refresh, "Prime Size", m_transfer_prime_size, m_transfer_prime_size_refresh);
		pane::add_spacing(transfer_layout, 2);

		QGroupBox* transfer_group = new QGroupBox("Transfer Router", this);
		transfer_group->setLayout(transfer_layout);

		// Hash
		QVBoxLayout* hash_layout = new QVBoxLayout(this);
		hash_layout->setSpacing(0);
		hash_layout->setMargin(6);

		pane::add_row(hash_layout, "Hash", m_hash, m_hash_refresh);
		pane::add_spacing(hash_layout, 2);

		pane::add_row(hash_layout, "Salt", m_salt, m_salt_clear);
		pane::add_spacing(hash_layout, 2);

		QGroupBox* hash_group = new QGroupBox("File/Folder", this);
		hash_group->setLayout(hash_layout);

		// Layout
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(6, 0, 0, 0);
		layout->setSpacing(0);

		layout->addWidget(message_group);
		layout->addSpacing(6); 

		layout->addWidget(transfer_group);
		layout->addSpacing(6);
		
		layout->addWidget(hash_group);
		layout->addStretch(1);

		setLayout(layout);
	}

	void client_security_pane::signal()
	{
		// Message Router
		connect(m_message_key_size, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
		{
			const auto security = m_entity->get<client_security_component>();
			if (!security)
				return;

			const auto& key_size = security->get_message_key_size();
			const auto qkey_size = m_message_key_size->itemData(index).toUInt();

			if (key_size != qkey_size)
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				security->set_message_key_size(qkey_size);
			}
		});

		connect(m_message_key_size_refresh, &QToolButton::clicked, [this]()
		{
			const auto security = m_entity->get<client_security_component>();
			if (!security)
				return;
			
			const auto& key_size = security->get_message_key_size();			
			const auto qkey_size = default_security::message_key_size;

			if (key_size != qkey_size)
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				security->set_message_key_size(qkey_size);

				const auto index = m_message_key_size->findData(static_cast<uint>(qkey_size));
				m_message_key_size->setCurrentIndex(index);
			}
		});

		connect(m_message_prime_size, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
		{
			const auto security = m_entity->get<client_security_component>();
			if (!security)
				return;

			const auto prime_size = security->get_message_prime_size();
			const auto qprime_size = m_message_prime_size->itemData(index).toUInt();

			if ((prime_size != qprime_size) && (qprime_size >= default_security::min_prime_size))
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				security->set_message_prime_size(qprime_size);
			}
		});

		connect(m_message_prime_size_refresh, &QToolButton::clicked, [this]()
		{
			const auto security = m_entity->get<client_security_component>();
			if (!security)
				return;

			const auto& prime_size = security->get_message_prime_size();
			const auto qprime_size = default_security::message_prime_size;

			if (prime_size != qprime_size)
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				security->set_message_prime_size(qprime_size);

				const auto index = m_message_prime_size->findData(static_cast<uint>(qprime_size));
				m_message_prime_size->setCurrentIndex(index);
			}
		});

		// Transfer Router
		connect(m_transfer_key_size, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
		{
			const auto security = m_entity->get<client_security_component>();
			if (!security)
				return;

			const auto& key_size = security->get_transfer_key_size();
			const auto qkey_size = m_transfer_key_size->itemData(index).toUInt();

			if (key_size != qkey_size)
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				security->set_transfer_key_size(qkey_size);
			}
		});

		connect(m_transfer_key_size_refresh, &QToolButton::clicked, [this]()
		{
			const auto security = m_entity->get<client_security_component>();
			if (!security)
				return;

			const auto& key_size = security->get_transfer_key_size();
			const auto qkey_size = default_security::transfer_key_size;

			if (key_size != qkey_size)
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				security->set_transfer_key_size(qkey_size);

				const auto index = m_transfer_key_size->findData(static_cast<uint>(qkey_size));
				m_transfer_key_size->setCurrentIndex(index);
			}
		});

		connect(m_transfer_prime_size, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
		{
			const auto security = m_entity->get<client_security_component>();
			if (!security)
				return;

			const auto prime_size = security->get_transfer_prime_size();
			const auto qprime_size = m_transfer_prime_size->itemData(index).toUInt();

			if ((prime_size != qprime_size) && (qprime_size >= default_security::min_prime_size))
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				security->set_transfer_prime_size(qprime_size);
			}
		});

		connect(m_transfer_prime_size_refresh, &QToolButton::clicked, [this]()
		{
			const auto security = m_entity->get<client_security_component>();
			if (!security)
				return;

			const auto& prime_size = security->get_transfer_prime_size();
			const auto qprime_size = default_security::transfer_prime_size;

			if (prime_size != qprime_size)
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				security->set_transfer_prime_size(qprime_size);

				const auto index = m_transfer_prime_size->findData(static_cast<uint>(qprime_size));
				m_transfer_prime_size->setCurrentIndex(index);
			}
		});

		// File/Folder
		connect(m_hash, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
		{
			const auto security = m_entity->get<client_security_component>();
			if (!security)
				return;

			const auto hash_type = security->get_hash_type();
			const auto qhash_type = m_hash->itemData(index).toUInt();

			if (hash_type != qhash_type)
				security->set_hash_type(qhash_type);
		});

		connect(m_hash_refresh, &QToolButton::clicked, [this]()
		{
			if (m_hash->currentIndex())
				m_hash->setCurrentIndex(0);
		});

		connect(m_salt, &QTextEdit::textChanged, [=]()
		{
			const auto security = m_entity->get<client_security_component>();
			if (!security)
				return;

			const auto& salt = security->get_salt();
			const auto qsalt = m_salt->toPlainText().toStdString();

			if (salt != qsalt)
				security->set_salt(qsalt);
		});

		connect(m_salt_clear, &QToolButton::clicked, [this]()
		{
			const auto qsalt = m_salt->toPlainText();
			if (qsalt.isEmpty())
				return;

			m_salt->clear();
			const auto security = m_entity->get<client_security_component>();
			if (security)
				security->set_salt();
		});
	}
}
