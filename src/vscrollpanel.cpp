#include <nanogui/vscrollpanel.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>

NANOGUI_NAMESPACE_BEGIN

VScrollPanel::VScrollPanel(Widget *parent)
    : Widget(parent), mChildPreferredHeight(0), mScroll(0.0f) {}

void VScrollPanel::performLayout(NVGcontext *ctx) {
    if (mChildren.empty())
        return;
    Widget *child = mChildren[0];
    mChildPreferredHeight = child->preferredSize(ctx).y();
    child->setPosition(Vector2i(0, 0));
    child->setSize(Vector2i(mSize.x()-12, mChildPreferredHeight));
}

Vector2i VScrollPanel::preferredSize(NVGcontext *ctx) const {
    if (mChildren.empty())
        return Vector2i::Zero();
    return mChildren[0]->preferredSize(ctx) + Vector2i(12, 0);
}

bool VScrollPanel::mouseDragEvent(const Vector2i &p, const Vector2i &rel,
                            int button, int modifiers) {
    if (mChildren.empty())
        return false;

    float scrollh = height() *
        std::min(1.0f, height() / (float)mChildPreferredHeight);

    mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
                 mScroll + rel.y() / (float)(mSize.y() - 8 - scrollh)));
    return true;
}

bool VScrollPanel::scrollEvent(const Vector2i &p, const Vector2f &rel) {
    float scrollAmount = rel.y() * (mSize.y() / 20.0f);
    float scrollh = height() *
        std::min(1.0f, height() / (float)mChildPreferredHeight);

    mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
            mScroll - scrollAmount / (float)(mSize.y() - 8 - scrollh)));
    return true;
}

bool VScrollPanel::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    if (mChildren.empty())
        return false;
    int shift = (int) (mScroll*(mChildPreferredHeight - mSize.y()));
    return mChildren[0]->mouseButtonEvent(p + Vector2i(0, shift), button, down, modifiers);
}

bool VScrollPanel::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) {
    if (mChildren.empty())
        return false;
    int shift = (int) (mScroll*(mChildPreferredHeight - mSize.y()));
    return mChildren[0]->mouseMotionEvent(p + Vector2i(0, shift), rel, button, modifiers);
}

void VScrollPanel::draw(NVGcontext *ctx) {
    if (mChildren.empty())
        return;
    Widget *child = mChildren[0];
    mChildPreferredHeight = child->preferredSize(ctx).y();
    float scrollh = height() *
        std::min(1.0f, height() / (float) mChildPreferredHeight);

    nvgSave(ctx);
    nvgTranslate(ctx, mPos.x(), mPos.y());
    nvgScissor(ctx, 0, 0, mSize.x(), mSize.y());
    nvgTranslate(ctx, 0, -mScroll*(mChildPreferredHeight - mSize.y()));
    if (child->visible())
        child->draw(ctx);
    nvgRestore(ctx);

    NVGpaint paint = nvgBoxGradient(
        ctx, mPos.x() + mSize.x() - 12 + 1, mPos.y() + 4 + 1, 8,
        mSize.y() - 8, 3, 4, Color(0, 32), Color(0, 92));
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + mSize.x() - 12, mPos.y() + 4, 8,
                   mSize.y() - 8, 3);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);

    paint = nvgBoxGradient(
        ctx, mPos.x() + mSize.x() - 12 - 1,
        mPos.y() + 4 + (mSize.y() - 8 - scrollh) * mScroll - 1, 8, scrollh,
        3, 4, Color(220, 100), Color(128, 100));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + mSize.x() - 12 + 1,
                   mPos.x() + 4 + 1 + (mSize.y() - 8 - scrollh) * mScroll, 8 - 2,
                   scrollh - 2, 2);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);
}

NANOGUI_NAMESPACE_END
