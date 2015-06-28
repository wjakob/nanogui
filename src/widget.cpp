#include <nanogui/widget.h>
#include <nanogui/layout.h>
#include <nanogui/theme.h>
#include <nanogui/window.h>
#include <nanogui/opengl.h>
#include <nanogui/screen.h>

NANOGUI_NAMESPACE_BEGIN

Widget::Widget(Widget *parent) 
    : mParent(nullptr), mLayout(nullptr), mPos(Vector2i::Zero()),
      mSize(Vector2i::Zero()), mFixedSize(Vector2i::Zero()), mVisible(true),
      mEnabled(true),mFocused(false),mMouseFocus(false),mTooltip(""),mFontSize(-1.0f),mTheme(nullptr),mCursor(Cursor::None){
    if (parent) {
        parent->addChild(this);
        mTheme = parent->mTheme;
    }
}

Widget::~Widget() {
    for (auto child : mChildren)
        delete child;
    if (mLayout)
        delete mLayout;
}

int Widget::fontSize() const {
  return mFontSize < 0 ? mTheme->mStandardFontSize : mFontSize;
}

Vector2i Widget::preferredSize(NVGcontext *ctx) const {
    if (mLayout)
        return mLayout->preferredSize(ctx, this);
    else
        return mSize;
}

void Widget::performLayout(NVGcontext *ctx) {
    if (mLayout) {
        mLayout->performLayout(ctx, this);
    } else {
        for (auto c : mChildren) {
            Vector2i pref = c->preferredSize(ctx), fix = c->fixedSize();
            c->setSize(Vector2i(
                fix[0] ? fix[0] : pref[0],
                fix[1] ? fix[1] : pref[1]
            ));
            c->performLayout(ctx);
        }
    }
}

Widget *Widget::findWidget(const Vector2i &p) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (child->visible() && child->contains(p - mPos))
            return child->findWidget(p - mPos);
    }
    return contains(p) ? this : nullptr;
}

bool Widget::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (child->visible() && child->contains(p - mPos) &&
            child->mouseButtonEvent(p - mPos, button, down, modifiers))
            return true;
    }
    if (button == GLFW_MOUSE_BUTTON_1 && down && !mFocused)
        requestFocus();

    return false;
}

bool Widget::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (!child->visible())
            continue;
        bool contained = child->contains(p - mPos), prevContained = child->contains(p - mPos - rel);
        if(contained != prevContained) {
            child->mouseEnterEvent(p,contained);
        }
        if ((contained || prevContained) && child->mouseMotionEvent(p - mPos, rel, button, modifiers))
            return true;
    }
    return false;
}

bool Widget::scrollEvent(const Vector2i &p, const Vector2f &rel) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (!child->visible())
            continue;
        if (child->contains(p - mPos) && child->scrollEvent(p - mPos, rel))
            return true;
    }
    return false;
}
bool Widget::mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) {
    return false;
}

bool Widget::mouseEnterEvent(const Vector2i &p, bool enter) {
    mMouseFocus = enter;

    Screen* sc = dynamic_cast<Screen*>(this);
    if(sc == nullptr)
      sc = dynamic_cast<Screen*>(this->window()->parent());

    sc->updateMouseFocus(this);
    return false;
}

bool Widget::focusEvent(bool focused) {
    mFocused = focused;
    return false;
}

bool Widget::keyboardEvent(int key, int scancode, int action, int modifiers) {
    return false;
}

bool Widget::keyboardEvent(unsigned int codepoint) {
    return false;
}

void Widget::addChild(Widget *widget) {
    mChildren.push_back(widget);
    widget->setParent(this);
}

Window *Widget::window() {
    Widget *widget = this;
    while (true) {
        if (!widget)
            throw std::runtime_error(
                "Widget:internal error (could not find parent window)");
        Window *window = dynamic_cast<Window *>(widget);
        if (window)
            return window;
        widget = widget->parent();
    }
}

inline void Widget::requestFocus() {
    Widget *widget = this;
    while (widget->parent())
        widget = widget->parent();
    ((Screen *) widget)->updateFocus(this);
}

void Widget::draw(NVGcontext *ctx) {
    #if NANOGUI_SHOW_WIDGET_BOUNDS
        nvgStrokeWidth(ctx, 1.0f);
        nvgBeginPath(ctx);
        nvgRect(ctx, mPos.x() - 0.5f, mPos.y() - 0.5f, mSize.x() + 1, mSize.y() + 1);
        nvgStrokeColor(ctx, nvgRGBA(255, 0, 0, 255));
        nvgStroke(ctx);
    #endif

    if (mChildren.empty())
        return;

    nvgTranslate(ctx, mPos.x(), mPos.y());
    for (auto child : mChildren)
        if (child->visible())
            child->draw(ctx);
    nvgTranslate(ctx, -mPos.x(), -mPos.y());
}

NANOGUI_NAMESPACE_END
