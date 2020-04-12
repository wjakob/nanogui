/*
    src/messagedialog.cpp -- Simple "OK" or "Yes/No"-style modal dialogs

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/messagedialog.h>
#include <nanogui/layout.h>
#include <nanogui/button.h>
#include <nanogui/label.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(MessageDialog, Window)
RTTI_IMPLEMENT_INFO(InAppNotification, MessageDialog)

MessageDialog::MessageDialog(Widget* parent, Orientation orient)
  : Window(parent, "")
{
  withLayout<BoxLayout>(orient, Alignment::Middle, 10, 10);
  setModal(true);

  widget(WidgetId{ "#panel_header" },
         WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 10, 15 },
         Element<Label> {
           WidgetId{ "#icon" }, Caption{ utf8(mTheme->mMessageInformationIcon).data() }, CaptionFont{ "icons" }, FontSize{ 50 }
         },
         Element<Label>{ WidgetId{ "#message" }, FixedWidth{ 200 }});

  widget(WidgetId{ "#panel_buttons" },
         WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 15},
         Element<Button>{ 
           WidgetId{ "#alt_button"} , Caption{ "Ok" }, Icon{ mTheme->mMessageAltButtonIcon },
           ButtonCallback{ [&] { if (mCallback) mCallback(1); dispose(); }}, IsVisible{ false }
         },
         Element<Button>{ 
           WidgetId{ "#button" }, Caption{ "Cancel" }, Icon{ mTheme->mMessagePrimaryButtonIcon },
           ButtonCallback{ [&] { if (mCallback) mCallback(0); dispose(); }}
         });
 
}

Label *MessageDialog::messageLabel() { return findWidget<Label>("#message"); }
const Label *MessageDialog::messageLabel() const { return const_cast<MessageDialog*>(this)->findWidget<Label>("#message"); }

void MessageDialog::setMessage(const std::string& message)
{
  if (auto label = findWidget<Label>("#message"))
    label->setCaption(message);
}

void MessageDialog::setDialogType(int type)
{
  int icon = 0;
  switch ((Type)type) 
  {
    case Type::Information: icon = mTheme->mMessageInformationIcon; break;
    case Type::Question: icon = mTheme->mMessageQuestionIcon; break;
    case Type::Warning: icon = mTheme->mMessageWarningIcon; break;
  }
  setIcon(icon);
}

void MessageDialog::setDialogButton(const std::string& text)
{
  if (auto btn = findWidget<Button>("#button"))
    btn->setCaption(text);
}

void MessageDialog::setDialogAltButton(const std::string& text)
{
  if (auto btn = findWidget<Button>("#alt_button"))
  {
    btn->setCaption(text);
    btn->show();
  }
}

void MessageDialog::draw(NVGcontext* ctx)
{
  if (!inited)
  {
    center();
    requestFocus();
    inited = true;
  }
  Window::draw(ctx);
}

void MessageDialog::setIcon(int c)
{
  if (auto icon = findWidget<Label>("#icon"))
    icon->setCaption(utf8(c).data());
}

InAppNotification::InAppNotification(Widget *parent)
  : MessageDialog(parent, Orientation::Horizontal)
{
  withLayout<BoxLayout>(Orientation::Horizontal, Alignment::Middle, 10, 10);
  setModal(true);
}

NAMESPACE_END(nanogui)
