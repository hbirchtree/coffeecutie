package me.birchtrees;

import android.app.NativeActivity;
import android.content.ComponentName;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;

public final class CoffeeNativeActivity extends NativeActivity {

    /* Reference:
     * https://stackoverflow.com/questions/41059909/hiding-the-navigation-bar-in-pure-android-native
     */

    private String getLibraryName() {
        try {
            ActivityInfo info = getPackageManager().getActivityInfo(
                    new ComponentName(this, this.getClass()),
                    PackageManager.GET_META_DATA);
            return info.metaData.getString("android.app.lib_name");
        }catch (PackageManager.NameNotFoundException e)
        {
            System.err.println(e.getMessage());
        }catch (NullPointerException e)
        {
            System.err.println("Failed to get library name");
        }
        return null;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        try {
            super.onCreate(savedInstanceState);
        }catch(Exception e)
        {
            System.err.println("Fatal error in loading activity, retrying with verbose output");
            System.loadLibrary(getLibraryName());
            return;
        }
        //Hide toolbar
        int SDK_INT = android.os.Build.VERSION.SDK_INT;
        if(SDK_INT >= 19)
        {
            setImmersiveSticky();

            View decorView = getWindow().getDecorView();
            decorView.setOnSystemUiVisibilityChangeListener
                    (new View.OnSystemUiVisibilityChangeListener() {
                        @Override
                        public void onSystemUiVisibilityChange(int visibility) {
                            setImmersiveSticky();
                        }
                    });
        }

        System.loadLibrary(getLibraryName());

        smuggleVariable(10, getCacheDir().getAbsolutePath());
        smuggleVariable(11,
                    Integer.toString(
                        getResources()
                            .getDisplayMetrics()
                            .densityDpi)
                    );

        {
            String[] sysAbis = android.os.Build.SUPPORTED_ABIS;
            String supportedAbis = "";

            for(int i=0; i<sysAbis.length; i++)
                supportedAbis = supportedAbis + " " + sysAbis[i];

            supportedAbis = supportedAbis.trim();

            smuggleVariable(12, supportedAbis);
        }


    }

    protected void onResume() {
        super.onResume();

        //Hide toolbar
        int SDK_INT = android.os.Build.VERSION.SDK_INT;
        if(SDK_INT >= 11 && SDK_INT < 14)
        {
            getWindow().getDecorView().setSystemUiVisibility(View.STATUS_BAR_HIDDEN);
        }
        else if(SDK_INT >= 14 && SDK_INT < 19)
        {
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_LOW_PROFILE);
        }
        else if(SDK_INT >= 19)
        {
            setImmersiveSticky();
        }

    }
    // Our popup window, you will call it from your C/C++ code later

    void setImmersiveSticky() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
    }

    private native void smuggleVariable(int id, String data);
}
