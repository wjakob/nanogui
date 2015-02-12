#include <nanogui/popup.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>

NANOGUI_NAMESPACE_BEGIN
    
Popup::Popup(Widget *parent, Window *parentWindow)
    : Window(parent, ""), mParentWindow(parentWindow),
      mAnchorPos(Vector2i::Zero()), mAnchorHeight(30) {
}

void Popup::performLayout(NVGcontext *ctx) {
    if (mLayout || mChildren.size() != 1) {
        Widget::performLayout(ctx);
    } else {
        mChildren[0]->setPosition(Vector2i::Zero());
        mChildren[0]->setSize(mSize);
        mChildren[0]->performLayout(ctx);
    }
}

void Popup::refreshRelativePlacement() {
    mParentWindow->refreshRelativePlacement();
    mVisible &= mParentWindow->visibleRecursive();
    mPos = mParentWindow->position() + mAnchorPos - Vector2i(0, mAnchorHeight);
}

void Popup::draw(NVGcontext* ctx) {
    refreshRelativePlacement();

    if (!mVisible)
        return;

    int ds = mTheme->mWindowDropShadowSize, cr = mTheme->mWindowCornerRadius;

    /* Draw a drop shadow */
    NVGpaint shadowPaint = nvgBoxGradient(
        ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr*2, ds*2,
        mTheme->mDropShadow, mTheme->mTransparent);

    nvgBeginPath(ctx);
    nvgRect(ctx, mPos.x()-ds,mPos.y()-ds, mSize.x()+2*ds, mSize.y()+2*ds);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);
    nvgPathWinding(ctx, NVG_HOLE);
    nvgFillPaint(ctx, shadowPaint);
    nvgFill(ctx);

    /* Draw window */
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);

    nvgMoveTo(ctx, mPos.x()-15,mPos.y()+mAnchorHeight);
    nvgLineTo(ctx, mPos.x()+1,mPos.y()+mAnchorHeight-15);
    nvgLineTo(ctx, mPos.x()+1,mPos.y()+mAnchorHeight+15);

    nvgFillColor(ctx, mTheme->mWindowPopup);
    nvgFill(ctx);

    Widget::draw(ctx);
}

NANOGUI_NAMESPACE_END
