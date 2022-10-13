package com.example.fmodtestapp;

import android.app.Activity;
import android.os.Build;
import android.app.Activity;
import android.util.TypedValue;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Button;
import android.content.pm.PackageManager;


import android.os.Bundle;
import android.widget.TextView;

import com.example.fmodtestapp.databinding.ActivityMainBinding;

import java.util.concurrent.RunnableFuture;

public class MainActivity extends Activity implements Runnable
{

    private TextView mTxtScreen;
    private Thread mThread;

    // Used to load the 'fmodtestapp' library on application startup.
    static {
        System.loadLibrary("fmodtestapp");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // Create the text area
        mTxtScreen = new TextView(this);
        mTxtScreen.setTextSize(TypedValue.COMPLEX_UNIT_SP, 10.0f);

        //Create the buttons

        Button[] buttons = new Button[9];

        for(int i = 0; i< buttons.length; i++)
        {
            buttons[i] = new Button(this);
            buttons[i].setText(getButtonLabel(i));
        }


        //Create the button row layouts
        LinearLayout llTopRowButtons = new LinearLayout(this);
        llTopRowButtons.setOrientation(LinearLayout.HORIZONTAL);
        LinearLayout llMiddleRowButtons = new LinearLayout(this);
        llMiddleRowButtons.setOrientation(LinearLayout.HORIZONTAL);
        LinearLayout llBottomRowButtons = new LinearLayout(this);
        llBottomRowButtons.setOrientation(LinearLayout.HORIZONTAL);

        


        org.fmod.FMOD.init(this);
        mThread = new Thread(this, "Example Main");
        mThread.start();


    }

    @Override
    public void run()
    {
        main();
    }

    public void updateScreen(final String text)
    {
        runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                mTxtScreen.setText(text);

            }
        });
    }

    private native String getButtonLabel(int index);
    private native void buttonDown(int index);
    private native void buttonUp(int index);
    private native void setStateCreate();
    private native void setStateStart();
    private native void setStateStop();
    private native void main();








    static
    {
        for (String lib: BuildConfig.FMOD_LIBS)
        {
            System.loadLibrary(lib);
        }
        System.loadLibrary("fmodtestapp");

    }

}

