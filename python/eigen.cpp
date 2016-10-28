#ifdef NANOGUI_PYTHON

#include "python.h"

void register_eigen(py::module &m) {
    py::class_<Color>(m, "Color", D(Color))
        .def(py::init<float, float, float, float>(), D(Color, Color, 7))
        .def(py::init<float, float>(), D(Color, Color, 5))
        .def("contrastingColor", &Color::contrastingColor, D(Color, contrastingColor))
        .def_property("r",
            [](const Color &c) { return c.r(); },
            [](Color &c, float v) { c.r() = v; },
            D(Color, r))
        .def_property("g",
            [](const Color &c) { return c.g(); },
            [](Color &c, float v) { c.g() = v; },
            D(Color, g))
        .def_property("b",
            [](const Color &c) { return c.b(); },
            [](Color &c, float v) { c.b() = v; },
            D(Color, b));
}
#endif
