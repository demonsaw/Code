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

#include "controller/application_controller.h"
#include "notification.h"

namespace eja
{
	notification::notification(QObject* parent) : QObject(parent)
	{
		connect(this, SIGNAL(notificationChanged()), this, SLOT(updateAndroidNotification()));
	}

	void notification::set_notification(const QString &notification)
	{
		m_notification = notification;

		emit notificationChanged();
	}

	QString notification::get_notification() const
	{
		return m_notification;
	}

#if defined(Q_OS_ANDROID)
	void notification::notification_clicked(QAndroidJniEnvironment *env, QAndroidJniObject thiz)
	{
		Q_UNUSED(env)
		Q_UNUSED(thiz)
		const auto app = application_controller::get();
		emit app->notification_clicked();
	}
#endif

	void notification::updateAndroidNotification()
	{
#if defined(Q_OS_ANDROID)
			// Setup
			QAndroidJniEnvironment env;
			QAndroidJniObject javaNotification = QAndroidJniObject::fromString(m_notification);
			QAndroidJniObject sessionID = QAndroidJniObject::fromString("1"); // TODO: replace 1 w id
			notificationInstance = QAndroidJniObject("com/eijah/enigma/CustomNotification", "(Landroid/app/Activity;Ljava/lang/String;)V", QtAndroid::androidActivity().object<jobject>(), sessionID.object<jstring>());

			// Callback
			JNINativeMethod methods[]{ { "notificationCallback", "()V", reinterpret_cast<void *>(notification_clicked) } };
			jclass objectClass = env->GetObjectClass(notificationInstance.object<jobject>());
			env->RegisterNatives(objectClass, methods, sizeof(methods) / sizeof(methods[0]));

			// Call method with string arg
			notificationInstance.callMethod<void>("notify", "(Ljava/lang/String;)V", javaNotification.object<jstring>());
#endif
	}
}