package com.eijah.enigma;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.media.RingtoneManager;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

public class CustomNotification
{
	private static Activity activity;
	private String sessionID;
    public static native void notificationCallback();

	public CustomNotification(Activity mainActivity, String id)
	{
		activity = mainActivity;
		sessionID = id;
	}

	public void notify(String s)
	{
        Intent notificationIntent = new Intent(activity.getApplicationContext(), BlankActivity.class);
        int requestID = (int) System.currentTimeMillis();
        notificationIntent.putExtra("id", sessionID);
        notificationIntent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
        notificationIntent.setAction(Intent.ACTION_MAIN);
        PendingIntent contentIntent = PendingIntent.getActivity(activity.getApplicationContext(), 0, notificationIntent,  PendingIntent.FLAG_UPDATE_CURRENT);

        NotificationManager m_notificationManager = (NotificationManager)activity.getSystemService(Context.NOTIFICATION_SERVICE);
        Notification.Builder m_builder = new Notification.Builder(activity);
        m_builder.setSmallIcon(R.drawable.logo);
        m_builder.setContentIntent(contentIntent);
        m_builder.setContentTitle("Enigma");

        Uri alarmSound = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
        m_builder.setContentText(s).setAutoCancel(true);
		m_builder.setSound(alarmSound);
		m_builder.setLights(0xff0971b2, 500, 2000);

        m_notificationManager.notify(1, m_builder.build());
	}
}
