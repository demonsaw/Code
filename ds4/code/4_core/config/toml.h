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

#ifndef _EJA_TOML_H_
#define _EJA_TOML_H_

namespace eja
{
	namespace toml
	{
		// Global
		const char* global = "global";
		//
		const char* active_tab = "active_tab";
		const char* auto_open = "auto_open";
		const char* user_colors = "user_colors";
		const char* download_speed = "download_speed";
		const char* font_size = "font_size";
		const char* max_tabs = "max_tabs";
		const char* qt_geometry = "qt_geometry";
		const char* qt_state = "qt_state";
		const char* statusbar = "statusbar";
		const char* system_tray = "system_tray";
		const char* theme = "theme";
		const char* timestamps = "timestamps";
		const char* upload_speed = "upload_speed";

		// Client/Router
		const char* client = "client";
		const char* router = "router";
		//
		const char* address = "address";
		const char* color = "color";
		const char* enabled = "enabled";
		const char* mode = "mode";
		const char* name = "name";
		const char* port = "port";
		const char* threads = "threads";

		// Group/Session/Verified
		const char* group = "group";
		const char* session = "session";
		const char* verified = "verified";
		//
		const char* algorithm = "algorithm";
		const char* cipher = "cipher";
		const char* entropy = "entropy";
		const char* hash = "hash";
		const char* iterations = "iterations";
		const char* key = "key";
		const char* key_size = "key_size";
		const char* password = "password";
		const char* percent = "percent";
		const char* prime_size = "prime_size";
		const char* salt = "salt";

		// Option
		const char* option = "option";
		//
		const char* blacklist = "blacklist";
		const char* buffer_size = "buffer_size";
		const char* chunk_size = "chunk_size";
		const char* datestamp = "datestamp";
		const char* max_downloads = "max_downloads";
		const char* max_uploads = "max_uploads";
		const char* motd = "motd";
		const char* path = "path";
		const char* redirect = "redirect";
		const char* room = "room";
		const char* share = "share";
		const char* timestamp = "timestamp";
		const char* whitelist = "whitelist";
	}
}

#endif
