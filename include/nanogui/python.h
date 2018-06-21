/*
    nanogui/python.h -- Macros to facilitate Python bindings of custom widgets

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/common.h>
#include <pybind11/pybind11.h>

/// Provides a ``PYBIND11_OVERLOAD`` for any relevant Widget items that need to be bound.
#define NANOGUI_WIDGET_OVERLOADS(Parent) \
    bool mouseButtonEvent(const ::nanogui::Vector2i &p, int button, bool down, int modifiers) override { \
        PYBIND11_OVERLOAD(bool, Parent, mouseButtonEvent, p, button, down, modifiers); \
    } \
    bool mouseMotionEvent(const ::nanogui::Vector2i &p, const ::nanogui::Vector2i &rel, int button, int modifiers) override { \
        PYBIND11_OVERLOAD(bool, Parent, mouseMotionEvent, p, rel, button, modifiers); \
    } \
    bool mouseDragEvent(const ::nanogui::Vector2i &p, const ::nanogui::Vector2i &rel, int button, int modifiers) override { \
        PYBIND11_OVERLOAD(bool, Parent, mouseDragEvent, p, rel, button, modifiers); \
    } \
    bool mouseEnterEvent(const ::nanogui::Vector2i &p, bool enter) override { \
        PYBIND11_OVERLOAD(bool, Parent, mouseEnterEvent, p, enter); \
    } \
    bool scrollEvent(const ::nanogui::Vector2i &p, const ::nanogui::Vector2f &rel) override { \
        PYBIND11_OVERLOAD(bool, Parent, scrollEvent, p, rel); \
    } \
    bool focusEvent(bool focused) override { \
        PYBIND11_OVERLOAD(bool, Parent, focusEvent, focused); \
    } \
    bool keyboardEvent(int key, int scancode, int action, int modifiers) override { \
        PYBIND11_OVERLOAD(bool, Parent, keyboardEvent, key, scancode, action, modifiers); \
    } \
    bool keyboardCharacterEvent(unsigned int codepoint) override { \
        PYBIND11_OVERLOAD(bool, Parent, keyboardCharacterEvent, codepoint); \
    } \
    ::nanogui::Vector2i preferredSize(NVGcontext *ctx) const override { \
        PYBIND11_OVERLOAD(::nanogui::Vector2i, Parent, preferredSize, ctx); \
    } \
    void performLayout(NVGcontext *ctx) override { \
        PYBIND11_OVERLOAD(void, Parent, performLayout, ctx); \
    } \
    void draw(NVGcontext *ctx) override { \
        PYBIND11_OVERLOAD(void, Parent, draw, ctx); \
    } \
    void addChild(int index, Widget *widget) override { \
        PYBIND11_OVERLOAD(void, Parent, addChild, index, widget); \
    } \
    void setTheme(Theme *theme) override { \
        PYBIND11_OVERLOAD(void, Parent, setTheme, theme); \
    } \
    std::string defaultFont() const override { \
        PYBIND11_OVERLOAD(std::string, Parent, defaultFont, ); \
    } \
    std::string defaultTooltipFont() const override { \
        PYBIND11_OVERLOAD(std::string, Parent, defaultTooltipFont, ); \
    } \
    std::string defaultIconFont() const override { \
        PYBIND11_OVERLOAD(std::string, Parent, defaultIconFont, ); \
    }

/// Provides a ``PYBIND11_OVERLOAD`` for any relevant Layout items that need to be bound.
#define NANOGUI_LAYOUT_OVERLOADS(Parent) \
    ::nanogui::Vector2i preferredSize(NVGcontext *ctx, const ::nanogui::Widget *widget) const override { \
        PYBIND11_OVERLOAD(::nanogui::Vector2i, Parent, preferredSize, ctx, widget); \
    } \
    void performLayout(NVGcontext *ctx, ::nanogui::Widget *widget) const override { \
        PYBIND11_OVERLOAD(void, Parent, performLayout, ctx, widget); \
    }

/// Provides a ``PYBIND11_OVERLOAD`` for any relevant Screen items that need to be bound.
#define NANOGUI_SCREEN_OVERLOADS(Parent) \
    virtual void drawAll() override { \
        PYBIND11_OVERLOAD(void, Parent, drawAll); \
    } \
    virtual void drawContents() override { \
        PYBIND11_OVERLOAD(void, Parent, drawContents); \
    } \
    virtual bool dropEvent(const std::vector<std::string> &filenames) override { \
        PYBIND11_OVERLOAD(bool, Parent, dropEvent, filenames); \
    } \
    virtual bool resizeEvent(const ::nanogui::Vector2i &size) override { \
        PYBIND11_OVERLOAD(bool, Parent, resizeEvent, size); \
    }
