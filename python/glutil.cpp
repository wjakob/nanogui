#ifdef NANOGUI_PYTHON

#include "python.h"
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
                    M.itemsize(), glType, integral, M.data(), version);
}

static void setUniformPy(GLShader &sh, const std::string &name, py::object arg, bool warn = true) {
    GLuint id = sh.uniform(name, warn);
    py::array value_ = py::array::ensure(arg);
    auto dtype = value_.dtype();
    if (dtype.kind() == 'f') {
        auto value = py::array_t<float, py::array::forcecast>(value_);
        if (!value.check())
            throw py::type_error("setUniform(): invalid argument!");

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
        if (!value.check())
            throw py::type_error("setUniform(): invalid argument!");

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
        if (!value.check())
            throw py::type_error("setUniform(): invalid argument!");

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
        .def("uniform", &GLShader::attrib, py::arg("name"),
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
        .def("glEnable", [](GLShader, py::int_ cap) {
                glEnable(cap);
             }, py::arg("cap"))
        .def("glDisable", [](GLShader, py::int_ cap) {
                glDisable(cap);
             }, py::arg("cap"));

    #define C(name) gl.attr(#name) = py::int_(GL_##name);
    py::module gl = m.def_submodule("gl");

    C(TRIANGLES); C(LINES); C(POINTS);
    C(DEPTH_TEST);
}

#endif
