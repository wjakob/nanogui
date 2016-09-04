#if !defined(__PYTHON_H)
#define __PYTHON_H

#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include <nanogui/python.h>
#include "py_doc.h"

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

#endif /* __PYTHON_H */
