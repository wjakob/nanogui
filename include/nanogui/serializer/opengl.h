/*
    nanogui/serializer/opengl.h -- serialization support for OpenGL buffers

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/serializer/core.h>
#include <nanogui/glutil.h>
#include <set>

NAMESPACE_BEGIN(nanogui)
NAMESPACE_BEGIN(detail)

// bypass template specializations
#ifndef DOXYGEN_SHOULD_SKIP_THIS

template<>
struct serialization_helper<GLShader> {
    static std::string type_id() {
        return "G";
    }

    static void write(Serializer &s, const GLShader *value, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            if (count > 1)
                s.push(value->name());
            for (auto &item : value->mBufferObjects) {
                const GLShader::Buffer &buf = item.second;
                size_t totalSize = (size_t) buf.size * (size_t) buf.compSize;
                s.push(item.first);
                s.set("glType", buf.glType);
                s.set("compSize", buf.compSize);
                s.set("dim", buf.dim);
                s.set("size", buf.size);
                s.set("version", buf.version);
                Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> temp(1, totalSize);

                if (item.first == "indices") {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.id);
                    glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, totalSize,
                                       temp.data());
                } else {
                    glBindBuffer(GL_ARRAY_BUFFER, buf.id);
                    glGetBufferSubData(GL_ARRAY_BUFFER, 0, totalSize, temp.data());
                }
                s.set("data", temp);
                s.pop();
            }
            if (count > 1)
                s.pop();
            ++value;
        }
    }

    static void read(Serializer &s, GLShader *value, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            if (count > 1)
                s.push(value->name());
            auto all_keys = s.keys();

            std::set<std::string> keys;
            for (auto key : all_keys) {
                auto it = key.find(".");
                if (it != std::string::npos)
                    keys.insert(key.substr(0, it));
            }
            value->bind();
            for (auto key : keys) {
                if (value->mBufferObjects.find(key) == value->mBufferObjects.end()) {
                    GLuint bufferID;
                    glGenBuffers(1, &bufferID);
                    value->mBufferObjects[key].id = bufferID;
                }
                GLShader::Buffer &buf = value->mBufferObjects[key];
                Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> data;

                s.push(key);
                s.get("glType", buf.glType);
                s.get("compSize", buf.compSize);
                s.get("dim", buf.dim);
                s.get("size", buf.size);
                s.get("version", buf.version);
                s.get("data", data);
                s.pop();

                size_t totalSize = (size_t) buf.size * (size_t) buf.compSize;
                if (key == "indices") {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.id);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalSize,
                                 (void *) data.data(), GL_DYNAMIC_DRAW);
                } else {
                    int attribID = value->attrib(key);
                    glEnableVertexAttribArray(attribID);
                    glBindBuffer(GL_ARRAY_BUFFER, buf.id);
                    glBufferData(GL_ARRAY_BUFFER, totalSize, (void *) data.data(),
                                 GL_DYNAMIC_DRAW);
                    glVertexAttribPointer(attribID, buf.dim, buf.glType,
                                          buf.compSize == 1 ? GL_TRUE : GL_FALSE, 0, 0);
                }
            }
            if (count > 1)
                s.pop();
            ++value;
        }
    }
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

NAMESPACE_END(detail)
NAMESPACE_END(nanogui)
