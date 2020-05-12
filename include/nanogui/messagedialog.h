/*
    nanogui/messagedialog.h -- Simple "OK" or "Yes/No"-style modal dialogs

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/window.h>

NAMESPACE_BEGIN(nanogui)

DECLSETTER(DialogType, int)
DECLSETTER(DialogResult, std::function<void(int)>)
DECLSETTER(DialogTitle, std::string)
DECLSETTER(DialogButton, std::string)
DECLSETTER(DialogAltButton, std::string)
DECLSETTER(DialogMessage, std::string)

/**
 * \class MessageDialog messagedialog.h nanogui/messagedialog.h
 *
 * \brief Simple "OK" or "Yes/No"-style modal dialogs.
 */
class NANOGUI_EXPORT MessageDialog : public Window {
public:
    RTTI_CLASS_UID(MessageDialog)
    RTTI_DECLARE_INFO(MessageDialog)

    /// Classification of the type of message this MessageDialog represents.
    enum class Type {
        Information,
        Question,
        Warning
    };

    explicit MessageDialog(Widget* parent, Orientation orient);

    using Window::set;
    template<typename... Args>
    MessageDialog(Widget* parent, const Args&... args) 
      : MessageDialog(parent, Orientation::Vertical)
    { set<MessageDialog, Args...>(args...); }

    Label *messageLabel();
    const Label *messageLabel() const;

    void setMessage(const std::string& message);

    std::function<void(int)> callback() const { return mCallback; }
    void setCallback(const std::function<void(int)> &callback) { mCallback = callback; }

    void setDialogType(int type);
    void setIcon(int icon);

    void setDialogButton(const std::string& text);
    void setDialogAltButton(const std::string& text);

    void draw(NVGcontext* ctx);
protected:
    std::function<void(int)> mCallback;
    bool inited = false;
public:
    PROPSETTER(DialogType, setDialogType)
    PROPSETTER(Icon, setIcon)
    PROPSETTER(DialogTitle, setTitle)
    PROPSETTER(DialogResult, setCallback)
    PROPSETTER(DialogMessage, setMessage)
    PROPSETTER(DialogButton, setDialogButton)
    PROPSETTER(DialogAltButton, setDialogAltButton)
};

namespace elm { using MessageDialog = Element<MessageDialog>; }

class NANOGUI_EXPORT InAppNotification : public MessageDialog
{
public:
  RTTI_CLASS_UID(InAppNotification)
  RTTI_DECLARE_INFO(InAppNotification)

  explicit InAppNotification(Widget *parent);

  using MessageDialog::set;
  template<typename... Args>
  InAppNotification(Widget* parent, const Args&... args)
    : InAppNotification(parent)
  { set<InAppNotification, Args...>(args...); }

  void draw(NVGcontext* ctx);
protected:
  enum { Expand, Collapse, Idle };

  int mState = Idle;
};

NAMESPACE_END(nanogui)
