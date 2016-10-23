#ifdef NANOGUI_PYTHON

#include "python.h"

DECLARE_WIDGET(GLCanvas);

void register_glcanvas(py::module &m) {
    py::class_<GLCanvas, Widget, ref<GLCanvas>, PyGLCanvas> glcanvas(m, "GLCanvas");
    glcanvas
        .def(py::init<Widget *>(), py::arg("parent"))
        .def("backgroundColor", &GLCanvas::backgroundColor)
        .def("setBackgroundColor", &GLCanvas::setBackgroundColor)
        .def("drawBorder", &GLCanvas::drawBorder)
        .def("setDrawBorder", &GLCanvas::setDrawBorder)
        .def("setGLDrawingCallback", &GLCanvas::setGLDrawingCallback);
}

#endif
