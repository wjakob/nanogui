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
    Vector2i ps = preferredSize(ctx,widget);
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
        Vector2i ps = Vector2i(availableWidth - (indentCur ? mGroupIndent : 0),c->preferredSize(ctx).y());
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

Vector2i GridLayout::preferredSize(NVGcontext *ctx,const Widget *widget) const {
  int axis1 = (int)mOrientation;
  int axis2 = ((int)mOrientation + 1)%2;

  std::vector<std::vector<int>> grid;
  computeGridLayout(ctx,widget,grid);

  Vector2i size = Vector2i::Zero();
  size[0] = 2*mMargin;
  size[1] = 2*mMargin;

  for(int cs : grid[1])
    size[0] += cs + 2*mSpacing;

  for(int rs : grid[0])
    size[1] += rs + 2*mSpacing;

  return size;
}

void GridLayout::performLayout(NVGcontext *ctx,Widget *widget) const {
  int axis1 = (int)mOrientation;
  int axis2 = ((int)mOrientation + 1)%2;

  std::vector<std::vector<Alignment>> alignment;
  alignment.push_back(mRowAlignment);
  alignment.push_back(mColAlignment);
  std::vector<Alignment> defaultAlignment;
  defaultAlignment.push_back(mDefaultRowAlignment);
  defaultAlignment.push_back(mDefaultColAlignment);

  std::vector<std::vector<int>> grid;
  computeGridLayout(ctx,widget,grid);

  // strech to size given by parent widget layout
  for(int i=0;i<2;i++) {
    int ax1 = (axis1+i) % 2;
    int ax2 = (axis2+i) % 2;
    float maxWidth = widget->fixedSize()(ax1)!= 0 ? widget->fixedSize()(ax1) : widget->size()(ax1);
    float gridWidth = 2*mMargin;

    for(int cs : grid[ax2])
      gridWidth += cs + 2*mSpacing;

    if(gridWidth < maxWidth) {
      int gap = maxWidth - gridWidth;
      int g = gap / grid[ax2].size();
      for(int c=0;c<grid[ax2].size();c++)
        grid[ax2][c] += g;
      int rest = gap -g*grid[ax2].size();
      for(int c=0;rest>0;rest--,c++)
        grid[ax2][c] += 1;
    }
  }

  Vector2i pos(mMargin,mMargin);

  if(dynamic_cast<Window *>(widget))
    pos(1) += widget->theme()->mWindowHeaderHeight - mMargin/2;

  pos(axis2) += mSpacing;

  int numChildren = widget->children().size();
  Vector2i cell(0,0);
  for(int child = 0;cell[0]<grid[axis1].size() && child < numChildren;cell[0]++)
  {
    pos(axis1) = mMargin + mSpacing;
    for(cell[1]=0;cell[1]<grid[axis2].size() && child < numChildren;cell[1]++)
    {
      auto c = widget->children()[child++];

      Vector2i ps = c->preferredSize(ctx);
      Vector2i fs = c->fixedSize();
      Vector2i cs(
        fs[0] ? fs[0] : ps[0],
        fs[1] ? fs[1] : ps[1]
        );

      Vector2i position = pos;

      for(int i=0;i<2;i++) {
        int ax1 = (axis1+i) % 2;
        int ax2 = (axis2+i) % 2;
        Alignment align = alignment[ax1].size() > cell[i] ? alignment[ax1][cell[i]] : defaultAlignment[ax1];
        switch(align) {
          case Alignment::Minimum:
          break;
          case Alignment::Middle:
          position[ax2] += (grid[ax1][cell[i]] - cs[ax2]) / 2;
          break;
          case Alignment::Maximum:
          position[ax2] += grid[ax1][cell[i]] - cs[ax2];
          break;
          case Alignment::Fill:
          cs[ax2] = fs[ax2] ? fs[ax2] : grid[ax1][cell[i]];
          break;
        }
      }

      c->setPosition(position);
      c->setSize(cs);
      c->performLayout(ctx);
      pos(axis1) += grid[axis2][cell[1]] + 2*mSpacing;
    }
    pos(axis2) += 2*mSpacing + grid[axis1][cell[0]];
  }
}

void GridLayout::computeGridLayout(NVGcontext *ctx,const Widget *widget,std::vector<std::vector<int>>& grid) const
{
  Vector2i size = Vector2i::Zero();
  int axis1 = (int)mOrientation;
  int axis2 = ((int)mOrientation + 1)%2;

  int numChildren = widget->children().size();

  Vector2i dim;
  dim[axis1] = (numChildren % mSize) ? numChildren / mSize + 1: numChildren / mSize;
  dim[axis2] = mSize;

  // find max row and colum sizes
  grid.push_back(std::vector<int>());
  grid.push_back(std::vector<int>());
  grid[axis1].resize(dim[axis1],0);
  grid[axis2].resize(dim[axis2],0);

  for(int row=0,child=0;row<dim[axis1] && child < numChildren;row++)
  {
    for(int col=0;col<dim[axis2] && child < numChildren;col++)
    {
      auto c = widget->children()[child++];

      Vector2i ps = c->preferredSize(ctx);
      Vector2i fs = c->fixedSize();
      Vector2i cs(
        fs[0] ? fs[0] : ps[0],
        fs[1] ? fs[1] : ps[1]
        );

      if(grid[axis2][col] < cs[axis1])
        grid[axis2][col] = cs[axis1];
      if(grid[axis1][row] < cs[axis2])
        grid[axis1][row] = cs[axis2];
    }
  }
}

NANOGUI_NAMESPACE_END
