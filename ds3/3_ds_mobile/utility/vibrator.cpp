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

#include <QDebug>

#include "utility/vibrator.h"

namespace eja
{
	vibrator::vibrator(QObject* parent /*= nullptr*/) : QObject(parent)
	{
#if defined(Q_OS_ANDROID)
		QAndroidJniObject vibroString = QAndroidJniObject::fromString("vibrator");
		QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
		QAndroidJniObject appctx = activity.callObjectMethod("getApplicationContext", "()Landroid/content/Context;");
		vibratorService = appctx.callObjectMethod("getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;", vibroString.object<jstring>());
#endif
	}

#if defined(Q_OS_ANDROID)
	void vibrator::vibrate(const int milliseconds /*= 50*/)
	{
		if (vibratorService.isValid()) 
		{
			jlong ms = milliseconds;
			jboolean hasvibro = vibratorService.callMethod<jboolean>("hasVibrator", "()Z");
			vibratorService.callMethod<void>("vibrate", "(J)V", ms);
		}
		else 
		{
			qDebug() << "No vibrator service available";
		}
	}

#else
	void vibrator::vibrate(const int milliseconds /*= 50*/)
	{
		Q_UNUSED(milliseconds);
	}
#endif
}
