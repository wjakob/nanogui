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

#include <Eigen/Core>
#include <stdint.h>
#include <array>
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

/* Import some common Eigen types */
/// Type alias to allow ``Eigen::Vector2f`` to be used as ``nanogui::Vector2f``.
using Vector2f = Eigen::Vector2f;
/// Type alias to allow ``Eigen::Vector3f`` to be used as ``nanogui::Vector3f``.
using Vector3f = Eigen::Vector3f;
/// Type alias to allow ``Eigen::Vector4f`` to be used as ``nanogui::Vector4f``.
using Vector4f = Eigen::Vector4f;
/// Type alias to allow ``Eigen::Vector2i`` to be used as ``nanogui::Vector2i``.
using Vector2i = Eigen::Vector2i;
/// Type alias to allow ``Eigen::Vector3i`` to be used as ``nanogui::Vector3i``.
using Vector3i = Eigen::Vector3i;
/// Type alias to allow ``Eigen::Vector4i`` to be used as ``nanogui::Vector4i``.
using Vector4i = Eigen::Vector4i;
/// Type alias to allow ``Eigen::Matrix3f`` to be used as ``nanogui::Matrix3f``.
using Matrix3f = Eigen::Matrix3f;
/// Type alias to allow ``Eigen::Matrix4f`` to be used as ``nanogui::Matrix4f``.
using Matrix4f = Eigen::Matrix4f;
/// Type alias to allow ``Eigen::VectorXf`` to be used as ``nanogui::VectorXf``.
using VectorXf = Eigen::VectorXf;
/// Type alias to allow ``Eigen::MatrixXf`` to be used as ``nanogui::MatrixXf``.
using MatrixXf = Eigen::MatrixXf;

/**
 * Convenience typedef for things like index buffers.  You would use it the same
 * as ``Eigen::MatrixXf``, only it is storing ``uint32_t`` instead of ``float``.
 */
using MatrixXu = Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic>;

/**
 * \class Color common.h nanogui/common.h
 *
 * \brief Stores an RGBA floating point color value.
 *
 * This class simply wraps around an ``Eigen::Vector4f``, providing some convenient
 * methods and terminology for thinking of it as a color.  The data operates in the
 * same way as ``Eigen::Vector4f``, and the following values are identical:
 *
 * \rst
 * +---------+-------------+-----------------------+-------------+
 * | Channel | Array Index | Eigen::Vector4f Value | Color Value |
 * +=========+=============+=======================+=============+
 * | Red     | ``0``       | x()                   | r()         |
 * +---------+-------------+-----------------------+-------------+
 * | Green   | ``1``       | y()                   | g()         |
 * +---------+-------------+-----------------------+-------------+
 * | Blue    | ``2``       | z()                   | b()         |
 * +---------+-------------+-----------------------+-------------+
 * | Alpha   | ``3``       | w()                   | w()         |
 * +---------+-------------+-----------------------+-------------+
 *
 * .. note::
 *    The method for the alpha component is **always** ``w()``.
 * \endrst
 *
 * You can and should still use the various convenience methods such as ``any()``,
 * ``all()``, ``head<index>()``, etc provided by Eigen.
 */
class Color
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

  Color(const Color& rgb, float _a)
  {
    *this = rgb;
    a() = _a;
  }

  Color& operator/=(float v) { 
    Color out = *this; 
    for (auto& a : out.rgba) a /= v;
    return out;
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

private:
  float rgba[4];
};

inline std::ostream& operator<<(std::ostream& os, const Color& c)
{
  os << c.r() << '.' << c.g() << '.' << c.b() << '.' << c.a();
  return os;
}

// skip the forward declarations for the docs
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* Forward declarations */
template <typename T> class ref;
class AdvancedGridLayout;
class BoxLayout;
class Button;
class CheckBox;
class ColorWheel;
class ColorPicker;
class ComboBox;
class GLFramebuffer;
class GLShader;
class GridLayout;
class GroupLayout;
class ImagePanel;
class ImageView;
class Label;
class Layout;
class MessageDialog;
class Object;
class Popup;
class PopupButton;
class ProgressBar;
class Screen;
class Serializer;
class Slider;
class StackedWidget;
class TabHeader;
class TabWidget;
class TextBox;
class GLCanvas;
class Theme;
class ToolButton;
class VScrollPanel;
class Widget;
class Window;

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
 * \brief Enter the application main loop
 *
 * \param refresh
 *     NanoGUI issues a redraw call whenever an keyboard/mouse/.. event is
 *     received. In the absence of any external events, it enforces a redraw
 *     once every ``refresh`` milliseconds. To disable the refresh timer,
 *     specify a negative value here.
 *
 * \param detach
 *     This parameter only exists in the Python bindings. When the active
 *     \c Screen instance is provided via the \c detach parameter, the
 *     ``mainloop()`` function becomes non-blocking and returns
 *     immediately (in this case, the main loop runs in parallel on a newly
 *     created thread). This feature is convenient for prototyping user
 *     interfaces on an interactive Python command prompt. When
 *     ``detach != None``, the function returns an opaque handle that
 *     will release any resources allocated by the created thread when the
 *     handle's ``join()`` method is invoked (or when it is garbage
 *     collected).
 *
 * \remark
 *     Unfortunately, Mac OS X strictly requires all event processing to take
 *     place on the application's main thread, which is fundamentally
 *     incompatible with this type of approach. Thus, NanoGUI relies on a
 *     rather crazy workaround on Mac OS (kudos to Dmitriy Morozov):
 *     ``mainloop()`` launches a new thread as before but then uses
 *     libcoro to swap the thread execution environment (stack, registers, ..)
 *     with the main thread. This means that the main application thread is
 *     hijacked and processes events in the main loop to satisfy the
 *     requirements on Mac OS, while the thread that actually returns from this
 *     function is the newly created one (paradoxical, as that may seem).
 *     Deleting or ``join()``ing the returned handle causes application to
 *     wait for the termination of the main loop and then swap the two thread
 *     environments back into their initial configuration.
 */
NANOGUI_EXPORT void mainloop(int refresh = 50);

/// Request the application main loop to terminate (e.g. if you detached mainloop).
NANOGUI_EXPORT void leave();

/// Return whether or not a main loop is currently active
NANOGUI_EXPORT bool active();

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
extern NANOGUI_EXPORT std::string
file_dialog(const std::vector<std::pair<std::string, std::string>> &filetypes,
            bool save);

NANOGUI_EXPORT bool isPointInsideRect(const Vector2i& p, const Vector4i& r);
NANOGUI_EXPORT bool isMouseActionRelease(int action);
NANOGUI_EXPORT bool isMouseActionPress(int action);
NANOGUI_EXPORT bool isMouseButtonLeft(int button);
NANOGUI_EXPORT bool isMouseButtonLeftMod(int button);
NANOGUI_EXPORT bool isMouseButtonRight(int button);

#define FOURCC(a,b,c,d) ( (uint32_t) (((d)<<24) | ((c)<<16) | ((b)<<8) | (a)) )
#define FOURCCS(s) ( (uint32_t) (((s[3])<<24) | ((s[2])<<16) | ((s[1])<<8) | (s[0])) )
NANOGUI_EXPORT uint32_t key2fourcc(int key);

NANOGUI_EXPORT bool appPostEmptyEvent();
NANOGUI_EXPORT void appForEachScreen(std::function<void(Screen*)> f);
NANOGUI_EXPORT bool appIsShouldCloseScreen(Screen* screen);
NANOGUI_EXPORT bool appWaitEvents();
NANOGUI_EXPORT bool appPollEvents();
NANOGUI_EXPORT float getTimeFromStart();

NANOGUI_EXPORT bool isKeyboardActionRelease(int action);
NANOGUI_EXPORT bool isKeyboardModifierCtrl(int modifier);
NANOGUI_EXPORT bool isKeyboardModifierShift(int modifier);
NANOGUI_EXPORT bool isKeyboardActionPress(int action);
NANOGUI_EXPORT bool isKeyboardActionRepeat(int action);
NANOGUI_EXPORT bool isKeyboardKeyEscape(int key);
inline bool isKeyboardKey(int key, const std::string& fourcc) { return key2fourcc(key) == FOURCCS(fourcc); }

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

/**
 * \brief Open a native file open dialog, which allows multiple selection.
 *
 * \param filetypes
 *     Pairs of permissible formats with descriptions like
 *     ``("png", "Portable Network Graphics")``.
 *
 * \param save
 *     Set to ``true`` if you would like subsequent file dialogs to open
 *     at whatever folder they were in when they close this one.
 *
 * \param multiple
 *     Set to ``true`` if you would like to be able to select multiple
 *     files at once. May not be simultaneously true with \p save.
 */
extern NANOGUI_EXPORT std::vector<std::string>
file_dialog(const std::vector<std::pair<std::string, std::string>> &filetypes,
            bool save, bool multiple);

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

extern NANOGUI_EXPORT void __nanogui_get_fontdata(const char* name, void*& data, uint32_t &datasize);

inline Vector2i NANOGUI_EXPORT rect_center(const Vector4i& r) { return Vector2i((r.z() + r.x()) / 2, (r.w() + r.y()) / 2); }

void NANOGUI_EXPORT logic_error(const char* err, const char* file, int line);

/// Helper function used by nvgImageIcon
extern NANOGUI_EXPORT int __nanogui_get_image(NVGcontext *ctx, const std::string &name, uint8_t *data, uint32_t size);

// Measures the specified text string height.
// Returns the vertical advance of the measured text (i.e. where the next character should drawn).
// Measured values are returned in local coordinate space.
extern NANOGUI_EXPORT float nvgTextHeight(NVGcontext* ctx, float x, float y, const char* string, const char* end, float* bounds);

NAMESPACE_END(nanogui)
