#ifdef NANOGUI_PYTHON

#include "python.h"

DECLARE_WIDGET(GLCanvas);

void register_glcanvas(py::module &m) {
    py::class_<GLCanvas, Widget, ref<GLCanvas>, PyGLCanvas> tbox(m, "GLCanvas", D(GLCanvas));
    glcanvas
        .def(py::init<Widget *, const std::string &>(), py::arg("parent"),
            D(GLCanvas, GLCanvas))
        .def("backgroundColor", &TextBox::backgroundColor, D(TextBox, backgroundColor))
        .def("setBackgroundColor", &TextBox::setBackgroundColor, D(TextBox, setBackgroundColor))
        .def("drawBorder", &TextBox::drawBorder, D(TextBox, drawBorder))
        .def("setDrawBorder", &TextBox::setDrawBorder, D(TextBox, setDrawBorder))
        .def("setDrawingCallback", &TextBox::setDrawingCallback, D(TextBox, setDrawingCallback));
}

#endif
