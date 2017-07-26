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

public class Minimize extends Activity
{
	private static Activity activity;

	public Minimize(Activity mainActivity)
	{
		activity = mainActivity;
	}

	public void minimize()
	{
		Log.d("Debug: ", "minimize()");
		Intent startMain = new Intent(Intent.ACTION_MAIN);
		startMain.addCategory(Intent.CATEGORY_HOME);
		startMain.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		activity.startActivity(startMain);
		Log.d("Debug: ", "minimize ending");
	}
}
