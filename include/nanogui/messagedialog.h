/*
    nanogui/messagedialog.h -- Simple "OK" or "Yes/No"-style modal dialogs

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/window.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT MessageDialog : public Window {
public:
    enum class Type {
        Information,
        Question,
        Warning
    };

    MessageDialog(Widget *parent, Type type, const std::string &title = "Untitled",
                  const std::string &message = "Message",
                  const std::string &buttonText = "OK",
                  const std::string &altButtonText = "Cancel", bool altButton = false);

    Label *messageLabel() { return mMessageLabel; }
    const Label *messageLabel() const { return mMessageLabel; }

    std::function<void(int)> callback() const { return mCallback; }
    void setCallback(const std::function<void(int)> &callback) { mCallback = callback; }
protected:
    std::function<void(int)> mCallback;
    Label *mMessageLabel;
};

NAMESPACE_END(nanogui)
