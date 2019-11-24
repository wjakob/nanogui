#include <nanogui/ledmatrix.h>
#include <nanovg.h>

NAMESPACE_BEGIN(nanogui)

bool LedMatrix::isValid(int row, int col) const
{
    return ( (row >= 0) &&
             (row < mRowCount) &&
             (col >= 0) &&
             (col < mColumnCount) );
}

void LedMatrix::clearColumn(int col)
{
  if (isValid(0, col))
  {
    for (auto& c: colorTable[col])
      c = NoColor;
  }
}

void LedMatrix::setColorAt(int row, int col, const Color& rgb)
{
    if (isValid(row, col))
    {
      colorTable[col][row] = rgb.toInt();
    }
}

void LedMatrix::performLayout(NVGcontext* ctx)
{
  Widget::performLayout(ctx);

  float side = height() / mRowCount;
  int columns = width() / side;
  setColumnCount(columns);
}

void LedMatrix::setText(const std::string& text)
{

}

void LedMatrix::drawLEDs(NVGcontext* ctx)
{
  float wside = width() / mColumnCount;
  float hside = height() / mRowCount;

  float side = std::min(wside, hside);
  float x = mPos.x() + side/2, y = mPos.y() + side/2;

  for (int row=0; row < mRowCount; ++row)
  {
      for (int col=0; col < mColumnCount; ++col)
      {
          nvgBeginPath(ctx);
          nvgFillColor(ctx, colorAt(row, col));
          nvgEllipse(ctx, x, y, side/2, side/2);
          nvgFill(ctx);
          x += side;
      }
      y += side;
      x = mPos.x() + side/2;
  }
}

LedMatrix::LedMatrix(Widget* parent)
 : Widget(parent)
{
    mBackgroundColor = Color(0x000000ff);
    mDarkLedColor = Color(NoColor);
    mColumnCount = 0;
    mRowCount = 0;
}

LedMatrix::~LedMatrix() {}
void LedMatrix::clear()
{
    for (int i=0; i < mRowCount; ++i)
    {
        for (int j=0; j < mColumnCount; ++j)
        {
            colorTable[j][i] = mDarkLedColor.toInt();
        }
    }
}

Color LedMatrix::backgroundColor() const
{
    return mBackgroundColor;
}

void LedMatrix::setBackgroundColor(const Color& color)
{
  mBackgroundColor = color;
}

void LedMatrix::setDarkLedColor(const Color& color)
{
    auto oldColor = mDarkLedColor;
    mDarkLedColor = color;

    for (int i=0; i < mRowCount; ++i)
    {
        for (int j=0; j < mColumnCount; ++j)
        {
            if (colorTable[j][i] == oldColor.toInt())
            {
                colorTable[j][i] = mDarkLedColor.toInt();
            }
        }
    }
}

Color LedMatrix::colorAt(int row, int col) const
{
    if (isValid(row, col))
    {
      return colorTable[col][row];
    }

    return mDarkLedColor;
}

int LedMatrix::rowCount() const { return mRowCount; }
void LedMatrix::setRowCount(int rows)
{
  mNeedRecalcParams = true;
    if ((rows >= 0) && (rows != mRowCount))
    {
        int previousRowCount = mRowCount;
        mRowCount = rows;

        for (int i=0; i < mColumnCount; ++i)
        {
            colorTable[i].resize(rows);
            if (rows > previousRowCount)
            {
                for (int j=previousRowCount; j < rows; ++j)
                {
                    setColorAt(j, i, mDarkLedColor);
                }
            }
        }
    }
}

int LedMatrix::columnCount() const { return mColumnCount; }

void LedMatrix::setColumnCount(int columns)
{
    if ((columns >= 0) && (columns != mColumnCount))
    {
        int previousColumnsCount = mColumnCount;
        mColumnCount = columns;

        colorTable.resize(columns);
        if (columns > previousColumnsCount)
        {
            for (int i=previousColumnsCount; i<columns; ++i)
            {
                colorTable[i].resize(mRowCount);
                for (int j=0; j < mRowCount; ++j)
                {
                    setColorAt(j, i, mDarkLedColor);
                }
            }
        }
    }
}

Vector2i LedMatrix::preferredSize(NVGcontext* ctx) const
{
    return Widget::preferredSize(ctx);
}



void LedMatrix::draw(NVGcontext *ctx)
{
  const float w = width();
  const float h = height();

  nvgBeginPath(ctx);
  nvgRect(ctx, mPos.x(), mPos.y(), w, h);
  nvgFillColor(ctx, mBackgroundColor);
  nvgFill(ctx);

  Widget::draw(ctx);

  nvgSave(ctx);

  drawLEDs(ctx);

  nvgRestore(ctx);
}

NAMESPACE_END(nanogui)