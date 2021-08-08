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

#include "component/client/client_component.h"

#include "entity/entity.h"
#include "object/download.h"
#include "object/upload.h"
#include "security/hash/hash.h"

namespace eja
{
	// Constructor
	client_component::client_component() : name_component(bits::motd | bits::chat_alert | bits::chat_notify | bits::pm_beep | bits::pm_alert | bits::pm_notify | bits::pm_tray | bits::poll | bits::room_alert | bits::room_notify | bits::tunnel),
		m_hash(hash_name::md5), m_downloads(download::num_threads), m_uploads(upload::num_threads)
	{

	}

	// Interface
	void client_list_component::clear()
	{
		thread_safe_ex<component, std::deque<entity::ptr>>::clear();

		set_expanded(false);
	}

	// Set
	void client_component::set_mode(const size_t value)
	{
		set_motd((value & bits::motd) == bits::motd);
		set_poll((value & bits::poll) == bits::poll);
		set_tunnel((value & bits::tunnel) == bits::tunnel);

		// Chat
		set_chat_beep((value & bits::chat_beep) == bits::chat_beep);
		set_chat_alert((value & bits::chat_alert) == bits::chat_alert);
		set_chat_notify((value & bits::chat_notify) == bits::chat_notify);
		set_chat_tray((value & bits::chat_tray) == bits::chat_tray);

		// Message
		set_pm_beep((value & bits::pm_beep) == bits::pm_beep);
		set_pm_alert((value & bits::pm_alert) == bits::pm_alert);
		set_pm_notify((value & bits::pm_notify) == bits::pm_notify);
		set_pm_tray((value & bits::pm_tray) == bits::pm_tray);

		// Room
		set_room_beep((value & bits::room_beep) == bits::room_beep);
		set_room_alert((value & bits::room_alert) == bits::room_alert);
		set_room_notify((value & bits::room_notify) == bits::room_notify);
		set_room_tray((value & bits::room_tray) == bits::room_tray);
	}

	// Get
	size_t client_component::get_mode() const
	{
		// NOTE: This ONLY returns bits that are serializable
		size_t value = bits::none;

		if (is_motd())
			value |= bits::motd;

		if (is_poll())
			value |= bits::poll;

		if (is_tunnel())
			value |= bits::tunnel;

		// Chat
		if (is_chat_alert())
			value |= bits::chat_alert;

		if (is_chat_beep())
			value |= bits::chat_beep;

		if (is_chat_notify())
			value |= bits::chat_notify;

		if (is_chat_tray())
			value |= bits::chat_tray;

		// PM
		if (is_pm_alert())
			value |= bits::pm_alert;

		if (is_pm_beep())
			value |= bits::pm_beep;

		if (is_pm_notify())
			value |= bits::pm_notify;

		if (is_pm_tray())
			value |= bits::pm_tray;

		// Room
		if (is_room_alert())
			value |= bits::room_alert;

		if (is_room_beep())
			value |= bits::room_beep;

		if (is_room_notify())
			value |= bits::room_notify;

		if (is_room_tray())
			value |= bits::room_tray;

		return value;
	}
}
