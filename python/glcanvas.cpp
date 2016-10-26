#ifdef NANOGUI_PYTHON

#include "python.h"

class PyGLCanvas : public GLCanvas {
public:
  using GLCanvas::GLCanvas;
  NANOGUI_WIDGET_OVERLOADS(GLCanvas);

  void drawGL() {
    PYBIND11_OVERLOAD(void, GLCanvas, drawGL);
  }
};

void register_glcanvas(py::module &m) {
    py::class_<GLCanvas, Widget, ref<GLCanvas>, PyGLCanvas> glcanvas(m, "GLCanvas");
    glcanvas
        .def(py::init<Widget *>(), py::arg("parent"))
        .def("backgroundColor", &GLCanvas::backgroundColor)
        .def("setBackgroundColor", &GLCanvas::setBackgroundColor)
        .def("drawBorder", &GLCanvas::drawBorder)
        .def("setDrawBorder", &GLCanvas::setDrawBorder)
        .def("drawGL", &GLCanvas::drawGL);
}

#endif
