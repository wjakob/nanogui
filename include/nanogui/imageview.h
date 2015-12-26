/*
    nanogui/imageview.h -- Simple widget which shows an image that was
    previously uploaded to the graphics card

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT ImageView : public Widget {
public:
    enum class SizePolicy {
       Fixed,
       Expand
    };

    ImageView(Widget *parent, int image = 0, SizePolicy policy = SizePolicy::Fixed);

    void setImage(int img)      { mImage = img; }
    int  image() const          { return mImage; }

    void       setPolicy(SizePolicy policy) { mPolicy = policy; }
    SizePolicy policy() const { return mPolicy; }

    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual void draw(NVGcontext* ctx);
protected:
    int mImage;
    SizePolicy mPolicy;
};

NAMESPACE_END(nanogui)
