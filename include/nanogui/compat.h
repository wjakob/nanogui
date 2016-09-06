/*
    nanogui/compat.h -- Compatibility layer

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <stdio.h>

#ifdef _MSC_VER
#define NANOGUI_SNPRINTF _snprintf
#else
/// Platform dependent snprintf (``_snprintf`` for MSVC, ``snprintf`` otherwise).
#define NANOGUI_SNPRINTF snprintf
#endif
