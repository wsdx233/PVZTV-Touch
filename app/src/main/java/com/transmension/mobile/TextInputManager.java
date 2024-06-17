package com.transmension.mobile;

import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;

/* loaded from: classes.dex */
public interface TextInputManager {
    int KEYBOARD_EMAIL = 3;
    int KEYBOARD_NORMAL = 0;
    int KEYBOARD_PASSWORD = 1;
    int KEYBOARD_URL = 2;
    int KEYBOARD_USERNAME = 4;

    /* loaded from: classes.dex */
    abstract class Listener {
        public abstract void onEditorAction(int i);

        public abstract void onTextChanged(View view, String str, int i, int i2, long j);

        public abstract void onTextInput(View view, String str);
    }

    int getImeOptions();

    long getInputCookie();

    int getInputType();

    Listener getListener();

    int getSelectionEnd();

    int getSelectionStart();

    CharSequence getText();

    void hideIme(int i);

    void hideTextInputDialog();

    void hideTextInputDialog(boolean z);

    InputConnection onCreateInputConnection(EditorInfo editorInfo);

    void setImeOptions(int i);

    void setInputCookie(long j);

    void setInputType(int i);

    void setListener(Listener listener);

    void setSelection(int i, int i2);

    void setText(String str);

    void setText(String str, int i, int i2);

    void showIme(int i);

    void showTextInputDialog(int i, String str, String str2, String str3);
}