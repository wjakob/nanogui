#include <nanogui/layout.h>
#include <nanogui/widget.h>
#include <nanogui/window.h>
#include <nanogui/theme.h>
#include <nanogui/label.h>

NANOGUI_NAMESPACE_BEGIN

Vector2i BoxLayout::preferredSize(NVGcontext *ctx, const Widget *widget) const {
    Vector2i size = Vector2i::Zero();
    int axis1 = (int) mOrientation, axis2 = ((int) mOrientation + 1)%2;

    size[axis1] = mMargin;
    size[axis2] = 2*mMargin;

    if (dynamic_cast<const Window *>(widget))
        size[axis1] += widget->theme()->mWindowHeaderHeight - mMargin/2;

    bool first = true;
    for (auto c : widget->children()) {
        if (first)
            first = false;
        else
            size[axis1] += mSpacing;

        Vector2i ps = c->preferredSize(ctx), fs = c->fixedSize();
        Vector2i cs(
            fs[0] ? fs[0] : ps[0],
            fs[1] ? fs[1] : ps[1]
        );

        size[axis1] += cs[axis1];
        size[axis2] = std::max(size[axis2], cs[axis2] + 2*mMargin);
        first = false;
    }
    size[axis1] += mMargin;
    return size;
}

void BoxLayout::performLayout(NVGcontext *ctx, Widget *widget) const {
    Vector2i containerSize = preferredSize(ctx, widget);
    int axis1 = (int) mOrientation, axis2 = ((int) mOrientation + 1)%2;
    int position = mMargin;

    if (dynamic_cast<Window *>(widget))
        position += widget->theme()->mWindowHeaderHeight - mMargin/2;

    bool first = true;
    for (auto c : widget->children()) {
        if (first)
            first = false;
        else
            position += mSpacing;

        Vector2i ps = c->preferredSize(ctx), fs = c->fixedSize();
        Vector2i cs(
            fs[0] ? fs[0] : ps[0],
            fs[1] ? fs[1] : ps[1]
        );
        Vector2i pos = Vector2i::Zero();
        pos[axis1] = position;

        switch (mAlignment) {
            case Minimum:
                pos[axis2] = mMargin;
                break;
            case Middle:
                pos[axis2] = (containerSize[axis2] - cs[axis2]) / 2;
                break;
            case Maximum:
                pos[axis2] = containerSize[axis2] - cs[axis2] - mMargin;
                break;
        }

        c->setPosition(pos);
        c->setSize(cs);
        c->performLayout(ctx);
        position += cs[axis1];
    }
}

Vector2i GroupLayout::preferredSize(NVGcontext *ctx, const Widget *widget) const {
    int height = mMargin, width = 2*mMargin;

    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty())
        height += widget->theme()->mWindowHeaderHeight - mMargin/2;

    bool first = true, indent = false;
    for (auto c : widget->children()) {
        const Label *label = dynamic_cast<const Label *>(c);
        if (!first)
            height += (label == nullptr) ? mSpacing : mGroupSpacing;
        first = false;

        Vector2i ps = c->preferredSize(ctx), fs = c->fixedSize();
        Vector2i cs(
            fs[0] ? fs[0] : ps[0],
            fs[1] ? fs[1] : ps[1]
        );

        bool indentCur = indent && label == nullptr;
        height += cs.y();
        width = std::max(width, cs.x() + 2*mMargin + (indentCur ? mGroupIndent : 0));

        if (label)
            indent = !label->caption().empty();
    }
    height += mMargin;
    return Vector2i(width, height);
}

void GroupLayout::performLayout(NVGcontext *ctx, Widget *widget) const {
    int height = mMargin, availableWidth = widget->width() - 2*mMargin;

    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty())
        height += widget->theme()->mWindowHeaderHeight - mMargin/2;

    bool first = true, indent = false;
    for (auto c : widget->children()) {
        const Label *label = dynamic_cast<const Label *>(c);
        if (!first)
            height += (label == nullptr) ? mSpacing : mGroupSpacing;
        first = false;

        Vector2i ps = c->preferredSize(ctx), fs = c->fixedSize();
        Vector2i cs(
            fs[0] ? fs[0] : ps[0],
            fs[1] ? fs[1] : ps[1]
        );

        bool indentCur = indent && label == nullptr;
        c->setPosition(Vector2i(mMargin + (indentCur ? mGroupIndent : 0), height));
        c->setSize(Vector2i(availableWidth - (indentCur ? mGroupIndent : 0), cs.y()));
        c->performLayout(ctx);

        height += cs.y();

        if (label)
            indent = !label->caption().empty();
    }
}

NANOGUI_NAMESPACE_END
