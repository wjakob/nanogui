#ifdef NANOGUI_PYTHON

#include "python.h"

void register_eigen(py::module &m) {
    py::class_<Color>(m, "Color", D(Color))
        .def(py::init<int, int, int, int>(), D(Color, Color, 7))
        .def(py::init<int, int>(), D(Color, Color, 5))
        .def(py::init<float, float, float, float>(), D(Color, Color, 7))
        .def(py::init<float, float>(), D(Color, Color, 5))
        .def("contrastingColor", &Color::contrastingColor,
             D(Color, contrastingColor))
        .def_property("r", [](const Color &c) { return c.r(); },
                      [](Color &c, float v) { c.r() = v; }, D(Color, r))
        .def_property("g", [](const Color &c) { return c.g(); },
                      [](Color &c, float v) { c.g() = v; }, D(Color, g))
        .def_property("b", [](const Color &c) { return c.b(); },
                      [](Color &c, float v) { c.b() = v; }, D(Color, b))
        .def_property("w", [](const Color &c) { return c.w(); },
                      [](Color &c, float v) { c.w() = v; }, "Return a reference to the alpha channel.")
        .def("__repr__", [](const Color &c) {
            std::ostringstream oss;
            oss << c.transpose();
            return oss.str();
        });
}
#endif
