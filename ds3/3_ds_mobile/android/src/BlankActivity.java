package com.eijah.enigma;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import java.lang.reflect.Method;

public class BlankActivity extends Activity
{
    // This constructor only used when a notification is clicked
    public BlankActivity()
    {
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
	{
        super.onCreate(savedInstanceState);
		Intent intent = new Intent(BlankActivity.this, org.qtproject.qt5.android.bindings.QtActivity.class);
		intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);

		// Make sure app is alive
		Method m = null;
		try
		{
			m = CustomNotification.class.getMethod("notificationCallback");
			if(m != null)
				m.invoke(null);
		}
		catch (Exception e)
		{

		}
        this.startActivity(intent);
        this.finish();
        return;
    }
}
