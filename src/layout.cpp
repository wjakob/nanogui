#include <nanogui/layout.h>
#include <nanogui/widget.h>
#include <nanogui/window.h>
#include <nanogui/theme.h>
#include <nanogui/label.h>
#include <numeric>

NANOGUI_NAMESPACE_BEGIN

Vector2i BoxLayout::preferredSize(NVGcontext *ctx, const Widget *widget) const {
    Vector2i size = Vector2i::Constant(2*mMargin);

    if (dynamic_cast<const Window *>(widget))
        size[1] += widget->theme()->mWindowHeaderHeight - mMargin/2;

    bool first = true;
    int axis1 = (int) mOrientation, axis2 = ((int) mOrientation + 1)%2;
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
    return size;
}

void BoxLayout::performLayout(NVGcontext *ctx, Widget *widget) const {
    Vector2i ps = preferredSize(ctx, widget);
    Vector2i fs = widget->fixedSize();
    Vector2i containerSize(
        fs[0] ? fs[0] : ps[0],
        fs[1] ? fs[1] : ps[1]
    );

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
            case Alignment::Minimum:
                pos[axis2] = mMargin;
                break;
            case Alignment::Middle:
                pos[axis2] = (containerSize[axis2] - cs[axis2]) / 2;
                break;
            case Alignment::Maximum:
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

        bool indentCur = indent && label == nullptr;
        Vector2i ps = Vector2i(availableWidth - (indentCur ? mGroupIndent : 0),
                               c->preferredSize(ctx).y());
        Vector2i fs = c->fixedSize();

        Vector2i cs(
            fs[0] ? fs[0] : ps[0],
            fs[1] ? fs[1] : ps[1]
        );

        c->setPosition(Vector2i(mMargin + (indentCur ? mGroupIndent : 0), height));
        c->setSize(cs);
        c->performLayout(ctx);

        height += cs.y();

        if (label)
            indent = !label->caption().empty();
    }
}

Vector2i GridLayout::preferredSize(NVGcontext *ctx,
                                   const Widget *widget) const {
    int axis1 = (int) mOrientation, axis2 = (axis1 + 1) % 2;

    /* Compute minimum row / column sizes */
    std::vector<int> grid[2];
    computeGridLayout(ctx, widget, grid);

    Vector2i size(
        2*mMargin[0] + std::accumulate(grid[0].begin(), grid[0].end(), 0)
         + std::max((int) grid[0].size() - 1, 0) * mSpacing[0],
        2*mMargin[1] + std::accumulate(grid[1].begin(), grid[1].end(), 0)
         + std::max((int) grid[1].size() - 1, 0) * mSpacing[1]
    );

    if (dynamic_cast<const Window *>(widget))
        size[1] += widget->theme()->mWindowHeaderHeight - mMargin[1]/2;

    return size;
}

void GridLayout::computeGridLayout(NVGcontext *ctx, const Widget *widget, std::vector<int> *grid) const {
    Vector2i size = Vector2i::Zero();
    int axis1 = (int) mOrientation, axis2 = (axis1 + 1) % 2;
    size_t numChildren = widget->children().size();

    Vector2i dim;
    dim[axis1] = mResolution;
    dim[axis2] = (numChildren + mResolution - 1) / mResolution;

    grid[axis1].clear(); grid[axis1].resize(dim[axis1], 0);
    grid[axis2].clear(); grid[axis2].resize(dim[axis2], 0);

    size_t child = 0;
    for (int i2 = 0; i2 < dim[axis2]; i2++) {
        for (int i1 = 0; i1 < dim[axis1]; i1++) {
            if (child >= numChildren)
                return;
            Widget *w = widget->children()[child++];

            Vector2i ps = w->preferredSize(ctx);
            Vector2i fs = w->fixedSize();
            Vector2i cs(
                fs[0] ? fs[0] : ps[0],
                fs[1] ? fs[1] : ps[1]
            );

            grid[axis1][i1] = std::max(grid[axis1][i1], cs[axis1]);
            grid[axis2][i2] = std::max(grid[axis2][i2], cs[axis2]);
        }
    }
}

void GridLayout::performLayout(NVGcontext *ctx, Widget *widget) const {
    Vector2i fs = widget->fixedSize();
    Vector2i containerSize(
        fs[0] ? fs[0] : widget->size()[0],
        fs[1] ? fs[1] : widget->size()[1]
    );

    /* Compute minimum row / column sizes */
    std::vector<int> grid[2];
    computeGridLayout(ctx, widget, grid);
    int dim[2] = { (int) grid[0].size(), (int) grid[1].size() };

    Vector2i extra = Vector2i::Zero();
    if (dynamic_cast<Window *>(widget))
        extra[1] += widget->theme()->mWindowHeaderHeight - mMargin[1] / 2;

    /* Strech to size provided by \c widget */
    for (int i = 0; i < 2; i++) {
        int gridSize = 2 * mMargin[i] + extra[i];
        for (int s : grid[i]) {
            gridSize += s;
            if (i+1 < dim[i])
                gridSize += mSpacing[i];
        }

        if (gridSize < containerSize[i]) {
            /* Re-distribute remaining space evenly */
            int gap = containerSize[i] - gridSize;
            int g = gap / dim[i];
            int rest = gap - g * dim[i];
            for (int j = 0; j < dim[i]; ++j)
                grid[i][j] += g;
            for (int j = 0; rest > 0 && j < dim[i]; --rest, ++j)
                grid[i][j] += 1;
        }
    }

    int axis1 = (int) mOrientation, axis2 = (axis1 + 1) % 2;
    Vector2i start = mMargin + extra;

    int numChildren = widget->children().size();
    size_t child = 0;

    Vector2i pos = start;
    for (int i2 = 0; i2 < dim[axis2]; i2++) {
        pos[axis1] = start[axis1];
        for (int i1 = 0; i1 < dim[axis1]; i1++) {
            if (child >= numChildren)
                return;
            Widget *w = widget->children()[child++];

            Vector2i ps = w->preferredSize(ctx);
            Vector2i fs = w->fixedSize();
            Vector2i cs(
                fs[0] ? fs[0] : ps[0],
                fs[1] ? fs[1] : ps[1]
            );

            Vector2i itemPos(pos);
            for (int j = 0; j < 2; j++) {
                int axis = (axis1 + j) % 2;
                int item = j == 0 ? i1 : i2;
                Alignment align = alignment(axis, item);

                switch (align) {
                    case Alignment::Minimum:
                        break;
                    case Alignment::Middle:
                        itemPos[axis] += (grid[axis][item] - cs[axis]) / 2;
                        break;
                    case Alignment::Maximum:
                        itemPos[axis] += grid[axis][item] - cs[axis];
                        break;
                    case Alignment::Fill:
                        cs[axis] = fs[axis] ? fs[axis] : grid[axis][item];
                        break;
                }
            }
            w->setPosition(itemPos);
            w->setSize(cs);
            w->performLayout(ctx);
            pos[axis1] += grid[axis1][i1] + mSpacing[axis1];
        }
        pos[axis2] += grid[axis2][i2] + mSpacing[axis2];
    }
}



NANOGUI_NAMESPACE_END
