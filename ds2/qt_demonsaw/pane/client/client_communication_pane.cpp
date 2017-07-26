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

#include <boost/lexical_cast.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QListView>
#include <QMediaPlayer>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "client_communication_pane.h"
#include "component/chat_component.h"
#include "component/client/client_option_component.h"
#include "pane/pane.h"
#include "resource/resource.h"
#include "utility/std.h"

namespace eja
{
	// Constructor
	client_communication_pane::client_communication_pane(entity::ptr entity, QWidget* parent /*= 0*/) : entity_pane(entity, parent)
	{
		create();
		layout();
		signal();
	}

	// Interface
	void client_communication_pane::create()
	{
		// Player
		m_player = new QMediaPlayer(this);

		// Chat
		m_chat_audio = new QCheckBox("Sound", this);
		m_chat_audio->setFixedWidth(73);

		m_chat_visual = new QCheckBox("Taskbar", this);
		m_chat_visual->setFixedWidth(72);

		m_chat_sound = new QLineEdit(this);
		m_chat_sound->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_chat_sound->setFixedWidth(230);
		m_chat_sound->setMaxLength(128);

		m_chat_sound_play = new QToolButton(this);
		m_chat_sound_play->setObjectName("pane");
		m_chat_sound_play->setIcon(QIcon(resource::menu::sound));
		m_chat_sound_play->setToolTip("Play");

		m_chat_sound_edit = new QToolButton(this);
		m_chat_sound_edit->setObjectName("pane");
		m_chat_sound_edit->setIcon(QIcon(resource::menu::edit));
		m_chat_sound_edit->setToolTip("Edit");

		m_chat_sound_clear = new QToolButton(this);
		m_chat_sound_clear->setObjectName("pane");
		m_chat_sound_clear->setIcon(QIcon(resource::menu::clear));
		m_chat_sound_clear->setToolTip("Clear");

		m_chat_timestamp = new QComboBox(this);
		m_chat_timestamp->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_chat_timestamp->setFixedWidth(230);
		m_chat_timestamp->setEditable(true);

		QListView* chat_timestamp_view = new QListView(this);
		chat_timestamp_view->setObjectName("pane");
		m_chat_timestamp->setView(chat_timestamp_view);
		m_chat_timestamp->addItem(default_chat::timestamp);
		m_chat_timestamp->addItem("%m-%d-%Y %I:%M:%S");
		m_chat_timestamp->addItem("%Y-%m-%d %H:%M:%S");
		m_chat_timestamp->addItem("%H:%M:%S");

		m_chat_timestamp_refresh = new QToolButton(this);
		m_chat_timestamp_refresh->setObjectName("pane");
		m_chat_timestamp_refresh->setIcon(QIcon(resource::menu::reset));
		m_chat_timestamp_refresh->setToolTip("Reset");

		m_chat_volume = new QSpinBox(this);
		m_chat_volume->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_chat_volume->setFixedWidth(112);
		m_chat_volume->setRange(default_chat::min_volume, default_chat::max_volume);
		m_chat_volume->setSuffix("%");

		m_chat_volume_refresh = new QToolButton(this);
		m_chat_volume_refresh->setObjectName("pane");
		m_chat_volume_refresh->setIcon(QIcon(resource::menu::reset));
		m_chat_volume_refresh->setToolTip("Reset");

		m_chat_history = new QSpinBox(this);
		m_chat_history->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_chat_history->setFixedWidth(112);
		m_chat_history->setRange(default_chat::min_history, default_chat::max_history);
		m_chat_history->setSuffix(" entries");

		m_chat_history_refresh = new QToolButton(this);
		m_chat_history_refresh->setObjectName("pane");
		m_chat_history_refresh->setIcon(QIcon(resource::menu::reset));
		m_chat_history_refresh->setToolTip("Reset");

		// Message
		m_message_audio = new QCheckBox("Sound", this);
		m_message_audio->setFixedWidth(73);

		m_message_visual = new QCheckBox("Taskbar", this);
		m_message_visual->setFixedWidth(72);

		m_message_sound = new QLineEdit(this);
		m_message_sound->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_message_sound->setFixedWidth(230);
		m_message_sound->setMaxLength(128);

		m_message_sound_play = new QToolButton(this);
		m_message_sound_play->setObjectName("pane");
		m_message_sound_play->setIcon(QIcon(resource::menu::sound));
		m_message_sound_play->setToolTip("Play");

		m_message_sound_edit = new QToolButton(this);
		m_message_sound_edit->setObjectName("pane");
		m_message_sound_edit->setIcon(QIcon(resource::menu::edit));
		m_message_sound_edit->setToolTip("Edit");

		m_message_sound_clear = new QToolButton(this);
		m_message_sound_clear->setObjectName("pane");
		m_message_sound_clear->setIcon(QIcon(resource::menu::clear));
		m_message_sound_clear->setToolTip("Clear");

		m_message_timestamp = new QComboBox(this);
		m_message_timestamp->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_message_timestamp->setFixedWidth(230);
		m_message_timestamp->setEditable(true);

		QListView* message_ttimestamp_view = new QListView(this);
		message_ttimestamp_view->setObjectName("pane");
		m_message_timestamp->setView(message_ttimestamp_view);
		m_message_timestamp->addItem(default_message::timestamp);
		m_message_timestamp->addItem("%m-%d-%Y %I:%M:%S");
		m_message_timestamp->addItem("%Y-%m-%d %H:%M:%S");
		m_message_timestamp->addItem("%H:%M:%S");

		m_message_timestamp_refresh = new QToolButton(this);
		m_message_timestamp_refresh->setObjectName("pane");
		m_message_timestamp_refresh->setIcon(QIcon(resource::menu::reset));
		m_message_timestamp_refresh->setToolTip("Reset");

		m_message_volume = new QSpinBox(this);
		m_message_volume->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_message_volume->setFixedWidth(112);
		m_message_volume->setRange(default_message::min_volume, default_message::max_volume);
		m_message_volume->setSuffix("%");

		m_message_volume_refresh = new QToolButton(this);
		m_message_volume_refresh->setObjectName("pane");
		m_message_volume_refresh->setIcon(QIcon(resource::menu::reset));
		m_message_volume_refresh->setToolTip("Reset");

		// Default
		const auto option = m_entity->get<client_option_component>();
		if (option)
		{
			// Chat						
			m_chat_visual->setChecked(option->has_chat_visual());
			m_chat_sound->setText(QString::fromStdString(option->get_chat_sound()));
			m_chat_volume->setValue(option->get_chat_volume());
			m_chat_history->setValue(option->get_chat_history());

			const auto chat_timestamp = option->get_chat_timestamp();
			const auto chat_qtimestamp = QString::fromStdString(chat_timestamp);
			const auto chat_timestamp_index = m_chat_timestamp->findText(chat_qtimestamp);
			if (chat_timestamp_index == -1)
				m_chat_timestamp->addItem(chat_qtimestamp);

			m_chat_timestamp->setCurrentText(chat_qtimestamp);

			const auto chat_audio = option->has_chat_audio();
			m_chat_audio->setChecked(chat_audio);
			m_chat_sound->setEnabled(chat_audio);
			m_chat_volume->setEnabled(chat_audio);
			m_chat_sound_play->setEnabled(chat_audio);
			m_chat_sound_edit->setEnabled(chat_audio);
			m_chat_sound_clear->setEnabled(chat_audio);
			m_chat_volume_refresh->setEnabled(chat_audio);

			// Message						
			m_message_visual->setChecked(option->has_message_visual());
			m_message_sound->setText(QString::fromStdString(option->get_message_sound()));
			m_message_volume->setValue(option->get_message_volume());

			const auto message_timestamp = option->get_message_timestamp();
			const auto message_qtimestamp = QString::fromStdString(message_timestamp);
			const auto message_timestamp_index = m_message_timestamp->findText(message_qtimestamp);
			if (message_timestamp_index == -1)
				m_message_timestamp->addItem(message_qtimestamp);

			m_message_timestamp->setCurrentText(message_qtimestamp);

			const auto message_audio = option->has_message_audio();
			m_message_audio->setChecked(message_audio);
			m_message_sound->setEnabled(message_audio);
			m_message_volume->setEnabled(message_audio);
			m_message_sound_play->setEnabled(message_audio);
			m_message_sound_edit->setEnabled(message_audio);
			m_message_sound_clear->setEnabled(message_audio);
			m_message_volume_refresh->setEnabled(message_audio);
		}
	}

	void client_communication_pane::layout()
	{
		// Chat
		QVBoxLayout* chat_layout = new QVBoxLayout(this);
		chat_layout->setSpacing(0);
		chat_layout->setMargin(6);

		pane::add_row(chat_layout, m_chat_audio, m_chat_visual);
		pane::add_spacing(chat_layout, 2);		
		pane::add_row(chat_layout, "Audio File", m_chat_sound, m_chat_sound_play, m_chat_sound_edit, m_chat_sound_clear);
		pane::add_spacing(chat_layout, 2);		
		pane::add_row(chat_layout, "Volume", m_chat_volume, m_chat_volume_refresh, "History", m_chat_history, m_chat_history_refresh);
		pane::add_spacing(chat_layout, 2); 
		pane::add_row(chat_layout, "Timestamp", m_chat_timestamp, m_chat_timestamp_refresh);

		QGroupBox* chat_group = new QGroupBox("Chat", this);
		chat_group->setLayout(chat_layout);

		// Message
		QVBoxLayout* message_layout = new QVBoxLayout(this);
		message_layout->setSpacing(0);
		message_layout->setMargin(6);

		pane::add_row(message_layout, m_message_audio, m_message_visual);
		pane::add_spacing(message_layout, 2);		
		pane::add_row(message_layout, "Audio File", m_message_sound, m_message_sound_play, m_message_sound_edit, m_message_sound_clear);		
		pane::add_spacing(message_layout, 2);
		pane::add_row(message_layout, "Volume", m_message_volume, m_message_volume_refresh);
		pane::add_spacing(message_layout, 2);
		pane::add_row(message_layout, "Timestamp", m_message_timestamp, m_message_timestamp_refresh);

		QGroupBox* message_group = new QGroupBox("Message", this);
		message_group->setLayout(message_layout);

		// Layout
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(6, 0, 0, 0);
		layout->setSpacing(0);

		layout->addWidget(chat_group);
		layout->addSpacing(6);

		layout->addWidget(message_group);
		layout->addStretch(1);

		setLayout(layout);
	}

	void client_communication_pane::signal()
	{
		// Chat
		connect(m_chat_audio, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				const auto audio = (state == Qt::Checked);
				option->set_chat_audio(audio);

				m_chat_sound->setEnabled(audio);
				m_chat_volume->setEnabled(audio);
				m_chat_sound_play->setEnabled(audio);
				m_chat_sound_edit->setEnabled(audio);
				m_chat_sound_clear->setEnabled(audio);
				m_chat_volume_refresh->setEnabled(audio);
			}
		});

		connect(m_chat_visual, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				const auto visual = (state == Qt::Checked);
				option->set_chat_visual(visual);
			}
		});

		connect(m_chat_timestamp, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [=](const QString& str)
		{
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			const auto& timestamp = option->get_chat_timestamp();
			const auto qtimestamp = str.toStdString();
			if (timestamp != qtimestamp)
				option->set_chat_timestamp(qtimestamp);
		});

		connect(m_chat_timestamp_refresh, &QToolButton::clicked, [this]()
		{
			if (m_chat_timestamp->currentIndex())
				m_chat_timestamp->setCurrentIndex(0);
		});

		connect(m_chat_sound, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_chat_sound(str.toStdString());
		});

		connect(m_chat_sound_play, &QToolButton::clicked, [this]()
		{
			if (m_player->state() == QMediaPlayer::PlayingState)
				return;

			const auto option = m_entity->get<client_option_component>();
			if (option && option->has_chat_audio())
			{
				if (!option->has_chat_sound())
					m_player->setMedia(QUrl(resource::audio::chat));
				else
					m_player->setMedia(QUrl::fromLocalFile(QString::fromStdString(option->get_chat_sound())));

				m_player->setVolume(option->get_chat_volume());
				m_player->play();
			}
		});

		connect(m_chat_sound_edit, &QToolButton::clicked, [this]()
		{
			QFileDialog dialog;
			dialog.setWindowTitle("Chat Alert");
			dialog.setFileMode(QFileDialog::ExistingFile);
			dialog.setOptions(QFileDialog::ReadOnly);
			dialog.setViewMode(QFileDialog::ViewMode::Detail);

			//"aac", "flac", "m4a", "mp3", "wav", "wma", "cue"
			QStringList filters;
			filters << "WAV (*.wav)" << "M4A (*.m4a)" << "MP3 (*.mp3)" << "WMA (*.wma)" << "All Files (*)";
			dialog.setNameFilters(filters);
			dialog.selectNameFilter("WAV (*.wav)");

			if (dialog.exec())
			{
				const auto qpaths = dialog.selectedFiles();
				const auto& qpath = qpaths.at(0);

				if (qpath != m_chat_sound->text())
				{
					m_chat_sound->setText(qpath);

					const auto option = m_entity->get<client_option_component>();
					if (option)
						option->set_chat_sound(qpath.toStdString());
				}
			}
		});

		connect(m_chat_sound_clear, &QToolButton::clicked, [this]()
		{
			const auto qsound = m_chat_sound->text();
			if (qsound.isEmpty())
				return;

			m_chat_sound->clear();
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_chat_sound();
		});

		connect(m_chat_volume, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				const auto& volume = option->get_chat_volume();
				const auto qvolume = static_cast<size_t>(value);

				if (volume != qvolume)
					option->set_chat_volume(qvolume);
			}
		});

		connect(m_chat_volume_refresh, &QToolButton::clicked, [this]()
		{
			m_chat_volume->setValue(default_chat::num_volume);

			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_chat_volume(default_chat::num_volume);
		});

		connect(m_chat_history, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				const auto history = option->get_chat_history();
				const auto qhistory = static_cast<size_t>(value);

				if (history != qhistory)
				{
					option->set_chat_history(qhistory);

					if (qhistory < history)
						m_entity->call(function_type::chat, function_action::clear, m_entity);
				}					
			}
		});

		connect(m_chat_history_refresh, &QToolButton::clicked, [this]()
		{
			m_chat_history->setValue(default_chat::num_history);

			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_chat_history(default_chat::num_history);
		});

		// Message
		connect(m_message_audio, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				const auto audio = (state == Qt::Checked);
				option->set_message_audio(audio);

				m_message_sound->setEnabled(audio);
				m_message_volume->setEnabled(audio);
				m_message_sound_play->setEnabled(audio);
				m_message_sound_edit->setEnabled(audio);
				m_message_sound_clear->setEnabled(audio);
				m_message_volume_refresh->setEnabled(audio);
			}
		});

		connect(m_message_visual, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				const auto visual = (state == Qt::Checked);
				option->set_message_visual(visual);
			}
		});

		connect(m_message_timestamp, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [=](const QString& str)
		{
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			const auto& timestamp = option->get_message_timestamp();
			const auto qtimestamp = str.toStdString();
			if (timestamp != qtimestamp)
				option->set_message_timestamp(qtimestamp);
		});

		connect(m_message_timestamp_refresh, &QToolButton::clicked, [this]()
		{
			if (m_message_timestamp->currentIndex())
				m_message_timestamp->setCurrentIndex(0);
		});

		connect(m_message_sound, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_message_sound(str.toStdString());
		});

		connect(m_message_sound_play, &QToolButton::clicked, [this]()
		{
			if (m_player->state() == QMediaPlayer::PlayingState)
				return;

			const auto option = m_entity->get<client_option_component>();
			if (option && option->has_message_audio())
			{
				if (!option->has_message_sound())
					m_player->setMedia(QUrl(resource::audio::message));
				else
					m_player->setMedia(QUrl::fromLocalFile(QString::fromStdString(option->get_message_sound())));

				m_player->setVolume(option->get_message_volume());
				m_player->play();
			}
		});

		connect(m_message_sound_edit, &QToolButton::clicked, [this]()
		{
			QFileDialog dialog;
			dialog.setWindowTitle("message Alert");
			dialog.setFileMode(QFileDialog::ExistingFile);
			dialog.setOptions(QFileDialog::ReadOnly);
			dialog.setViewMode(QFileDialog::ViewMode::Detail);

			//"aac", "flac", "m4a", "mp3", "wav", "wma", "cue"
			QStringList filters;
			filters << "WAV (*.wav)" << "M4A (*.m4a)" << "MP3 (*.mp3)" << "WMA (*.wma)" << "All Files (*)";
			dialog.setNameFilters(filters);
			dialog.selectNameFilter("WAV (*.wav)");

			if (dialog.exec())
			{
				const auto qpaths = dialog.selectedFiles();
				const auto& qpath = qpaths.at(0);

				if (qpath != m_message_sound->text())
				{
					m_message_sound->setText(qpath);

					const auto option = m_entity->get<client_option_component>();
					if (option)
						option->set_message_sound(qpath.toStdString());
				}
			}
		});

		connect(m_message_sound_clear, &QToolButton::clicked, [this]()
		{
			const auto qsound = m_message_sound->text();
			if (qsound.isEmpty())
				return;

			m_message_sound->clear();
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_message_sound();
		});

		connect(m_message_volume, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				const auto& volume = option->get_message_volume();
				const auto qvolume = static_cast<size_t>(value);

				if (volume != qvolume)
					option->set_message_volume(qvolume);
			}
		});

		connect(m_message_volume_refresh, &QToolButton::clicked, [this]()
		{
			m_message_volume->setValue(default_message::num_volume);

			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_message_volume(default_message::num_volume);
		});
	}
}
