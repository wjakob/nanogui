/*
    nanogui/vscrollpanel.h -- Adds a vertical scrollbar around a widget
    that is too big to fit into a certain area

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT VScrollPanel : public Widget {
public:
    VScrollPanel(Widget *parent);

    virtual void performLayout(NVGcontext *ctx);
    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel);
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);
    virtual bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);
    virtual void draw(NVGcontext *ctx);
    virtual void save(Serializer &s) const;
    virtual bool load(Serializer &s);
protected:
    int mChildPreferredHeight;
    float mScroll;
};

NAMESPACE_END(nanogui)
