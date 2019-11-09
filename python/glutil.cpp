#ifdef NANOGUI_PYTHON

#include "python.h"
#include <nanogui/glutil.h>
#include <pybind11/numpy.h>

static void uploadAttribPy(GLShader &sh, const std::string &name, py::array M, int version) {
    if (M.ndim() != 2)
        throw py::type_error("uploadAttrib(): expects 2D array");

    M = py::array::ensure(M, py::array::f_style);

    py::dtype dtype = M.dtype();
    GLuint glType;
    bool integral = true;

    if (dtype.kind() == 'i') {
        switch (dtype.itemsize()) {
            case 1: glType = GL_BYTE; break;
            case 2: glType = GL_SHORT; break;
            case 4: glType = GL_INT; break;
            default: throw py::type_error("uploadAttrib(): Invalid integer type!");
        }
    } else if (dtype.kind() == 'u') {
        switch (dtype.itemsize()) {
            case 1: glType = GL_UNSIGNED_BYTE; break;
            case 2: glType = GL_UNSIGNED_SHORT; break;
            case 4: glType = GL_UNSIGNED_INT; break;
            default: throw py::type_error("uploadAttrib(): Invalid unsigned integer type!");
        }
    } else if (dtype.kind() == 'f') {
        switch (dtype.itemsize()) {
            case 2: glType = GL_HALF_FLOAT; break;
            case 4: glType = GL_FLOAT; break;
            case 8: glType = GL_DOUBLE; break;
            default: throw py::type_error("uploadAttrib(): Invalid floating point type!");
        }
        integral = false;
    } else {
        throw py::type_error("uploadAttrib(): Invalid type!");
    }

    sh.uploadAttrib(name, M.shape(0) * M.shape(1), (int) M.shape(0),
                    (uint32_t)M.itemsize(), glType, integral, M.data(), version);
}

static void setUniformPy(GLShader &sh, const std::string &name, py::object arg, bool warn = true) {
    GLuint id = sh.uniform(name, warn);
    py::array value_ = py::array::ensure(arg);
    auto dtype = value_.dtype();
    if (dtype.kind() == 'f') {
        auto value = py::array_t<float, py::array::forcecast | py::array::c_style>(value_);
        if (value.ndim() == 0 || (value.ndim() == 1 && value.shape(0) == 1))
            glUniform1fv(id, 1, value.data());
        else if (value.ndim() == 1 && value.shape(0) == 2)
            glUniform2fv(id, 1, value.data());
        else if (value.ndim() == 1 && value.shape(0) == 3)
            glUniform3fv(id, 1, value.data());
        else if (value.ndim() == 1 && value.shape(0) == 4)
            glUniform4fv(id, 1, value.data());
        else if (value.ndim() == 2 && value.shape(0) == 3 && value.shape(1) == 3)
            glUniformMatrix3fv(id, 1, GL_TRUE, value.data());
        else if (value.ndim() == 2 && value.shape(0) == 4 && value.shape(1) == 4)
            glUniformMatrix4fv(id, 1, GL_TRUE, value.data());
        else
            throw py::type_error("setUniform(): invalid dimension/size!");
    } else if (dtype.kind() == 'i') {
        auto value = py::array_t<int, py::array::forcecast>(value_);

        if (value.ndim() == 0 || (value.ndim() == 1 && value.shape(0) == 1))
            glUniform1iv(id, 1, value.data());
        else if (value.ndim() == 1 && value.shape(0) == 2)
            glUniform2iv(id, 1, value.data());
        else if (value.ndim() == 1 && value.shape(0) == 3)
            glUniform3iv(id, 1, value.data());
        else if (value.ndim() == 1 && value.shape(0) == 4)
            glUniform4iv(id, 1, value.data());
        else
            throw py::type_error("setUniform(): invalid dimension/size!");
    } else if (dtype.kind() == 'u') {
        auto value = py::array_t<unsigned int, py::array::forcecast>(value_);

        if (value.ndim() == 0 || (value.ndim() == 1 && value.shape(0) == 1))
            glUniform1uiv(id, 1, value.data());
        else if (value.ndim() == 1 && value.shape(0) == 2)
            glUniform2uiv(id, 1, value.data());
        else if (value.ndim() == 1 && value.shape(0) == 3)
            glUniform3uiv(id, 1, value.data());
        else if (value.ndim() == 1 && value.shape(0) == 4)
            glUniform4uiv(id, 1, value.data());
        else
            throw py::type_error("setUniform(): invalid dimension/size!");
    }
}

void register_glutil(py::module &m) {
    py::class_<GLShader>(m, "GLShader", D(GLShader))
        .def(py::init<>())
        .def("init", &GLShader::init, py::arg("name"),
             py::arg("vertex_str"), py::arg("fragment_str"),
             py::arg("geometry_str") = "", D(GLShader, init))
        .def("initFromFiles", &GLShader::initFromFiles, py::arg("name"),
             py::arg("vertex_fname"), py::arg("fragment_fname"),
             py::arg("geometry_fname") = "", D(GLShader, initFromFiles))
        .def("name", &GLShader::name, D(GLShader, name))
        .def("define", &GLShader::define, py::arg("key"), py::arg("value"),
             D(GLShader, define))
        .def("bind", &GLShader::bind, D(GLShader, bind))
        .def("free", &GLShader::free, D(GLShader, free))
        .def("attrib", &GLShader::attrib, py::arg("name"),
             py::arg("warn") = true, D(GLShader, attrib))
        .def("uniform", &GLShader::uniform, py::arg("name"),
             py::arg("warn") = true, D(GLShader, uniform))
        .def("uploadAttrib", &uploadAttribPy, py::arg("name"),
             py::arg("M"), py::arg("version") = -1)
        .def("uploadIndices", [](GLShader &sh, py::array M, int version) {
                uploadAttribPy(sh, "indices", M, version);
             }, py::arg("M"), py::arg("version") = -1)
        .def("invalidateAttribs", &GLShader::invalidateAttribs,
             D(GLShader, invalidateAttribs))
        .def("freeAttrib", &GLShader::freeAttrib,
             D(GLShader, freeAttrib))
        .def("hasAttrib", &GLShader::hasAttrib,
             D(GLShader, hasAttrib))
        .def("attribVersion", &GLShader::attribVersion,
             D(GLShader, attribVersion))
        .def("resetAttribVersion", &GLShader::resetAttribVersion,
             D(GLShader, resetAttribVersion))
        .def("shareAttrib", &GLShader::shareAttrib,
             D(GLShader, shareAttrib), py::arg("otherShader"),
             py::arg("name"), py::arg("as") = "")
        .def("drawArray", &GLShader::drawArray,
             D(GLShader, drawArray), py::arg("type"),
             py::arg("offset"), py::arg("count"))
        .def("drawIndexed", &GLShader::drawIndexed,
             D(GLShader, drawIndexed), py::arg("type"),
             py::arg("offset"), py::arg("count"))
        .def("setUniform", &setUniformPy, py::arg("name"),
             py::arg("value"), py::arg("warn") = true)
        .def("attribBuffer", &GLShader::attribBuffer, D(GLShader, attribBuffer));

    py::class_<GLShader::Buffer>(m, "Buffer", D(GLShader, Buffer))
        .def(py::init<>())
        .def_readonly("id", &GLShader::Buffer::id, D(GLShader, Buffer, id))
        .def_readonly("glType", &GLShader::Buffer::glType, D(GLShader, Buffer, glType))
        .def_readonly("dim", &GLShader::Buffer::dim, D(GLShader, Buffer, dim))
        .def_readonly("compSize", &GLShader::Buffer::compSize, D(GLShader, Buffer, compSize))
        .def_readonly("size", &GLShader::Buffer::size, D(GLShader, Buffer, size))
        .def_readonly("version", &GLShader::Buffer::version, D(GLShader, Buffer, version));

    py::class_<Arcball>(m, "Arcball", D(Arcball))
        .def(py::init<float>(), py::arg("speedFactor") = 2.f, D(Arcball, Arcball))
        .def(py::init<const Quaternionf &>(), D(Arcball, Arcball, 2))
        .def("state", (Quaternionf& (Arcball::*)()) &Arcball::state, D(Arcball, state))
        .def("setState", &Arcball::setState, D(Arcball, setState))
        .def("size", &Arcball::size, D(Arcball, size))
        .def("setSize", &Arcball::setSize, D(Arcball, setSize))
        .def("speedFactor", &Arcball::speedFactor, D(Arcball, speedFactor))
        .def("setSpeedFactor", &Arcball::setSpeedFactor, D(Arcball, setSpeedFactor))
        .def("active", &Arcball::active, D(Arcball, active))
        .def("button", &Arcball::button, py::arg("pos"), py::arg("pressed"), D(Arcball, button))
        .def("motion", &Arcball::motion, py::arg("pos"), D(Arcball, motion))
        .def("matrix", &Arcball::matrix, D(Arcball, matrix))
        .def("activeState", &Arcball::activeState, D(Arcball, activeState))
        .def("interrupt", &Arcball::interrupt, D(Arcball, interrupt));

    m.def("project", &project, py::arg("obj"), py::arg("model"),
          py::arg("proj"), py::arg("viewportSize"), D(project));

    m.def("unproject", &unproject, py::arg("win"), py::arg("model"),
          py::arg("proj"), py::arg("viewportSize"), D(unproject));

    m.def("lookAt", &lookAt, py::arg("origin"), py::arg("target"),
          py::arg("up"), D(lookAt));

    m.def("ortho", &ortho, py::arg("left"), py::arg("right"), py::arg("bottom"),
          py::arg("top"), py::arg("zNear"), py::arg("zFar"), D(ortho));

    m.def("frustum", &frustum, py::arg("left"), py::arg("right"), py::arg("bottom"),
          py::arg("top"), py::arg("nearVal"), py::arg("farVal"), D(frustum));

    m.def("scale", &scale, py::arg("v"), D(scale));

    m.def("translate", &translate, py::arg("v"), D(translate));

    /* Very basic OpenGL coverage */

    #define C(name) gl.attr(#name) = py::int_(GL_##name);
    py::module gl = m.def_submodule("gl", "OpenGL bindings");

    gl.def("Enable", [](GLenum cap) { glEnable(cap); }, py::arg("cap"));
    gl.def("Disable", [](GLenum cap) { glDisable(cap); }, py::arg("cap"));
    gl.def("BlendFunc", [](GLenum sfactor,
                           GLenum dfactor) { glBlendFunc(sfactor, dfactor); },
           py::arg("sfactor"), py::arg("dfactor"));
    gl.def("Scissor", [](GLint x, GLint y, GLsizei w, GLsizei h) { glScissor(x, y, w, h); });
    gl.def("Cull", [](GLenum mode) { glCullFace(mode); });
    gl.def("PointSize", [](GLfloat size) { glPointSize(size); });
    gl.def("LineWidth", [](GLfloat size) { glLineWidth(size); });

    /* Primitive types */
    C(POINTS); C(LINE_STRIP); C(LINE_LOOP); C(LINES); C(LINE_STRIP_ADJACENCY);
    C(LINES_ADJACENCY); C(TRIANGLE_STRIP); C(TRIANGLE_FAN); C(TRIANGLES);
    C(TRIANGLE_STRIP_ADJACENCY); C(TRIANGLES_ADJACENCY);

    /* Depth testing */
    C(DEPTH_TEST); C(NEVER); C(LESS); C(EQUAL); C(LEQUAL); C(GREATER);
    C(NOTEQUAL); C(GEQUAL); C(ALWAYS);

    /* Blend functions */
    C(BLEND); C(ZERO); C(ONE); C(SRC_COLOR); C(DST_COLOR);
    C(ONE_MINUS_DST_COLOR); C(SRC_ALPHA); C(ONE_MINUS_SRC_ALPHA);
    C(DST_ALPHA); C(ONE_MINUS_DST_ALPHA); C(CONSTANT_COLOR);
    C(ONE_MINUS_CONSTANT_COLOR); C(CONSTANT_ALPHA);
    C(ONE_MINUS_CONSTANT_ALPHA);

    /* Culling functions */
    C(FRONT); C(BACK); C(FRONT_AND_BACK);

    /* Remaining glEnable/glDisable enums */
    C(SCISSOR_TEST); C(STENCIL_TEST); C(PROGRAM_POINT_SIZE);
    C(LINE_SMOOTH); C(POLYGON_SMOOTH); C(CULL_FACE);
}

#endif
