/*
    nanogui/opengl.h -- Pulls in OpenGL, GLAD (if needed), GLFW, and
    NanoVG header files

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/common.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#if defined(NANOGUI_GLAD)
    #if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
        #define GLAD_GLAPI_EXPORT
    #endif

    #include <glad/glad.h>
#else
    #if defined(__APPLE__)
        #define GLFW_INCLUDE_GLCOREARB
    #else
        #define GL_GLEXT_PROTOTYPES
    #endif
#endif
#endif // DOXYGEN_SHOULD_SKIP_THIS

#include <GLFW/glfw3.h>
#include <nanovg.h>

// Special treatment of linux Nvidia opengl headers
#if !defined(_WIN32) && !defined(__APPLE__)
  #if !defined(GL_UNIFORM_BUFFER)
    #warning NanoGUI suspects you have the NVIDIA OpenGL headers installed.  \
             Compilation will likely fail. If it does, you have two choices: \
             (1) Re-install the mesa-libGL header files.                     \
             (2) Compile with NANOGUI_USE_GLAD.
  #endif
#endif

NAMESPACE_BEGIN(nanogui)

/// Allows for conversion between nanogui::Color and the NanoVG NVGcolor class.
inline Color::operator const NVGcolor &() const {
    return reinterpret_cast<const NVGcolor &>(*this->data());
}

/// Determine whether an icon ID is a texture loaded via nvgImageIcon
inline bool nvgIsImageIcon(int value) { return value < 1024; }

/// Determine whether an icon ID is a font-based icon (e.g. from the entypo.ttf font)
inline bool nvgIsFontIcon(int value) { return value >= 1024; }

NAMESPACE_END(nanogui)
