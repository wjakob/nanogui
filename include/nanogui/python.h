/*
    nanogui/python.h -- Macros to facilitate Python bindings of custom widgets

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <pybind11/pybind11.h>

#define NANOGUI_WIDGET_OVERLOADS(Parent) \
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) { \
        PYBIND11_OVERLOAD(bool, Parent, mouseButtonEvent, p, button, down, modifiers); \
    } \
    bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) { \
        PYBIND11_OVERLOAD(bool, Parent, mouseMotionEvent, p, rel, button, modifiers); \
    } \
    bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) { \
        PYBIND11_OVERLOAD(bool, Parent, mouseDragEvent, p, rel, button, modifiers); \
    } \
    bool mouseEnterEvent(const Vector2i &p, bool enter) { \
        PYBIND11_OVERLOAD(bool, Parent, mouseEnterEvent, p, enter); \
    } \
    bool scrollEvent(const Vector2i &p, const Vector2f &rel) { \
        PYBIND11_OVERLOAD(bool, Parent, scrollEvent, p, rel); \
    } \
    bool focusEvent(bool focused) { \
        PYBIND11_OVERLOAD(bool, Parent, focusEvent, focused); \
    } \
    bool keyboardEvent(int key, int scancode, int action, int modifiers) { \
        PYBIND11_OVERLOAD(bool, Parent, keyboardEvent, key, scancode, action, modifiers); \
    } \
    bool keyboardCharacterEvent(unsigned int codepoint) { \
        PYBIND11_OVERLOAD(bool, Parent, keyboardCharacterEvent, codepoint); \
    } \
    Vector2i preferredSize(NVGcontext *ctx) const { \
        PYBIND11_OVERLOAD(Vector2i, Parent, preferredSize, ctx); \
    } \
    void performLayout(NVGcontext *ctx) { \
        PYBIND11_OVERLOAD(void, Parent, performLayout, ctx); \
    } \
    void draw(NVGcontext *ctx) { \
        PYBIND11_OVERLOAD(void, Parent, draw, ctx); \
    }

#define NANOGUI_LAYOUT_OVERLOADS(Parent) \
    Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const { \
        PYBIND11_OVERLOAD(Vector2i, Parent, preferredSize, ctx, widget); \
    } \
    void performLayout(NVGcontext *ctx, Widget *widget) const { \
        PYBIND11_OVERLOAD(void, Parent, performLayout, ctx, widget); \
    }

#define NANOGUI_SCREEN_OVERLOADS(Parent) \
    virtual void drawAll() { \
        PYBIND11_OVERLOAD(void, Parent, drawAll); \
    } \
    virtual void drawContents() { \
        PYBIND11_OVERLOAD(void, Parent, drawContents); \
    } \
    virtual bool dropEvent(const std::vector<std::string> &filenames) { \
        PYBIND11_OVERLOAD(bool, Parent, dropEvent, filenames); \
    } \
    virtual bool resizeEvent(const Vector2i &size) { \
        PYBIND11_OVERLOAD(bool, Parent, resizeEvent, size); \
    }
