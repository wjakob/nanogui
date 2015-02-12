#if !defined(__NANOGUI_OPENGL_H)
#define __NANOGUI_OPENGL_H

#include <nanogui/nanogui.h>

#if defined(__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
#elif defined(WIN32)
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #define GL_GLEXT_PROTOTYPES
#endif

#include <GLFW/glfw3.h>
#include <nanovg.h>

NANOGUI_NAMESPACE_BEGIN

inline Color::operator const NVGcolor &() const {
    return reinterpret_cast<const NVGcolor &>(*this->data());
}

/// Determine whether an icon ID is a texture loaded via nvgImageIcon
inline bool nvgIsImageIcon(int value) { return value < 1024; }

/// Determine whether an icon ID is a font-based icon (e.g. from the entypo.ttf font)
inline bool nvgIsFontIcon(int value) { return value >= 1024; }

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_OPENGL_H */
