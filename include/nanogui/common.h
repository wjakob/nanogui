/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/common.h
 *
 * \brief Common definitions used by NanoGUI.
 */

#pragma once

#if defined(_WIN32)
#  if defined(NANOGUI_BUILD)
     /* Quench a few warnings on when compiling NanoGUI on Windows */
#    pragma warning(disable : 4127) // warning C4127: conditional expression is constant
#    pragma warning(disable : 4244) // warning C4244: conversion from X to Y, possible loss of data
#  endif
#  pragma warning(disable : 4251) // warning C4251: class X needs to have dll-interface to be used by clients of class Y
#  pragma warning(disable : 4714) // warning C4714: function X marked as __forceinline not inlined
#  pragma warning(disable : 4127) // warning C4127: conditional expression is constant
#endif

#include <stdint.h>
#include <array>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <memory>
#include <string>
#include <functional>
//#include <type_traits>
#include <vector>

/* Set to 1 to draw boxes around widgets */
//#define NANOGUI_SHOW_WIDGET_BOUNDS 1

#if !defined(NAMESPACE_BEGIN) || defined(DOXYGEN_DOCUMENTATION_BUILD)
    /**
     * \brief Convenience macro for namespace declarations
     *
     * The macro ``NAMESPACE_BEGIN(nanogui)`` will expand to ``namespace
     * nanogui {``. This is done to hide the namespace scope from editors and
     * C++ code formatting tools that may otherwise indent the entire file.
     * The corresponding ``NAMESPACE_END`` macro also lists the namespace
     * name for improved readability.
     *
     * \param name
     *     The name of the namespace scope to open
     */
    #define NAMESPACE_BEGIN(name) namespace name {
#endif
#if !defined(NAMESPACE_END) || defined(DOXYGEN_DOCUMENTATION_BUILD)
    /**
     * \brief Convenience macro for namespace declarations
     *
     * Closes a namespace (counterpart to ``NAMESPACE_BEGIN``)
     * ``NAMESPACE_END(nanogui)`` will expand to only ``}``.
     *
     * \param name
     *     The name of the namespace scope to close
     */
    #define NAMESPACE_END(name) }
#endif

#if defined(NANOGUI_SHARED)
#  if defined(_WIN32)
#    if defined(NANOGUI_BUILD)
#      define NANOGUI_EXPORT __declspec(dllexport)
#    else
#      define NANOGUI_EXPORT __declspec(dllimport)
#    endif
#  elif defined(NANOGUI_BUILD)
#    define NANOGUI_EXPORT __attribute__ ((visibility("default")))
#  else
#    define NANOGUI_EXPORT
#  endif
#else
     /**
      * If the build flag ``NANOGUI_SHARED`` is defined, this directive will expand
      * to be the platform specific shared library import / export command depending
      * on the compilation stage.  If undefined, it expands to nothing. **Do not**
      * define this directive on your own.
      */
#    define NANOGUI_EXPORT
#endif

/* Force usage of discrete GPU on laptops (macro must be invoked in main application) */
#if defined(_WIN32) && !defined(DOXYGEN_DOCUMENTATION_BUILD)
#define NANOGUI_FORCE_DISCRETE_GPU() \
    extern "C" { \
        __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; \
        __declspec(dllexport) int NvOptimusEnablement = 1; \
    }
#else
/**
 * On Windows, exports ``AmdPowerXpressRequestHighPerformance`` and
 * ``NvOptimusEnablement`` as ``1``.
 */
#define NANOGUI_FORCE_DISCRETE_GPU()
#endif

// These will produce broken links in the docs build
#if !defined(DOXYGEN_SHOULD_SKIP_THIS)

struct NVGcontext { /* Opaque handle type, never de-referenced within NanoGUI */ };
struct NVGpaint;
struct GLFWwindow { /* Opaque handle type, never de-referenced within NanoGUI */ };

struct NVGcolor;
struct NVGglyphPosition;
struct GLFWcursor;

#endif // DOXYGEN_SHOULD_SKIP_THIS

// Define command key for windows/mac/linux
#if defined(__APPLE__) || defined(DOXYGEN_DOCUMENTATION_BUILD)
    /// If on OSX, maps to ``GLFW_MOD_SUPER``.  Otherwise, maps to ``GLFW_MOD_CONTROL``.
    #define SYSTEM_COMMAND_MOD GLFW_MOD_SUPER
#else
    #define SYSTEM_COMMAND_MOD GLFW_MOD_CONTROL
    #define SYSTEM_COMMAND_SHIFT GFLW_MOD_SHIFT
#endif

NAMESPACE_BEGIN(nanogui)

/// Cursor shapes available to use in GLFW.  Shape of actual cursor determined by Operating System.
enum class Cursor {
    Arrow = 0,  ///< The arrow cursor.
    IBeam,      ///< The I-beam cursor.
    Crosshair,  ///< The crosshair cursor.
    Hand,       ///< The hand cursor.
    HResize,    ///< The horizontal resize cursor.
    VResize,    ///< The vertical resize cursor.
    CursorCount ///< Not a cursor --- should always be last: enables a loop over the cursor types.
};

namespace math
{
    const float ROUNDING_ERROR_f32 = 0.000001f;
    //! 64bit constant for converting from degrees to radians (formally known as GRAD_PI2)
    const float DEGTORADFLT = (float)M_PI / 180.0f;
    const float RADTODEGFLT = 180.0f / (float)M_PI;

    template<class T>
    inline bool isEqual(const T a, const T b, const T tolerance = ROUNDING_ERROR_f32)
    { return (a + tolerance >= b) && (a - tolerance <= b); }

    template<class T>
    inline bool lessOrEqual(const T a, const T b) { return (a < b) || isEqual(a, b); }

    template<class T>
    inline bool greatOrEqual(const T a, const T b) { return (a > b) || isEqual(a, b); }
};

template <class T>
class Vector2
{
public:
  inline T& x() { return _d[0]; }
  inline T& y() { return _d[1]; }

  inline const T& x() const { return _d[0]; }
  inline const T& y() const { return _d[1]; }

  //! Default constructor (null vector)
  Vector2() : Vector2(0, 0) {}
  //! Constructor with two different values
  Vector2(T nx, T ny) { x() = nx; y() = ny; }
  //! Constructor with the same value for both members
  explicit Vector2(T n) : Vector2(n, n) {}
  //! Copy constructor
  Vector2(const Vector2<T>& o) { _d[0] = o._d[0]; _d[1] = o._d[1]; }

  static Vector2 Constant(T v) { return Vector2(v, v); }
  static Vector2 Zero() { return Vector2(0, 0); }

  // operators
  inline Vector2 operator-() const { return Vector2(-x(), -y()); }
  inline Vector2& operator=(const Vector2& o) { x() = o.x(); y() = o.y(); return *this; }

  Vector2 operator+(const Vector2& o) const { return{ x() + o.x(), y() + o.y() }; }

  Vector2& operator+=(const Vector2& o) { x() += o.x(); y() += o.y(); return *this; }
  template<typename Scalar> Vector2 operator+(Scalar v) const { return Vector2(x() + v, y() + v); }
  template<typename Scalar> Vector2& operator+=(Scalar v) { x() += v; y() += v; return *this; }

  Vector2 operator-(const Vector2& o) const { return{ x() - o.x(), y() - o.y() }; }
  Vector2& operator-=(const Vector2& o) { x() -= o.x(); y() -= o.y(); return *this; }
  template<typename Scalar> Vector2 operator-(Scalar v) const { return Vector2<T>(x() - v, y() - v); }
  template<typename Scalar> Vector2& operator-=(Scalar v) { x() -= v; y() -= v; return *this; }

  Vector2 operator*(const Vector2& o) const { return Vector2(x() * o.x(), y() * o.y()); }
  Vector2& operator*=(const Vector2& o) { x() *= o.x(); y() *= o.y(); return *this; }
  
  template<typename Scalar> Vector2 operator*(Scalar v) const { return Vector2(x() * v, y() * v); }
  template<typename Scalar> Vector2& operator*=(Scalar v) { x() *= v; y() *= v; return *this; }

  Vector2 operator/(const Vector2& o) const { return Vector2(x() / o.x(), y() / o.y()); }
  Vector2& operator/=(const Vector2& o) { x() /= o.x(); y() /= o.y(); return *this; }
  template<typename Scalar> Vector2 operator/(Scalar v) const { return Vector2(x() / v, y() / v); }
  template<typename Scalar> Vector2& operator/=(Scalar v) { x() /= v; y() /= v; return *this; }

  //! sort in order X, Y. Equality with rounding tolerance.
  inline bool operator<=(const Vector2& o) const
  { return (math::lessOrEqual(x(), o.x()) && math::lessOrEqual(y(), o.y())); }

  bool positive() const { return x() >= 0 && y() >= 0; }

  inline bool lessOrEq(const Vector2& o) const
  { return (math::lessOrEqual(x(), o.x()) && math::lessOrEqual(y(), o.y())); }

  inline bool lessOrEq(T _x, T _y) const 
  { return (math::lessOrEqual(x(), _x) && math::lessOrEqual(y(), _y)); }

  //! sort in order X, Y. Equality with rounding tolerance.
  inline bool operator>=(const Vector2&o) const
  { return (math::greatOrEqual(x(), o.x()) && math::greatOrEqual(y(), o.y())); }

  //! sort in order X, Y. Difference must be above rounding tolerance.
  bool operator<(const Vector2&o) const
  { return (x() < o.x()) && (y() < o.y()); }

  //! sort in order X, Y. Difference must be above rounding tolerance.
  bool operator>(const Vector2&o) const
  { return (x() > o.x()) && (y() > o.y()); }

  bool operator==(const Vector2& o) const { return isEqual(o, math::ROUNDING_ERROR_f32); }
  bool operator!=(const Vector2& o) const { return !isEqual(o, math::ROUNDING_ERROR_f32); }

  Vector2 cquotient(const Vector2& o) const { return Vector2(x() / (float)o.x(), y() / (float)o.y()); }

  T minCoeff() const { return x() > y() ? y() : x(); }

  // functions

  //! Checks if this vector equals the o one.
  /** Takes floating point rounding errors into account.
  \param o Vector to compare with.
  \return True if the two vector are (almost) equal, else false. */
  bool isEqual(const Vector2& o, float tolerance) const
  { return math::isEqual<T>(x(), o.x(), tolerance) && math::isEqual<T>(y(), o.y(), tolerance); }

  Vector2& set(T nx, T ny) { x() = nx; y() = ny; return *this; }
  Vector2& set(const Vector2& p) { x() = p.x(); y() = p.y(); return *this; }

  //! Gets the length of the vector.
  /** \return The length of the vector. */
  float getLength() const { return std::sqrt(x()*x() + y()*y()); }

  inline Vector2 yy() const { return{ y(), y() }; }
  inline Vector2 _0y()const { return{ 0,   y() }; }
  inline Vector2 _x0()const { return{ x(), 0 }; }

  inline Vector2 xx() const { return{ x(), x() }; }
  inline Vector2 yx() const { return{ y(), x() }; }

  //! Get the squared length of this vector
  /** This is useful because it is much faster than getLength().
  \return The squared length of the vector. */
  T getLengthSQ() const { return x()*x() + y()*y(); }

  //! Get the dot product of this vector with ano.
  /** \param o o vector to take dot product with.
  \return The dot product of the two vectors. */
  T dotProduct(const Vector2& o) const { return x()*o.x() + y()*o.y();  }

  template< class A > Vector2<A> As() { return Vector2<A>((A)x(), (A)y()); }
  template< class A > Vector2<A> As() const { return Vector2<A>((A)x(), (A)y()); }

  //! Gets distance from ano point.
  /** Here, the vector is interpreted as a point in 2-dimensional space.
  \param o o vector to measure from.
  \return Distance from o point. */
  float getDistanceFrom(const Vector2& o) const
  { return Vector2(x() - o.x(), y() - o.y()).getLength();}

  //! Returns squared distance from ano point.
  /** Here, the vector is interpreted as a point in 2-dimensional space.
  \param o o vector to measure from.
  \return Squared distance from o point. */
  T getDistanceFromSQ(const Vector2<T>& o) const
  { return Vector2(x() - o.x(), y() - o.y()).getLengthSQ(); }

  //! rotates the point anticlockwise around a center by an amount of degrees.
  /** \param degrees Amount of degrees to rotate by, anticlockwise.
  \param center Rotation center.
  \return This vector after transformation. */
  Vector2& rotateBy(float degrees, const Vector2& center)
  {
    const float cs = std::cos(degrees);
    const float sn = std::sin(degrees);

    *this -= center;
    set((T)(x()*cs - y()*sn), (T)(x()*sn + y()*cs));

    *this += center;
    return *this;
  }

  //! Normalize the vector.
  /** The null vector is left untouched.
  \return Reference to this vector, after normalization. */
  Vector2& normalize()
  {
    float length = (float)(x()*x() + y()*y());

    if (math::isEqual(length, 0.f))
      return *this;
    length = 1.f / std::sqrt(length);
    x() = (T)(x() * length);
    y() = (T)(y() * length);
    return *this;
  }

  //! Calculates the angle of this vector in degrees in the counter trigonometric sense.
  /** 0 is to the right (3 o'clock), values increase clockwise.
  \return Returns a value between 0 and 360. */
  inline float getAngle() const
  {
    if (y() == 0) // corrected thanks to a suggestion by Jox
            return x() < 0 ? 180.f : 0.f;
    else if (x() == 0)
            return y() < 0 ? 90.f : 270.f;

    // don't use getLength here to avoid precision loss with s32 vectors
    float tmp = y() / std::sqrt((float)(x()*x() + y()*y()));
    tmp = std::atan( std::sqrt(1.f - tmp*tmp) / tmp) * math::RADTODEGFLT;

    if (x()>0 && y()>0)
      return tmp + 270;
    else if (x()>0 && y()<0)
      return tmp + 90;
    else if (x()<0 && y()<0)
      return 90 - tmp;
    else if (x()<0 && y()>0)
      return 270 - tmp;

    return tmp;
  }

  //! Calculates the angle between this vector and ano one in degree.
  /** \param b o vector to test with.
  \return Returns a value between 0 and 90. */
  inline float getAngleWith(const Vector2& b) const
  {
    double tmp = x()*b.x() + y()*b.y();

    if (tmp == 0.0)
      return 90.0f;

    tmp = tmp / std::sqrt((float)((x()*x() + y()*y()) * (b.x()*b.x() + b.y()*b.y())));
    if (tmp < 0.0)
      tmp = -tmp;

    return std::atan(std::sqrt(1 - tmp*tmp) / tmp) * math::RADTODEGFLT;
  }

  //! Returns if this vector interpreted as a point is on a line between two o points.
  /** It is assumed that the point is on the line.
  \param begin Beginning vector to compare between.
  \param end Ending vector to compare between.
  \return True if this vector is between begin and end, false if not. */
  bool isBetweenPoints(const Vector2& begin, const Vector2& end) const
  {
    if (begin.x != end.x)
    {
      return ((begin.x() <= x() && x() <= end.x()) ||
              (begin.x() >= x() && x() >= end.x()));
    }
    else
    {
      return ((begin.y() <= y() && y() <= end.y()) ||
              (begin.y() >= y() && y() >= end.y()));
    }
  }

  T& operator[](int index) { return _d[index]; }
  const T& operator[](int index) const { return _d[index]; }

  //! Creates an interpolated vector between this vector and ano vector.
  /** \param o The o vector to interpolate with.
  \param d Interpolation value between 0.0f (all the o vector) and 1.0f (all this vector).
  Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
  \return An interpolated vector.  This vector is not modified. */
  Vector2 getInterpolated(const Vector2& o, T d) const
  {
    float inv = 1.0f - (float)d;
    return Vector2((T)(o.x()*inv + x()*d), (T)(o.y()*inv + y()*d));
  }

  //! Creates a quadratically interpolated vector between this and two o vectors.
  /** \param v2 Second vector to interpolate with.
  \param v3 Third vector to interpolate with (maximum at 1.0f)
  \param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
  Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
  \return An interpolated vector. This vector is not modified. */
  Vector2 getInterpolated_quadratic(const Vector2& v2, const Vector2& v3, const T d) const
  {
    // this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
    const float inv = 1.0f - d;
    const float mul0 = inv * inv;
    const float mul1 = 2.0f * d * inv;
    const float mul2 = d * d;

    return Vector2 ( (T)(x() * mul0 + v2.x() * mul1 + v3.x() * mul2),
                     (T)(y() * mul0 + v2.y() * mul1 + v3.y() * mul2));
  }

  Vector2 cwiseMax(T rx, T ry) const { return Vector2(std::max(x(), rx), std::max(y(), ry)); }
  Vector2 cwiseMax(const Vector2& o) const { return{ std::max(x(), o.x()), std::max(y(), o.y()) }; }
  Vector2 cwiseMin(const Vector2& o) const { return{ std::min(x(), o.x()), std::min(y(), o.y()) }; }
  Vector2 fillZero(const Vector2& o) const { return{ x() ? x() : o.x(), y() ? y() : o.y() }; }

  template<class B>
  Vector2 alignTo(const Vector2<B>& size) const
  {
    Vector2 ret;
    ret.x() = (x() / size.x())*size.x();
    ret.y() = (y() / size.y())*size.y();
    return ret;
  }

  //! Sets this vector to the linearly interpolated vector between a and b.
  /** \param a first vector to interpolate with, maximum at 1.0f
  \param b second vector to interpolate with, maximum at 0.0f
  \param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
  Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
  */
  Vector2& interpolate(const Vector2& a, const Vector2& b, const T d)
  {
    x() = (T)((float)b.x() + ( ( a.x() - b.x() ) * d ));
    y() = (T)((float)b.y() + ( ( a.y() - b.y() ) * d ));
    return *this;
  }

  template <class B>
  inline Vector2<B> cast() const { return Vector2<B>(static_cast<B>(x()), static_cast<B>(y())); }

  inline Vector2<float> tofloat() const { return cast<float>(); }
  inline Vector2<int> toint() const { return cast<int>(); }

  inline Vector2<float> floor() const { return Vector2<float>(std::floor(x()), std::floor(y())); }
  inline Vector2<float> ceil() const { return Vector2<float>(std::ceil(x()), std::ceil(y())); }
  inline T squaredNorm() const { return x() * x() + y() * y(); }

  const T* data() const { return _d; }
  T* data() { return _d; }

  Vector2 unaryExpr(const std::function<T(T)>& f) { return Vector2(f(x()), f(y())); }
  Vector2 cwiseQuotient(const Vector2& divisor) { return Vector2(x() / divisor.x(), y() / divisor.y()); }

  T _d[2] = { 0 };
};

template <class T>
class Vector4
{
public:
  T& x() { return _d[0]; }
  T& y() { return _d[1]; }
  T& z() { return _d[2]; }
  T& w() { return _d[3]; }

  const T& x() const { return _d[0]; }
  const T& y() const { return _d[1]; }
  const T& z() const { return _d[2]; }
  const T& w() const { return _d[3]; }

  T width() const { return z() - x(); }
  T height() const { return w() - y(); }

  Vector2<T> size() const { return{ width(), height() }; }

  //! Default constructor (null vector)
  Vector4() : Vector4(0) {}
  //! Constructor with two different values
  Vector4(T nx, T ny, T nz, T nw) { x() = nx; y() = ny; z() = nz; w() = nw; }
  Vector4(const Vector2<T>& b, const Vector2<T>& e) { x() = b.x(); y() = b.y(); z() = e.x(); w() = e.y(); }
  //! Constructor with the same value for both members
  explicit Vector4(T n) : Vector4(n, n, n, n) {}
  //! Copy constructor
  Vector4(const Vector4& o) { for (int i=0; i < 4; i++) _d[i] = o._d[i]; }

  Vector2<T> lefttop() const { return Vector2<T>(x(), y()); }
  Vector2<T> lowerright() const { return Vector2<T>(z(), w()); }
  Vector2<T> center() const { return Vector2<T>((x() + z())/2, (y() + w())/2); }
  Vector4 operator+(const Vector2<T>& p) const
  {
    Vector4 r = *this;
    r.x() += p.x(); r.y() += p.y();
    r.z() += p.x(); r.w() += p.y();
    return r;
  }

  Vector4& operator+=(const Vector2<T>& p)
  {
    x() += p.x(); y() += p.y();
    z() += p.x(); w() += p.y();
    return *this;
  }

  Vector4& operator-=(const Vector2<T>& p)
  {
    x() -= p.x(); y() -= p.y();
    z() -= p.x(); w() -= p.y();
    return *this;
  }

  template<class B>
  Vector4 alignTo(const Vector2<B>& size) const
  {
    Vector4 ret;
    ret.x() = (x() / size.x())*size.x();
    ret.y() = (y() / size.y())*size.y();
    ret.z() = (z() / size.x())*size.x();
    ret.w() = (w() / size.y())*size.y();
    return ret;
  }

  Vector4 operator-(const Vector2<T>& p) const
  {
    Vector4 ret = *this;
    ret.x() -= p.x(); ret.y() -= p.y();
    ret.z() -= p.x(); ret.w() -= p.y();
    return ret;
  }

  template <class B>
  inline Vector4<B> cast() const { return Vector4<B>(static_cast<B>(x()), static_cast<B>(y()),
                                                     static_cast<B>(z()), static_cast<B>(w())); }

  const T* data() const { return _d; }
  T* data() { return _d; }

  bool isPointInside(const Vector2<T>& p) const
  {
    return (p.x() >= x() && p.y() >= y() && p.x() <= z() && p.y() <= w());
  }

  T _d[4] = { 0 };
};

using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;
using Vector4f = Vector4<float>;
using Vector4i = Vector4<int>;
using VectorXf = std::vector<float>;

inline Vector2f operator-(const Vector2i& p, const Vector2f o) { return (p.cast<float>() - o); }
inline Vector2f operator+(const Vector2i& p, const Vector2f o) { return (p.cast<float>() + o); }

using shared_bool = std::shared_ptr<bool>;
using shared_int = std::shared_ptr<int>;
using shared_float = std::shared_ptr<float>;

struct BoolObservable {
  shared_bool _b;
  std::function<bool()> _get;
  std::function<void(bool)> _set;
  BoolObservable() : _b(std::make_shared<bool>()) {}
  explicit BoolObservable(shared_bool b) : _b(b) {}
  explicit BoolObservable(std::function<bool()> g, std::function<void(bool)> s = nullptr)
    : _get(g), _set(s) {}

  explicit BoolObservable(std::function<bool& ()> _gs)
    : _get([_gs]() {return static_cast<bool>(_gs()); }),
      _set([_gs](bool v) { _gs() = v; }) {}

  operator bool() const { return _b ? *_b : _get ? _get() : false; }
  BoolObservable& operator &=(bool v) { bool t = *this; (*this) = t && v; return *this; }
  BoolObservable& operator=(bool v) { if (_b) { *_b = v; } else if (_set) {_set(v); }; return *this; }
};

struct BoolObservableRef : public BoolObservable {
  explicit BoolObservableRef(bool& ref)
    : BoolObservable([&]() {return ref;}, [&](bool v){ref = v;}) {}
};

template<class Scalar>
struct ScalarObservable {
  using _Scalar = Scalar;  //macosx clang happy
  std::shared_ptr<Scalar> _v;
  std::function<Scalar()> _get;
  std::function<void(Scalar)> _set;
  explicit ScalarObservable(const Scalar& v = Scalar()) : _v(std::make_shared<Scalar>()) { *_v = v; }
  explicit ScalarObservable(std::shared_ptr<Scalar> v) : _v(v) {}
  explicit ScalarObservable(std::function<Scalar()> g, std::function<void(Scalar)> s) : _get(g), _set(s) {}
  operator _Scalar() const { return _v ? *_v : _get ? _get() : _Scalar(); }
  ScalarObservable& operator=(_Scalar v) { if (_v) { *_v = v;} else if (_set) { _set(v); }; return *this; }
};

using IntObservable = ScalarObservable<int>;
using FloatObservable = ScalarObservable<float>;


//! Calculates the angle of this vector in degrees in the trigonometric sense.
/** 0 is to the right (3 o'clock), values increase counter-clockwise.
This method has been suggested by Pr3t3nd3r.
\return Returns a value between 0 and 360. */
inline float getAnglePointToAxis(const Vector2f& p)
{
  if (p.y() == 0)
    return p.x() < 0 ? 180.f : 0.f;
  else if (p.x() == 0)
    return p.y() < 0 ? 270.f : 90.f;

  if (p.y() > 0)
  {
    if (p.x() > 0)
      return std::atan((float)p.y() / (float)p.x()) * math::RADTODEGFLT;
    else
      return 180.0f - std::atan((float)p.y() / -(float)p.x()) * math::RADTODEGFLT;
  }
  else
  {
    if (p.x() > 0)
      return 360.0f - std::atan(-(float)p.y() / (float)p.x()) * math::RADTODEGFLT;
    else
      return 180.0f + std::atan(-(float)p.y() / -(float)p.x()) * math::RADTODEGFLT;
  }
}

// skip the forward declarations for the docs
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* Forward declarations */
template <typename T> class ref;

#endif // DOXYGEN_SHOULD_SKIP_THIS

/**
 * Static initialization; should be called once before invoking **any** NanoGUI
 * functions **if** you are having NanoGUI manage OpenGL / GLFW.  This method
 * is effectively a wrapper call to ``glfwInit()``, so if you are managing
 * OpenGL / GLFW on your own *do not call this method*.
 *
 * \rst
 * Refer to :ref:`nanogui_example_3` for how you might go about managing OpenGL
 * and GLFW on your own, while still using NanoGUI's classes.
 * \endrst
 */
NANOGUI_EXPORT void init();

/// Static shutdown; should be called before the application terminates.
NANOGUI_EXPORT void shutdown();

/**
 * \brief Open a native file open/save dialog.
 *
 * \param filetypes
 *     Pairs of permissible formats with descriptions like
 *     ``("png", "Portable Network Graphics")``.
 *
 * \param save
 *     Set to ``true`` if you would like subsequent file dialogs to open
 *     at whatever folder they were in when they close this one.
 */
NANOGUI_EXPORT std::vector<std::string>
file_dialog(const std::vector<std::pair<std::string, std::string>> &filetypes, bool save, bool multiple);

NANOGUI_EXPORT std::string
file_dialog(const std::vector<std::pair<std::string, std::string>> &filetypes, bool save);


NANOGUI_EXPORT bool isPointInsideRect(const Vector2i& p, const Vector4i& r);
NANOGUI_EXPORT bool isMouseActionRelease(int action);
NANOGUI_EXPORT bool isMouseActionPress(int action);
NANOGUI_EXPORT bool isMouseButtonLeft(int button);
NANOGUI_EXPORT bool isMouseButtonLeftMod(int button);
NANOGUI_EXPORT bool isMouseButtonRight(int button);

constexpr uint32_t FOURCC(const char* p) {
    return   (static_cast<uint32_t>(p[0]) << (8*0))
           | (static_cast<uint32_t>(p[1]) << (8*1))
           | (static_cast<uint32_t>(p[2]) << (8*2))
           | (static_cast<uint32_t>(p[3]) << (8*3));
}

inline uint32_t FOURCC(const std::string& p) { return FOURCC(p.c_str()); }

NANOGUI_EXPORT uint32_t key2fourcc(int key);

NANOGUI_EXPORT float getTimeFromStart();

NANOGUI_EXPORT bool isKeyboardActionRelease(int action);
NANOGUI_EXPORT bool isKeyboardModifierCtrl(int modifier);
NANOGUI_EXPORT bool isKeyboardModifierShift(int modifier);
NANOGUI_EXPORT bool isKeyboardActionPress(int action);
NANOGUI_EXPORT bool isKeyboardActionRepeat(int action);
NANOGUI_EXPORT bool isKeyboardKeyEscape(int key);
inline bool isKeyboardKey(int key, const std::string& fourcc) { return key2fourcc(key) == FOURCC(fourcc); }
inline bool isKeyboardKeys(int key, const std::vector<std::string>& fourcc) {
  for (auto& k : fourcc)
    if (isKeyboardKey(key, k))
      return true;
  return false;
}

/**
* \brief Determine whether an icon ID is a texture loaded via ``nvgImageIcon``.
*
* \rst
* The implementation defines all ``value < 1024`` as image icons, and
* everything ``>= 1024`` as an Entypo icon (see :ref:`file_nanogui_entypo.h`).
* The value ``1024`` exists to provide a generous buffer on how many images
* may have been loaded by NanoVG.
* \endrst
*
* \param value
*     The integral value of the icon.
*
* \return
*     Whether or not this is an image icon.
*/
inline bool nvgIsImageIcon(int value) { return value < 1024; }

/**
* \brief Determine whether an icon ID is a font-based icon (e.g. from ``entypo.ttf``).
*
* \rst
* See :func:`nanogui::nvgIsImageIcon` for details.
* \endrst
*
* \param value
*     The integral value of the icon.
*
* \return
*     Whether or not this is a font icon (from ``entypo.ttf``).
*/
inline bool nvgIsFontIcon(int value) { return value >= 1024; }

void NANOGUI_EXPORT nvgRect(NVGcontext* ctx, const Vector2i& pos, const Vector2i& size);
Vector2f NANOGUI_EXPORT nvgTextBounds(NVGcontext* ctx, float x, float y, const char* string, const char* end=nullptr);
Vector2f NANOGUI_EXPORT nvgTextBoxBounds(NVGcontext* ctx, float x, float y, float s, const char* string, const char* end=nullptr);
void NANOGUI_EXPORT nvgRect(NVGcontext* ctx, const Vector2f& pos, const Vector2f& size);
void NANOGUI_EXPORT nvgRect(NVGcontext* ctx, const Vector4i& r);
void NANOGUI_EXPORT nvgTranslate(NVGcontext* ctx, const Vector2i& pos);
void NANOGUI_EXPORT nvgTranslate(NVGcontext* ctx, const Vector2f& pos);
void NANOGUI_EXPORT nvgRoundedRect(NVGcontext* ctx, const Vector2i& p, const Vector2i& size, float r);
void NANOGUI_EXPORT nvgRoundedRect(NVGcontext* ctx, const Vector2f& p, const Vector2f& size, float r);
void NANOGUI_EXPORT nvgText(NVGcontext* ctx, const Vector2i& p, const std::string& text);
void NANOGUI_EXPORT nvgText(NVGcontext* ctx, const Vector2f& p, const std::string& text);
void NANOGUI_EXPORT nvgArc(NVGcontext* ctx, const Vector2f& c, float r, float a0, float a1, int dir);
void NANOGUI_EXPORT nvgCircle(NVGcontext* ctx, const Vector2f& c, float r);
void NANOGUI_EXPORT nvgFontFaceSize(NVGcontext* ctx, const char* font, float size);

template<class T> void nvgMoveTo(NVGcontext* ctx, const T& pos) { nvgMoveTo(ctx, pos.x(), pos.y()); }
template<class T> void nvgLineTo(NVGcontext* ctx, const T& pos) { nvgLineTo(ctx, pos.x(), pos.y()); }
template<class T> void nvgLine(NVGcontext* ctx, const T& from, const T& to) { nvgMoveTo(ctx, from); nvgLineTo(ctx, to); }

#if defined(__APPLE__) || defined(DOXYGEN_DOCUMENTATION_BUILD)
/**
 * \brief Move to the application bundle's parent directory
 *
 * This is function is convenient when deploying .app bundles on OSX. It
 * adjusts the file path to the parent directory containing the bundle.
 */
extern NANOGUI_EXPORT void chdir_to_bundle_parent();
#endif

/**
 * \brief Convert a single UTF32 character code to UTF8.
 *
 * \rst
 * NanoGUI uses this to convert the icon character codes
 * defined in :ref:`file_nanogui_entypo.h`.
 * \endrst
 *
 * \param c
 *     The UTF32 character to be converted.
 */
extern NANOGUI_EXPORT std::array<char, 8> utf8(int c);

/// Load a directory of PNG images and upload them to the GPU (suitable for use with ImagePanel)
extern NANOGUI_EXPORT std::vector<std::pair<int, std::string>>
    loadImageDirectory(NVGcontext *ctx, const std::string &path,
                       std::function<bool(const std::string&)> f = nullptr);

/// Convenience function for instanting a PNG icon from the application's data segment (via bin2c)
#define nvgImageIcon(ctx, name) __nanogui_get_image(ctx, #name, name##_png, name##_png_size)
template<typename T> T
clamp(const T& val, const T& min, const T& max) { return val < min ? min : (val > max ? max : val); }

void NANOGUI_EXPORT __nanogui_get_fontdata(const char* name, void*& data, uint32_t &datasize);
void NANOGUI_EXPORT logic_error(const char* err, const char* file, int line);

/// Helper function used by nvgImageIcon
int NANOGUI_EXPORT __nanogui_get_image(NVGcontext *ctx, const std::string &name, uint8_t *data, uint32_t size);

class Screen;
class Color;
namespace sample
{
  using WindowHandle = void*;
  WindowHandle NANOGUI_EXPORT create_window(int& w, int& h, const std::string& caption, bool resizable, bool fullscreen, bool header);
  Vector2i NANOGUI_EXPORT get_window_pos(WindowHandle w);
  Vector2i NANOGUI_EXPORT get_cursor_pos();
  void NANOGUI_EXPORT set_window_topmost(WindowHandle w, bool top, bool always);
  void NANOGUI_EXPORT set_window_pos(WindowHandle w, const Vector2i& pos);
  void NANOGUI_EXPORT destroy_window(WindowHandle wnd);
  void NANOGUI_EXPORT create_context();
  void NANOGUI_EXPORT run(std::function<void ()> func, int refresh = 50);
  void NANOGUI_EXPORT clear_frame(const Color& background);
  void NANOGUI_EXPORT setup_window_params(WindowHandle window, Screen* screen);
  void NANOGUI_EXPORT present_frame(WindowHandle window);
  void NANOGUI_EXPORT frame_loop(std::function<void()> &f);
  bool NANOGUI_EXPORT is_main_loop_active();
  void NANOGUI_EXPORT destroy_context();
  bool NANOGUI_EXPORT wait_events();
  bool NANOGUI_EXPORT poll_events();
  void NANOGUI_EXPORT stop_frame_loop();
  bool NANOGUI_EXPORT post_empty_event();
}

// Measures the specified text string height.
// Returns the vertical advance of the measured text (i.e. where the next character should drawn).
// Measured values are returned in local coordinate space.
float NANOGUI_EXPORT nvgTextHeight(NVGcontext* ctx, float x, float y, const char* string, const char* end, float* bounds);

extern NVGcontext* __nanogui_context;
void NANOGUI_EXPORT __nanogui_destroy_cursor(intptr_t cursor);
intptr_t NANOGUI_EXPORT __nanogui_create_cursor(int shape);

NAMESPACE_END(nanogui)

inline nanogui::Vector2i operator*(float v, const nanogui::Vector2i& t) { return (t.cast<float>() * v).cast<int>(); }
inline nanogui::Vector2f operator*(float v, const nanogui::Vector2f& t) { return t * v; }
