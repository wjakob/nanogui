#ifdef NANOGUI_PYTHON

#include "python.h"

DECLARE_WIDGET(GLCanvas);

void register_glcanvas(py::module &m) {
    py::class_<GLCanvas, Widget, ref<GLCanvas>, PyGLCanvas> tbox(m, "GLCanvas", D(GLCanvas));
    glcanvas
        .def(py::init<Widget *, const std::string &>(), py::arg("parent"),
            D(GLCanvas, GLCanvas))
        .def("backgroundColor", &GLCanvas::backgroundColor)
        .def("setBackgroundColor", &GLCanvas::setBackgroundColor)
        .def("drawBorder", &GLCanvas::drawBorder)
        .def("setDrawBorder", &GLCanvas::setDrawBorder)
        .def("setDrawingCallback", &GLCanvas::setDrawingCallback);
}

#endif
