/*
    nanogui/opengl.h -- Pulls in OpenGL, GLEW (if needed), GLFW, and
    NanoVG header files

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/common.h>

#if defined(__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
#elif defined(_WIN32)
    #define GLAD_GLAPI_EXPORT
    #include <glad/glad.h>
#else
    #define GL_GLEXT_PROTOTYPES
#endif

#include <GLFW/glfw3.h>
#include <nanovg.h>

NAMESPACE_BEGIN(nanogui)

inline Color::operator const NVGcolor &() const {
    return reinterpret_cast<const NVGcolor &>(*this->data());
}

/// Determine whether an icon ID is a texture loaded via nvgImageIcon
inline bool nvgIsImageIcon(int value) { return value < 1024; }

/// Determine whether an icon ID is a font-based icon (e.g. from the entypo.ttf font)
inline bool nvgIsFontIcon(int value) { return value >= 1024; }

NAMESPACE_END(nanogui)
