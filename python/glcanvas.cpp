#ifdef NANOGUI_PYTHON

#include "python.h"
#include <nanogui/glcanvas.h>

class PyGLCanvas : public GLCanvas {
public:
    using GLCanvas::GLCanvas;
    NANOGUI_WIDGET_OVERLOADS(GLCanvas);

    void drawGL() {
        PYBIND11_OVERLOAD(void, GLCanvas, drawGL);
    }
};

void register_glcanvas(py::module &m) {
    py::class_<GLCanvas, Widget, ref<GLCanvas>, PyGLCanvas> glcanvas(m, "GLCanvas", D(GLCanvas));
    glcanvas
        .def(py::init<Widget *>(), py::arg("parent"), D(GLCanvas, GLCanvas))
        .def("backgroundColor", &GLCanvas::backgroundColor, D(GLCanvas, backgroundColor))
        .def("setBackgroundColor", &GLCanvas::setBackgroundColor, D(GLCanvas, setBackgroundColor))
        .def("drawBorder", &GLCanvas::drawBorder, D(GLCanvas, drawBorder))
        .def("setDrawBorder", &GLCanvas::setDrawBorder, D(GLCanvas, setDrawBorder))
        .def("drawGL", &GLCanvas::drawGL, D(GLCanvas, drawGL));
}

#endif
