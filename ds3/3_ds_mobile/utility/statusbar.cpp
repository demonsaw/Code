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
#include "statusbar.h"

#ifdef Q_OS_ANDROID
#include <QtAndroid>

// WindowManager.LayoutParams
#define FLAG_TRANSLUCENT_STATUS 0x04000000
#define FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS 0x80000000
#endif
namespace eja
{
	StatusBar::StatusBar(QObject *parent) : QObject(parent)
	{
	}

	QColor StatusBar::color() const
	{
		return m_color;
	}

	void StatusBar::setColor(const QColor &color)
	{
		Q_UNUSED(color);
		if (!isAvailable())
			return;

#ifdef Q_OS_ANDROID
		QtAndroid::runOnAndroidThread([=]() {
			QAndroidJniObject window = QtAndroid::androidActivity().callObjectMethod("getWindow", "()Landroid/view/Window;");
			window.callMethod<void>("addFlags", "(I)V", FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
			window.callMethod<void>("clearFlags", "(I)V", FLAG_TRANSLUCENT_STATUS);
			window.callMethod<void>("setStatusBarColor", "(I)V", color.rgba());
		});
#endif
		emit colorChanged();
	}

	bool StatusBar::isAvailable() const
	{
#ifdef Q_OS_ANDROID
		return QtAndroid::androidSdkVersion() >= 21;
#else
		return false;
#endif
	}
}