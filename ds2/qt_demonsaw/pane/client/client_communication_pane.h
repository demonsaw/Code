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

#ifndef _EJA_CLIENT_COMMUNICATION_PANE_
#define _EJA_CLIENT_COMMUNICATION_PANE_

#include "entity/entity_pane.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QMediaPlayer;
class QSpinBox;
class QToolButton;

namespace eja
{
	class client_communication_pane final : public entity_pane
	{
	private:
		// Player
		QMediaPlayer* m_player = nullptr;

		// Chat
		QCheckBox* m_chat_audio = nullptr;
		QCheckBox* m_chat_visual = nullptr;

		QComboBox* m_chat_timestamp = nullptr;
		QToolButton* m_chat_timestamp_refresh = nullptr;

		QLineEdit* m_chat_sound = nullptr;
		QToolButton* m_chat_sound_play = nullptr;
		QToolButton* m_chat_sound_edit = nullptr;
		QToolButton* m_chat_sound_clear = nullptr;

		QSpinBox* m_chat_volume = nullptr;
		QToolButton* m_chat_volume_refresh = nullptr;

		QSpinBox* m_chat_history = nullptr;
		QToolButton* m_chat_history_refresh = nullptr;

		// Message
		QCheckBox* m_message_audio = nullptr;
		QCheckBox* m_message_visual = nullptr;

		QComboBox* m_message_timestamp = nullptr;
		QToolButton* m_message_timestamp_refresh = nullptr;

		QLineEdit* m_message_sound = nullptr;
		QToolButton* m_message_sound_play = nullptr;
		QToolButton* m_message_sound_edit = nullptr;
		QToolButton* m_message_sound_clear = nullptr;

		QSpinBox* m_message_volume = nullptr;
		QToolButton* m_message_volume_refresh = nullptr;

	protected:
		// Interface
		virtual void create() override;
		virtual void layout() override;
		virtual void signal() override;

	public:
		client_communication_pane(entity::ptr entity, QWidget* parent = 0);
		virtual ~client_communication_pane() override { }
	};
}

#endif
