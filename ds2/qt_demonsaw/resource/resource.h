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

#ifndef _EJA_RESOURCE_
#define _EJA_RESOURCE_

namespace eja
{
	namespace resource
	{
		// Main
		static const char* icon = ":/demonsaw/icon.ico";
		static const char* splash = ":/demonsaw/splash.png";
		static const char* logo = ":/demonsaw/logo.png";
		static const char* no_image = ":/demonsaw/no_image.png";
		static const char* style = ":/demonsaw/style.css";
		static const char* logo_black = ":/32/demonsaw.png";

		namespace audio
		{
			static const char* chat = "qrc://resource/audio/chat.wav";
			static const char* message = "qrc://resource/audio/chat.wav";
		}

		namespace font
		{
			static const char* droid = ":/font/droid.ttf";
			static const char* dejavu = ":/font/dejavu.ttf";
			static const char* noto = ":/font/noto.ttf";			
			/*static const char* liberation = ":/font/liberation.ttf";
			static const char* vera = ":/font/vera.ttf";
			static const char* junicode = ":/font/junicode.ttf";
			static const char* athena = ":/font/athena.ttf";
			static const char* source = ":/font/source.ttf";*/
		}

		namespace tab
		{
			static const char* client = ":/32/client.png";
			static const char* router = ":/32/router.png";
			static const char* server = ":/32/server.png";
			static const char* group = ":/32/group.png";
			static const char* session = ":/32/session.png";
			static const char* search = ":/32/search.png";
			static const char* browse = ":/32/browse.png";
			static const char* share = ":/32/share.png";
			static const char* transfer = ":/32/transfer.png";
			static const char* download = ":/32/download.png";
			static const char* queued = ":/32/queued.png";
			static const char* upload = ":/32/upload.png";		
			static const char* chat = ":/32/chat.png"; 
			static const char* chat_unread = ":/32/chat_unread.png";
			static const char* message = ":/32/message.png";
			static const char* message_unread = ":/32/message_unread.png";
			static const char* phone = ":/32/phone.png"; 
			static const char* option = ":/32/option.png";
			static const char* none = ":/32/none.png";
			static const char* sad = ":/32/sad.png";
			static const char* error = ":/32/error.png"; 
			static const char* log = ":/32/log.png";
			static const char* debug = ":/32/debug.png";
			static const char* check = ":/32/check.png";
			static const char* finish = ":/32/finish.png";
			static const char* folder = ":/32/folder.png";
			static const char* file = ":/32/file.png";
			static const char* stats = ":/32/stats.png";
			static const char* lightning = ":/32/lightning.png";
		}

		namespace tree
		{
			static const char* plus = ":/tree/plus.png";
			static const char* minus = ":/tree/minus.png"; 
			static const char* vline = ":/tree/vline.png";
			static const char* more = ":/tree/more.png";
			static const char* end = ":/tree/end.png";
			static const char* closed = ":/tree/closed.png";
			static const char* open = ":/tree/open.png";

			static const char* up = ":/tree/up.png";
			static const char* down = ":/tree/down.png";
			static const char* left = ":/tree/left.png";
			static const char* right = ":/tree/right.png";
		}

		namespace tool
		{
			static const char* option = ":/32/option.png";
			static const char* network = ":/32/network.png";
			static const char* router = ":/32/router.png";
			static const char* security = ":/32/security.png";
			static const char* timeout = ":/32/timeout.png";
			static const char* protocol = ":/32/protocol.png";
		}

		namespace menu
		{
			static const char* arrow = ":/32/arrow.png";
			
			static const char* client = ":/32/client.png";
			static const char* router = ":/32/router.png";
			static const char* refresh = ":/32/refresh.png";
			static const char* restart = ":/32/restart.png"; 
			static const char* reset = ":/32/reset.png";

			static const char* add = ":/32/add.png";
			static const char* remove = ":/32/remove.png";
			static const char* remove_all = ":/32/remove_all.png";
			static const char* edit = ":/32/edit.png";
			static const char* clear = ":/32/exit.png";

			static const char* search = ":/32/search.png";
			static const char* open = ":/32/open.png";
			static const char* download = ":/32/download.png";
			static const char* sound = ":/32/sound.png";
			static const char* mute = ":/32/mute.png";
			static const char* unmute = ":/32/sound.png";

			static const char* start = ":/32/start.png";
			static const char* stop = ":/32/stop.png";
			static const char* pause = ":/32/pause.png";
			static const char* resume = ":/32/resume.png";
			static const char* queue = ":/32/queued.png";
			static const char* finish = ":/32/finish.png";

			static const char* exit = ":/32/exit.png";			
			static const char* save = ":/32/save.png"; 
			static const char* help = ":/32/demonsaw.png";
			static const char* about = ":/32/about.png";
		}

		namespace status
		{
			static const char* none = ":/status/none.png";
			static const char* info = ":/status/info.png";
			static const char* success = ":/status/success.png";
			static const char* warning = ":/status/warning.png";
			static const char* error = ":/status/error.png";
			static const char* pending = ":/status/pending.png";
			static const char* cancelled = ":/status/cancel.png";
			static const char* unknown = ":/status/cancel.png";
			static const char* question = ":/32/question.png";
		}

		namespace client
		{
			static const char* none = ":/client/none.png";
			static const char* info = ":/client/info.png";
			static const char* success = ":/client/success.png";
			static const char* warning = ":/client/warning.png";
			static const char* pending = ":/client/pending.png";
			static const char* error = ":/client/error.png";
		}

		namespace router
		{
			static const char* none = ":/router/none.png";
			static const char* pending = ":/router/pending.png";
			static const char* info = ":/router/info.png";
			static const char* success = ":/router/success.png";
			static const char* warning = ":/router/warning.png";
			static const char* error = ":/router/error.png";			
		}
	}
}

#endif
