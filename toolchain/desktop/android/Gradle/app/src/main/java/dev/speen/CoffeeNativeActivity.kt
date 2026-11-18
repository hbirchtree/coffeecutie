package dev.speen

import android.annotation.SuppressLint
import android.app.NativeActivity
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.view.View
import android.view.ViewGroup
import android.view.WindowInsets
import android.view.WindowInsetsController
import android.view.WindowManager
import android.view.inputmethod.InputMethodManager
import java.util.ArrayDeque

@Suppress("DEPRECATION")
class CoffeeNativeActivity : NativeActivity() {
    fun loadIfExists(libname: String) {
        try {
            System.loadLibrary(libname)
        } catch(_: UnsatisfiedLinkError) {}
    }

    fun libraryName(): String? {
        return packageManager
                .getActivityInfo(this.componentName, PackageManager.GET_META_DATA)
                .metaData
                .getString("android.app.lib_name");
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        loadIfExists("openal")
        try {
            super.onCreate(savedInstanceState);
        } catch (e: Exception)
        {
            System.err.println("Loading failed: ${e.message}");
            System.err.println("Reloading with verbose output");
            libraryName()?.let { it: String ->
                System.loadLibrary(it)
            }
        }

        window.decorView.setOnSystemUiVisibilityChangeListener {
            makeFullscreen();
        };

        makeFullscreen();
    }

    private val textEvents = ArrayDeque<String>()
    private var textView: TextInputView? = null

    private fun inputMethods() =
        getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager

    /* Called from native code: soft keyboard and input method on our editor */
    fun startTextInput() = runOnUiThread {
        val view = textView ?: TextInputView(this) { event ->
            synchronized(textEvents) { textEvents.add(event) }
        }.also {
            addContentView(it, ViewGroup.LayoutParams(1, 1))
            textView = it
        }
        view.requestFocus()
        view.post { inputMethods().showSoftInput(view, 0) }
    }

    fun stopTextInput() = runOnUiThread {
        val view = textView ?: return@runOnUiThread
        inputMethods().hideSoftInputFromWindow(view.windowToken, 0)
        view.clearFocus()
        inputMethods().restartInput(view)
        synchronized(textEvents) { textEvents.clear() }
    }

    /* Called from native code until null: "w" + text written, "e" + the
     * composition, as UTF-16 */
    fun pollTextInput(): CharArray? =
        synchronized(textEvents) { textEvents.poll() }?.toCharArray()

    @SuppressLint("ObsoleteSdkInt")
    fun makeFullscreen() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN) {
            window.setFlags(
                    WindowManager.LayoutParams.FLAG_FULLSCREEN,
                    WindowManager.LayoutParams.FLAG_FULLSCREEN);
            window.decorView.systemUiVisibility = View.STATUS_BAR_HIDDEN;
        } else if(Build.VERSION.SDK_INT <= Build.VERSION_CODES.KITKAT) {
            window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_FULLSCREEN or
                    View.SYSTEM_UI_FLAG_LOW_PROFILE;
        } else if(Build.VERSION.SDK_INT < Build.VERSION_CODES.R) {
            window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_FULLSCREEN or
                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION or
                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY or
                    View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN or
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE;
        } else if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            window.insetsController?.systemBarsBehavior =
                    WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
            window.insetsController?.hide(WindowInsets.Type.statusBars());
            window.insetsController?.hide(WindowInsets.Type.navigationBars());
        }
    }
}
