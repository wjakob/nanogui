/*
    nanogui/glutil.h -- Convenience classes for accessing OpenGL >= 3.x

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/opengl.h>
#include <Eigen/Geometry>
#include <map>

namespace half_float { class half; }

NAMESPACE_BEGIN(nanogui)

using Eigen::Quaternionf;

template <typename T> struct type_traits;
template <> struct type_traits<uint32_t> { enum { type = GL_UNSIGNED_INT, integral = 1 }; };
template <> struct type_traits<int32_t> { enum { type = GL_INT, integral = 1 }; };
template <> struct type_traits<uint16_t> { enum { type = GL_UNSIGNED_SHORT, integral = 1 }; };
template <> struct type_traits<int16_t> { enum { type = GL_SHORT, integral = 1 }; };
template <> struct type_traits<uint8_t> { enum { type = GL_UNSIGNED_BYTE, integral = 1 }; };
template <> struct type_traits<int8_t> { enum { type = GL_BYTE, integral = 1 }; };
template <> struct type_traits<double> { enum { type = GL_DOUBLE, integral = 0 }; };
template <> struct type_traits<float> { enum { type = GL_FLOAT, integral = 0 }; };
template <> struct type_traits<half_float::half> { enum { type = GL_HALF_FLOAT, integral = 0 }; };

/**
 * Helper class for compiling and linking OpenGL shaders and uploading
 * associated vertex and index buffers from Eigen matrices
 */
class NANOGUI_EXPORT GLShader {
public:
    /// Create an unitialized OpenGL shader
    GLShader()
        : mVertexShader(0), mFragmentShader(0), mGeometryShader(0),
          mProgramShader(0), mVertexArrayObject(0) { }

    /// Initialize the shader using the specified source strings
    bool init(const std::string &name, const std::string &vertex_str,
              const std::string &fragment_str,
              const std::string &geometry_str = "");

    /// Initialize the shader using the specified files on disk
    bool initFromFiles(const std::string &name,
                       const std::string &vertex_fname,
                       const std::string &fragment_fname,
                       const std::string &geometry_fname = "");

    /// Return the name of the shader
    const std::string &name() const { return mName; }

    /// Set a preprocessor definition
    void define(const std::string &key, const std::string &value) { mDefinitions[key] = value; }

    /// Select this shader for subsequent draw calls
    void bind();

    /// Release underlying OpenGL objects
    void free();

    /// Return the handle of a named shader attribute (-1 if it does not exist)
    GLint attrib(const std::string &name, bool warn = true) const;

    /// Return the handle of a uniform attribute (-1 if it does not exist)
    GLint uniform(const std::string &name, bool warn = true) const;

    /// Upload an Eigen matrix as a vertex buffer object (refreshing it as needed)
    template <typename Matrix> void uploadAttrib(const std::string &name, const Matrix &M, int version = -1) {
        uint32_t compSize = sizeof(typename Matrix::Scalar);
        GLuint glType = (GLuint) type_traits<typename Matrix::Scalar>::type;
        bool integral = (bool) type_traits<typename Matrix::Scalar>::integral;

        uploadAttrib(name, (uint32_t) M.size(), (int) M.rows(), compSize,
                     glType, integral, (const uint8_t *) M.data(), version);
    }

    /// Download a vertex buffer object into an Eigen matrix
    template <typename Matrix> void downloadAttrib(const std::string &name, Matrix &M) {
        uint32_t compSize = sizeof(typename Matrix::Scalar);
        GLuint glType = (GLuint) type_traits<typename Matrix::Scalar>::type;

        auto it = mBufferObjects.find(name);
        if (it == mBufferObjects.end())
            throw std::runtime_error("downloadAttrib(" + mName + ", " + name + ") : buffer not found!");

        const Buffer &buf = it->second;
        M.resize(buf.dim, buf.size / buf.dim);

        downloadAttrib(name, M.size(), M.rows(), compSize, glType, (uint8_t *) M.data());
    }

    /// Upload an index buffer
    template <typename Matrix> void uploadIndices(const Matrix &M) {
        uploadAttrib("indices", M);
    }

    /// Invalidate the version numbers assiciated with attribute data
    void invalidateAttribs();

    /// Completely free an existing attribute buffer
    void freeAttrib(const std::string &name);

    /// Check if an attribute was registered a given name
    bool hasAttrib(const std::string &name) const { 
        auto it = mBufferObjects.find(name);
        if (it == mBufferObjects.end())
            return false;
        return true;
    }

    /// Create a symbolic link to an attribute of another GLShader. This avoids duplicating unnecessary data
    void shareAttrib(const GLShader &otherShader, const std::string &name, const std::string &as = "");

    /// Return the version number of a given attribute
    int attribVersion(const std::string &name) const {
        auto it = mBufferObjects.find(name);
        if (it == mBufferObjects.end())
            return -1;
        return it->second.version;
    }

    /// Reset the version number of a given attribute
    void resetAttribVersion(const std::string &name) {
        auto it = mBufferObjects.find(name);
        if (it != mBufferObjects.end())
            it->second.version = -1;
    }

    /// Draw a sequence of primitives
    void drawArray(int type, uint32_t offset, uint32_t count);

    /// Draw a sequence of primitives using a previously uploaded index buffer
    void drawIndexed(int type, uint32_t offset, uint32_t count);

    /// Initialize a uniform parameter with a 4x4 matrix
    void setUniform(const std::string &name, const Matrix4f &mat, bool warn = true) {
        glUniformMatrix4fv(uniform(name, warn), 1, GL_FALSE, mat.data());
    }

    /// Initialize a uniform parameter with an integer value
    void setUniform(const std::string &name, int value, bool warn = true) {
        glUniform1i(uniform(name, warn), value);
    }

    /// Initialize a uniform parameter with a float value
    void setUniform(const std::string &name, float value, bool warn = true) {
        glUniform1f(uniform(name, warn), value);
    }

    /// Initialize a uniform parameter with a 2D vector
    void setUniform(const std::string &name, const Vector2f &v, bool warn = true) {
        glUniform2f(uniform(name, warn), v.x(), v.y());
    }

    /// Initialize a uniform parameter with a 3D vector
    void setUniform(const std::string &name, const Vector3f &v, bool warn = true) {
        glUniform3f(uniform(name, warn), v.x(), v.y(), v.z());
    }

    /// Initialize a uniform parameter with a 4D vector
    void setUniform(const std::string &name, const Vector4f &v, bool warn = true) {
        glUniform4f(uniform(name, warn), v.x(), v.y(), v.z(), v.w());
    }

    /// Return the size of all registered buffers in bytes
    size_t bufferSize() const {
        size_t size = 0;
        for (auto const &buf : mBufferObjects)
            size += buf.second.size;
        return size;
    }
protected:
    void uploadAttrib(const std::string &name, uint32_t size, int dim,
                       uint32_t compSize, GLuint glType, bool integral, 
                       const uint8_t *data, int version = -1);
    void downloadAttrib(const std::string &name, uint32_t size, int dim,
                       uint32_t compSize, GLuint glType, uint8_t *data);
protected:
    struct Buffer {
        GLuint id;
        GLuint glType;
        GLuint dim;
        GLuint compSize;
        GLuint size;
        int version;
    };
    std::string mName;
    GLuint mVertexShader;
    GLuint mFragmentShader;
    GLuint mGeometryShader;
    GLuint mProgramShader;
    GLuint mVertexArrayObject;
    std::map<std::string, Buffer> mBufferObjects;
    std::map<std::string, std::string> mDefinitions;
};

/// Helper class for creating framebuffer objects
class NANOGUI_EXPORT GLFramebuffer {
public:
    GLFramebuffer() : mFramebuffer(0), mDepth(0), mColor(0), mSamples(0) { }

    /// Create a new framebuffer with the specified size and number of MSAA samples
    void init(const Vector2i &size, int nSamples);

    /// Release all associated resources
    void free();

    /// Bind the framebuffer boject
    void bind();

    /// Release/unbind the framebuffer object
    void release();

    /// Blit the framebuffer object onto the screen
    void blit();

    /// Return whether or not the framebuffer object has been initialized
    bool ready() { return mFramebuffer != 0; }

    /// Return the number of MSAA samples
    int samples() const { return mSamples; }
protected:
    GLuint mFramebuffer, mDepth, mColor;
    Vector2i mSize;
    int mSamples;
};

/// Arcball helper class to interactively rotate objects on-screen
struct Arcball {
    Arcball(float speedFactor = 2.0f)
        : mActive(false), mLastPos(Vector2i::Zero()), mSize(Vector2i::Zero()),
          mQuat(Quaternionf::Identity()),
          mIncr(Quaternionf::Identity()),
          mSpeedFactor(speedFactor) { }

    Arcball(const Quaternionf &quat)
        : mActive(false), mLastPos(Vector2i::Zero()), mSize(Vector2i::Zero()),
          mQuat(quat),
          mIncr(Quaternionf::Identity()),
          mSpeedFactor(2.0f) { }

    Quaternionf &state() { return mQuat; }

    void setState(const Quaternionf &state) {
        mActive = false;
        mLastPos = Vector2i::Zero();
        mQuat = state;
        mIncr = Quaternionf::Identity();
    }

    void setSize(Vector2i size) { mSize = size; }
    const Vector2i &size() const { return mSize; }
    void setSpeedFactor(float speedFactor) { mSpeedFactor = speedFactor; }
    float speedFactor() const { return mSpeedFactor; }
    bool active() const { return mActive; }

    void button(Vector2i pos, bool pressed) {
        mActive = pressed;
        mLastPos = pos;
        if (!mActive)
            mQuat = (mIncr * mQuat).normalized();
        mIncr = Quaternionf::Identity();
    }

    bool motion(Vector2i pos) {
        if (!mActive)
            return false;

        /* Based on the rotation controller form AntTweakBar */
        float invMinDim = 1.0f / mSize.minCoeff();
        float w = (float) mSize.x(), h = (float) mSize.y();

        float ox = (mSpeedFactor * (2*mLastPos.x() - w) + w) - w - 1.0f;
        float tx = (mSpeedFactor * (2*pos.x()      - w) + w) - w - 1.0f;
        float oy = (mSpeedFactor * (h - 2*mLastPos.y()) + h) - h - 1.0f;
        float ty = (mSpeedFactor * (h - 2*pos.y())      + h) - h - 1.0f;

        ox *= invMinDim; oy *= invMinDim;
        tx *= invMinDim; ty *= invMinDim;

        Vector3f v0(ox, oy, 1.0f), v1(tx, ty, 1.0f);
        if (v0.squaredNorm() > 1e-4f && v1.squaredNorm() > 1e-4f) {
            v0.normalize(); v1.normalize();
            Vector3f axis = v0.cross(v1);
            float sa = std::sqrt(axis.dot(axis)),
                  ca = v0.dot(v1),
                  angle = std::atan2(sa, ca);
            if (tx*tx + ty*ty > 1.0f)
                angle *= 1.0f + 0.2f * (std::sqrt(tx*tx + ty*ty) - 1.0f);
            mIncr = Eigen::AngleAxisf(angle, axis.normalized());
            if (!std::isfinite(mIncr.norm()))
                mIncr = Quaternionf::Identity();
        }
        return true;
    }

    Matrix4f matrix() const {
        Matrix4f result2 = Matrix4f::Identity();
        result2.block<3,3>(0, 0) = (mIncr * mQuat).toRotationMatrix();
        return result2;
    }

protected:
    bool mActive;
    Vector2i mLastPos;
    Vector2i mSize;
    Quaternionf mQuat, mIncr;
    float mSpeedFactor;
};

extern NANOGUI_EXPORT Vector3f project(const Vector3f &obj, const Matrix4f &model,
                        const Matrix4f &proj, const Vector2i &viewportSize);

extern NANOGUI_EXPORT Vector3f unproject(const Vector3f &win, const Matrix4f &model,
                          const Matrix4f &proj, const Vector2i &viewportSize);

extern NANOGUI_EXPORT Matrix4f lookAt(const Vector3f &eye, const Vector3f &center,
                       const Vector3f &up);

extern NANOGUI_EXPORT Matrix4f ortho(const float left, const float right, const float bottom,
                      const float top, const float zNear, const float zFar);

extern NANOGUI_EXPORT Matrix4f frustum(const float left, const float right, const float bottom,
                        const float top, const float nearVal,
                        const float farVal);

extern NANOGUI_EXPORT Matrix4f scale(const Matrix4f &m, const Vector3f &v);

extern NANOGUI_EXPORT Matrix4f translate(const Matrix4f &m, const Vector3f &v);

NAMESPACE_END(nanogui)
