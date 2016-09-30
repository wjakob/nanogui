#ifdef NANOGUI_PYTHON

#include "python.h"

void register_eigen(py::module &m) {
    py::handle vector2i = py::detail::get_type_handle(typeid(Vector2i), false);
    if (!vector2i) {
        py::class_<Vector2i>(m, "Vector2i")
            .def(py::init<int, int>())
            .def("__init__", [](Vector2i &v, py::list l) {
                if (l.size() != 2)
                    throw std::runtime_error("Incompatible list size!");
                new (&v) Vector2i(
                    l[0].cast<int>(),
                    l[1].cast<int>()
                );
            })
            .def("__init__", [](Vector2i &v, py::tuple t) {
                if (t.size() != 2)
                    throw std::runtime_error("Incompatible tuple size!");
                new (&v) Vector2i(
                    t[0].cast<int>(),
                    t[1].cast<int>()
                );
            })
            .def_property("x", [](const Vector2i &v) { return v.x(); }, [](Vector2i &v, int x) { v.x() = x; })
            .def_property("y", [](const Vector2i &v) { return v.y(); }, [](Vector2i &v, int y) { v.y() = y; })
            .def("__getitem__", [](const Vector2i &m, size_t i) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                return m[i];
             })
            .def("__setitem__", [](Vector2i &m, size_t i, int v) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                m[i] = v;
             });

        py::implicitly_convertible<py::list, Vector2i>();
        py::implicitly_convertible<py::tuple, Vector2i>();
    } else {
        /* Don't create a new type if some other library has already
           exposed (potentially much fancier) Eigen Python bindings */
        m.attr("Vector2i") = vector2i;
    }

    py::handle vector2f = py::detail::get_type_handle(typeid(Vector2f), false);
    if (!vector2f) {
        py::class_<Vector2f>(m, "Vector2f")
            .def(py::init<float, float>())
            .def("__init__", [](Vector2f &v, py::list l) {
                if (l.size() != 2)
                    throw std::runtime_error("Incompatible list size!");
                new (&v) Vector2i(
                    l[0].cast<float>(),
                    l[1].cast<float>()
                );
            })
            .def("__init__", [](Vector2f &v, py::tuple t) {
                if (t.size() != 2)
                    throw std::runtime_error("Incompatible tuple size!");
                new (&v) Vector2i(
                    t[0].cast<float>(),
                    t[1].cast<float>()
                );
            })
            .def_property("x", [](const Vector2f &v) { return v.x(); }, [](Vector2f &v, float x) { v.x() = x; })
            .def_property("y", [](const Vector2f &v) { return v.y(); }, [](Vector2f &v, float y) { v.y() = y; })
            .def("__getitem__", [](const Vector2f &m, size_t i) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                return m[i];
             })
            .def("__setitem__", [](Vector2f &m, size_t i, float v) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                m[i] = v;
             });
        py::implicitly_convertible<py::list, Vector2f>();
        py::implicitly_convertible<py::tuple, Vector2f>();
    } else {
        /* Don't create a new type if some other library has already
           exposed (potentially much fancier) Eigen Python bindings */
        m.attr("Vector2f") = vector2f;
    }

    py::handle vectorXf = py::detail::get_type_handle(typeid(VectorXf), false);
    if (!vectorXf) {
        py::class_<VectorXf>(m, "VectorXf")
            .def(py::init<>())
            .def(py::init<int>())
            .def("resize", [](VectorXf &v, int i) { v.resize(i); })
            .def("__init__", [](VectorXf &v, const std::vector<float> &v2) {
                new (&v) VectorXf(v2.size());
                memcpy(v.data(), &v2[0], sizeof(float) * v2.size());
            })
            .def("__init__", [](VectorXf &v, py::buffer b) {
                py::buffer_info info = b.request();
                if (info.format != py::format_descriptor<float>::value) {
                    throw std::runtime_error("Incompatible buffer format!");
                } else if (info.ndim == 1 && info.strides[0] == sizeof(float)) {
                    new (&v) VectorXf(info.shape[0]);
                    memcpy(v.data(), info.ptr, sizeof(float) * info.shape[0]);
                } else if (info.ndim == 2 && ((info.shape[0] == 1 && info.strides[0] == sizeof(float))
                                           || (info.shape[1] == 1 && info.strides[1] == sizeof(float)))) {
                    new (&v) VectorXf(info.shape[0] * info.shape[1]);
                    memcpy(v.data(), info.ptr, sizeof(float) * info.shape[0] * info.shape[1]);
                } else {
                    throw std::runtime_error("Incompatible buffer dimension!");
                }
            })
            .def("size", [](const VectorXf &v) { return v.size(); })
            .def("__repr__", [](const VectorXf &v) {
                std::ostringstream oss;
                oss << v.transpose();
                return oss.str();
            })
            .def("__getitem__", [](const VectorXf &m, size_t i) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                return m[i];
             })
            .def("__setitem__", [](VectorXf &m, size_t i, float v) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                m[i] = v;
             })

            /* Buffer access for interacting with NumPy */
            .def_buffer([](VectorXf &m) -> py::buffer_info {
                return py::buffer_info(
                    m.data(),               /* Pointer to buffer */
                    sizeof(float),          /* Size of one scalar */
                    /* Python struct-style format descriptor */
                    py::format_descriptor<float>::value,
                    1,                      /* Number of dimensions */
                    { (size_t) m.size() },  /* Buffer dimensions */
                    { sizeof(float) }       /* Strides (in bytes) for each index */
                );
             });
        py::implicitly_convertible<std::vector<float>, VectorXf>();
    } else {
        /* Don't create a new type if some other library has already
           exposed (potentially much fancier) Eigen Python bindings */
        m.attr("VectorXf") = vectorXf;
    }

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
