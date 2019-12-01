#pragma once

/**
 * \class LedMatrix
 *
 * \brief The LedMatrix widget simulates a LED matrix (dot matrix) display.
 *
 * A LED matrix display is a 2-dimensional array of dots used to generate
 * characters, symbols and images. It is often seen on machines, clocks and
 * display boards.
 *
 * This generic implementation can configured for any size of rows (greater
 * than 0) and columns. The number of colors is not limited, therefore it can
 * be used to make a monochrome, bi-color or even a full RGB display.
 *
 * Individual LEDs can be addressed in the matrix by their row and column
 * position. The index (0,0) is located in the upper left corner. Columns
 * grow from left to right, and rows grow from top to bottom.
 *
 * \author Pierre-Etienne Messier  <pierre.etienne@gmail.com>
 * nanogui ported dalerank <dalerankn8@gmail.com>
 */

#include <nanogui/widget.h>
#include <vector>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT LedMatrix : public Widget
{
public:
  RTTI_CLASS_UID("LDMX")
  RTTI_DECLARE_INFO(LedMatrix)

  LedMatrix(Widget* parent);
  virtual ~LedMatrix();

  enum LEDColor
  {
      NoColor   = 0x222222FF,

      Red       = 0xFF0000FF,
      Green     = 0x00FF00FF,
      Blue      = 0x0000FFFF,

      White     = 0xFFFFFFFF,
      Orange    = 0xFF8800FF,
      OrangeRed = 0xFF2200FF,
      Yellow    = 0xFFFF00FF
  };

  void clear();

  Color backgroundColor() const;
  void setBackgroundColor(const Color& color);

  Color colorAt(int row, int col) const;
  void setColorAt(int row, int col, const Color& rgb);
  void clearColumn(int col);

  int rowCount() const;
  void setRowCount(int rows);

  int columnCount() const;

  void setImage(int imageIndex);
  void setText(const std::string& text);

  Vector2i preferredSize(NVGcontext* ctx) const override;
  void performLayout(NVGcontext* ctx) override;

  void draw(NVGcontext* ctx) override;
protected:

  bool isValid(int row, int col) const;
  void drawLEDs(NVGcontext* ctx);
  void setDarkLedColor(const Color& color);
  void setColumnCount(int columns);

  Color mBackgroundColor;
  Color mDarkLedColor;
  std::vector<std::vector<int>> colorTable;
  int mRowCount;
  int mColumnCount;
  bool mNeedRecalcParams = false;
};

NAMESPACE_END(nanogui)
