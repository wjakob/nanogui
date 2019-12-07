#if !defined(__PYTHON_H)
#define __PYTHON_H

#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>

#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include <nanogui/python.h>
#include "py_doc.h"

PYBIND11_MAKE_OPAQUE(nanogui::Color)

#define D(...) DOC(nanogui, __VA_ARGS__ )

#define DECLARE_LAYOUT(Name) \
    class Py##Name : public Name { \
    public: \
        using Name::Name; \
        NANOGUI_LAYOUT_OVERLOADS(Name); \
    }

#define DECLARE_WIDGET(Name) \
    class Py##Name : public Name { \
    public: \
        using Name::Name; \
        NANOGUI_WIDGET_OVERLOADS(Name); \
    }

#define DECLARE_SCREEN(Name) \
    class Py##Name : public Name { \
    public: \
        using Name::Name; \
        NANOGUI_WIDGET_OVERLOADS(Name); \
        NANOGUI_SCREEN_OVERLOADS(Name); \
    }

namespace py = pybind11;
using namespace nanogui;

/// Make pybind aware of the ref-counted wrapper type
PYBIND11_DECLARE_HOLDER_TYPE(T, ref<T>);

#endif /* __PYTHON_H */

