/*
 * \class Color color.h nanogui/color.h
 *
 * \brief Stores an RGBA floating point color value.
 *
 * This class simply wraps around an ``4 float``, providing some convenient
 * methods and terminology for thinking of it as a color.  The data operates in the
 * same way as 4 float, and the following values are identical:
 *
 * \rst
 * +---------+-------------+-------------+
 * | Channel | Array Index | Color Value |
 * +=========+=============+=============+
 * | Red     | ``0``       | r()         |
 * +---------+-------------+-------------+
 * | Green   | ``1``       | g()         |
 * +---------+-------------+-------------+
 * | Blue    | ``2``       | b()         |
 * +---------+-------------+-------------+
 * | Alpha   | ``3``       | w()         |
 * +---------+-------------+-------------+
 *
 */
#pragma once

#include <nanogui/common.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT Color
{
public:
  Color() : Color(0, 0, 0, 0) {}

  Color(float intensity, float alpha)
  {
    r() = intensity; g() = intensity;
    b() = intensity; a() = alpha;
  }

  Color(int intensity, int alpha)
  {
    r() = (intensity / 255.f); g() = (intensity / 255.f);
    b() = (intensity / 255.f); a() = alpha / 255.f;
  }

  Color(float _r, float _g, float _b, float _a)
  {
    r() = _r; g() = _g; b() = _b; a() = _a;
  }

  Color(int _r, int _g, int _b, int _a)
  {
    r() = _r / 255.f; g() = _g / 255.f; b() = _b / 255.f; a() = _a / 255.f;
  }

  Color(int v)
  {
    r() = ((v >> 24) & 0xff)/255.f;
    g() = ((v >> 16) & 0xff) / 255.f;
    b() = ((v >> 8) & 0xff) / 255.f;
    a() = (v & 0xff) / 255.f;
  }

  Color(const Color& rgb, float _a) { *this = rgb; a() = _a; }

  Color& operator/=(float v) {
    for (auto& a : rgba) a /= v;
    return *this;
  }
  /// Return a reference to the red channel
  float &r() { return rgba[0]; }
  /// Return a reference to the red channel (const version)
  const float &r() const { return rgba[0]; }
  /// Return a reference to the green channel
  float &g() { return rgba[1]; }
  /// Return a reference to the green channel (const version)
  const float &g() const { return rgba[1]; }
  /// Return a reference to the blue channel
  float &b() { return rgba[2]; }
  /// Return a reference to the blue channel (const version)
  const float &b() const { return rgba[2]; }

  float &a() { return rgba[3]; }
  const float &a() const { return rgba[3]; }

  float &w() { return rgba[3]; }
  const float &w() const { return rgba[3]; }

  const Color& notW(const Color& c) const { return w() == 0 ? c : *this; }

  Color rgb() const { return Color(rgba[0], rgba[1], rgba[2], 0.f); }

  void setAlpha(float a) { rgba[3] = a; }

  Color withAlpha(float a) const { Color c = *this; c.rgba[3] = a; return c; }

  bool operator!=(const Color& c)
  {
    return !(c.a() == a() && c.r() == r() && c.g() == g() && c.b() == b());
  }

  Color contrastingColor() const {
    float luminance = r() * 0.299f + g() * 0.587f + b() * 0.144f;
    return Color(luminance < 0.5f ? 1.f : 0.f, 1.f);
  }

  Color operator*(float m) const
  {
    return Color(r()*m, g()*m, b()*m, a()*m);
  }

  Color operator+(const Color& c) const
  {
    return Color(r() + c.r(), g() + c.g(), b() + c.b(), a() + c.a());
  }

  inline operator const NVGcolor &() const { return reinterpret_cast<const NVGcolor &>(rgba); }
  inline Color mul_a(float mul) { Color ret = *this; ret.w() *= mul; return ret; }
  inline int toInt() const { return ((int)(r() * 255) << 24) + ((int)(g() * 255) << 16) + ((int)(b() * 255) << 8) + (int)(w() * 255); }

  inline const float* data() const { return rgba; }
  inline float* data() { return rgba; }

  Color transpose() const
  {
    Color out;
    for (int i = 0; i < 2; i++)
      for (int j = 0; j < 2; j++)
        out.rgba[j*2+i] = rgba[i*2+j];
    return out;
  }

  static const Color& find( const std::string& name );
#define __DEFINE_COLOR(a) static const Color a;
__DEFINE_COLOR( transparent )
__DEFINE_COLOR( red )
__DEFINE_COLOR( caesarRed )
__DEFINE_COLOR( blue )
__DEFINE_COLOR( green )
__DEFINE_COLOR( show )
__DEFINE_COLOR( ghost)
__DEFINE_COLOR( whitesmoke)
__DEFINE_COLOR( floralwhite)
__DEFINE_COLOR( oldlace)
__DEFINE_COLOR( linen)
__DEFINE_COLOR( antiqueWhite)
__DEFINE_COLOR( papayaWhip)
__DEFINE_COLOR( blanchedAlmond)
__DEFINE_COLOR( bisque)
__DEFINE_COLOR( peachPuff)
__DEFINE_COLOR( navajoWhite)
__DEFINE_COLOR( moccasin)
__DEFINE_COLOR( cornsilk)
__DEFINE_COLOR( ivory)
__DEFINE_COLOR( lemonChiffon)
__DEFINE_COLOR( seashell)
__DEFINE_COLOR( honeydew)
__DEFINE_COLOR( mintCream)
__DEFINE_COLOR( azure)
__DEFINE_COLOR( aliceBlue)
__DEFINE_COLOR( lavender)
__DEFINE_COLOR( lavenderBlush)
__DEFINE_COLOR( mistyRose)
__DEFINE_COLOR( white)
__DEFINE_COLOR( black)
__DEFINE_COLOR( darkSlateGray)
__DEFINE_COLOR( dimGrey)
__DEFINE_COLOR( slateGrey)
__DEFINE_COLOR( lightSlateGray)
__DEFINE_COLOR( grey)
__DEFINE_COLOR( lightGray)
__DEFINE_COLOR( midnightBlue)
__DEFINE_COLOR( navyBlue)
__DEFINE_COLOR( cornflowerBlue)
__DEFINE_COLOR( darkSlateBlue)
__DEFINE_COLOR( slateBlue)
__DEFINE_COLOR( mediumSlateBlue)
__DEFINE_COLOR( lightSlateBlue)
__DEFINE_COLOR( mediumBlue)
__DEFINE_COLOR( royalBlue)
__DEFINE_COLOR( skyBlue)
__DEFINE_COLOR( paleTurquoise)
__DEFINE_COLOR( cyan)
__DEFINE_COLOR( mediumAquamarine)
__DEFINE_COLOR( darkGreen)
__DEFINE_COLOR( darkSeaGreen)
__DEFINE_COLOR( paleGreen)
__DEFINE_COLOR( limeGreen)
__DEFINE_COLOR( darkKhaki)
__DEFINE_COLOR( paleGoldenrod)
__DEFINE_COLOR( yellow)
__DEFINE_COLOR( gold)
__DEFINE_COLOR( rosyBrown)
__DEFINE_COLOR( indianRed)
__DEFINE_COLOR( sienna)
__DEFINE_COLOR( peru)
__DEFINE_COLOR( wheat)
__DEFINE_COLOR( sandyBrown)
__DEFINE_COLOR( tan)
__DEFINE_COLOR( chocolate)
__DEFINE_COLOR( firebrick)
__DEFINE_COLOR( brown)
__DEFINE_COLOR( salmon)
__DEFINE_COLOR( orange)
__DEFINE_COLOR( darkOrange)
__DEFINE_COLOR( coral)
__DEFINE_COLOR( tomato)
__DEFINE_COLOR( orangeRed)
__DEFINE_COLOR( hotPink)
__DEFINE_COLOR( deepPink)
__DEFINE_COLOR( pink)
__DEFINE_COLOR( maroon)
__DEFINE_COLOR( magenta)
__DEFINE_COLOR( violet)
__DEFINE_COLOR( orchid)
__DEFINE_COLOR( purple)
__DEFINE_COLOR( snow)
__DEFINE_COLOR( dodgerBlue)
__DEFINE_COLOR( steelBlue)
__DEFINE_COLOR( slateGray)
__DEFINE_COLOR( aquamarine)
__DEFINE_COLOR( seaGreen)
__DEFINE_COLOR( springGreen)
__DEFINE_COLOR( chartreuse)
__DEFINE_COLOR( plum)
__DEFINE_COLOR( thistle)
__DEFINE_COLOR( darkGrey)
__DEFINE_COLOR( heavyDarkGrey)

private:
  float rgba[4];
};

NAMESPACE_END(nanogui)

inline std::ostream& operator<<(std::ostream& os, const nanogui::Color& c)
{
  os << c.r() << '.' << c.g() << '.' << c.b() << '.' << c.a();
  return os;
}