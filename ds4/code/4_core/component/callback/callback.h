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

#ifndef _EJA_CALLBACK_H_
#define _EJA_CALLBACK_H_

#include <stddef.h>

namespace eja
{
	namespace callback
	{
		static const size_t bits		= 0x10;
		static const size_t none		= 0x00;

		// Type
		static const size_t browse		= 0x01;
		static const size_t chat		= 0x02;
		static const size_t client		= 0x03;
		static const size_t download	= 0x04;
		static const size_t error		= 0x05;
		static const size_t finished	= 0x06;
		static const size_t group		= 0x07;
		static const size_t help		= 0x08;
		static const size_t option		= 0x09;
		static const size_t pm			= 0x0A;
		static const size_t room		= 0x0B;
		static const size_t router		= 0x0C;
		static const size_t search		= 0x0D;
		static const size_t session		= 0x0E;
		static const size_t share		= 0x0F;
		static const size_t status		= 0x10;
		static const size_t statusbar	= 0x11;
		static const size_t transfer	= 0x12;
		static const size_t upload		= 0x13;
		static const size_t window		= 0x14;

		// Action
		static const size_t create		= 0x01 << bits;
		static const size_t destroy		= 0x02 << bits;

		static const size_t init		= 0x03 << bits;
		static const size_t shutdown	= 0x04 << bits;
		static const size_t update		= 0x05 << bits;

		static const size_t add			= 0x06 << bits;
		static const size_t remove		= 0x07 << bits;
		static const size_t clear		= 0x08 << bits;

		static const size_t start		= 0x09 << bits;
		static const size_t stop		= 0x0A << bits;
		static const size_t restart		= 0x0B << bits;
	}
}

#endif
