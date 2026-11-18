package dev.speen

import android.content.Context
import android.text.InputType
import android.view.KeyEvent
import android.view.View
import android.view.inputmethod.BaseInputConnection
import android.view.inputmethod.EditorInfo
import android.view.inputmethod.InputConnection

/* An editor for input methods to write into. It keeps no text: what is
 * written and composed goes to `sink`, deletions and actions become key
 * events, which reach native code like any other key */
class TextInputView(context: Context, private val sink: (String) -> Unit) : View(context) {
    init {
        isFocusable = true
        isFocusableInTouchMode = true
    }

    override fun onCheckIsTextEditor() = true

    override fun onCreateInputConnection(info: EditorInfo): InputConnection {
        info.inputType = InputType.TYPE_CLASS_TEXT
        info.imeOptions = EditorInfo.IME_ACTION_SEND or
                EditorInfo.IME_FLAG_NO_FULLSCREEN or
                EditorInfo.IME_FLAG_NO_EXTRACT_UI
        return Connection()
    }

    private inner class Connection : BaseInputConnection(this@TextInputView, false) {
        private var composing = ""

        override fun setComposingText(text: CharSequence, newCursorPosition: Int): Boolean {
            composing = text.toString()
            sink("e$composing")
            return true
        }

        override fun finishComposingText(): Boolean {
            commit(composing)
            return true
        }

        override fun commitText(text: CharSequence, newCursorPosition: Int): Boolean {
            commit(text.toString())
            return true
        }

        override fun deleteSurroundingText(beforeLength: Int, afterLength: Int): Boolean {
            repeat(beforeLength) { key(KeyEvent.KEYCODE_DEL) }
            return true
        }

        override fun performEditorAction(actionCode: Int): Boolean {
            key(KeyEvent.KEYCODE_ENTER)
            return true
        }

        private fun commit(text: String) {
            if (composing.isNotEmpty()) {
                composing = ""
                sink("e")
            }
            if (text.isNotEmpty())
                sink("w$text")
        }

        private fun key(code: Int) {
            sendKeyEvent(KeyEvent(KeyEvent.ACTION_DOWN, code))
            sendKeyEvent(KeyEvent(KeyEvent.ACTION_UP, code))
        }
    }
}
