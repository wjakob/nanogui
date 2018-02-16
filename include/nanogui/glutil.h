/*
    nanogui/glutil.h -- Convenience classes for accessing OpenGL >= 3.x

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/opengl.h>
#include <Eigen/Geometry>
#include <map>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace half_float { class half; }
#endif

#if !defined(GL_HALF_FLOAT) || defined(DOXYGEN_DOCUMENTATION_BUILD)
    /// Ensures that ``GL_HALF_FLOAT`` is defined properly for all platforms.
    #define GL_HALF_FLOAT 0x140B
#endif

NAMESPACE_BEGIN(nanogui)

// bypass template specializations
#ifndef DOXYGEN_SHOULD_SKIP_THIS

NAMESPACE_BEGIN(detail)
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
template <typename T> struct serialization_helper;
NAMESPACE_END(detail)

#endif // DOXYGEN_SHOULD_SKIP_THIS

using Eigen::Quaternionf;

class GLUniformBuffer;

//  ----------------------------------------------------

/**
 * \class GLShader glutil.h nanogui/glutil.h
 *
 * Helper class for compiling and linking OpenGL shaders and uploading
 * associated vertex and index buffers from Eigen matrices.
 */
class NANOGUI_EXPORT GLShader {
// this friendship breaks the documentation
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    template <typename T> friend struct detail::serialization_helper;
#endif
public:
    /**
     * \struct Buffer glutil.h nanogui/glutil.h
     *
     * A wrapper struct for maintaining various aspects of items being managed
     * by OpenGL.  Buffers are created when \ref GLShader::uploadAttrib is
     * called.
     */
    struct Buffer {
        GLuint id;      ///< The identifier used with OpenGL.
        GLuint glType;  ///< The OpenGL type of this buffer.
        GLuint dim;     ///< The dimension of this buffer (typically the row width).
        GLuint compSize;///< The size (in bytes) of an individual element in this buffer.
        GLuint size;    ///< The total number of elements represented by this buffer.
        int version;    ///< The current version if this buffer.
    };

    /// Create an unitialized OpenGL shader
    GLShader()
        : mVertexShader(0), mFragmentShader(0), mGeometryShader(0),
          mProgramShader(0), mVertexArrayObject(0) { }

    /**
     * \brief Initialize the shader using the specified source strings.
     *
     * \param name
     *     The name this shader will be registered as.
     *
     * \param vertex_str
     *     The source of the vertex shader as a string.
     *
     * \param fragment_str
     *     The source of the fragment shader as a string.
     *
     * \param geometry_str
     *     The source of the geometry shader as a string.  The default value is
     *     the empty string, which indicates no geometry shader will be used.
     */
    bool init(const std::string &name, const std::string &vertex_str,
              const std::string &fragment_str,
              const std::string &geometry_str = "");

    /**
     * \brief Initialize the shader using the specified files on disk.
     *
     * \param name
     *     The name this shader will be registered as.
     *
     * \param vertex_fname
     *     The path to the file containing the source of the fragment shader.
     *
     * \param fragment_fname
     *     The path to the file containing the source of the vertex shader.
     *
     * \param geometry_fname
     *     The path to the file containing the source of the geometry shader.
     *     The default value is the empty string, which indicates no geometry
     *     shader will be used.
     */
    bool initFromFiles(const std::string &name,
                       const std::string &vertex_fname,
                       const std::string &fragment_fname,
                       const std::string &geometry_fname = "");

    /// Return the name of the shader
    const std::string &name() const { return mName; }

    /**
     * Set a preprocessor definition.  Custom preprocessor definitions must be
     * added **before** initializing the shader (e.g., via \ref initFromFiles).
     * See also: \ref mDefinitions.
     */
    void define(const std::string &key, const std::string &value) { mDefinitions[key] = value; }

    /**
     * Select this shader for subsequent draw calls.  Simply executes ``glUseProgram``
     * with \ref mProgramShader, and ``glBindVertexArray`` with \ref mVertexArrayObject.
     */
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
        GLuint glType = (GLuint) detail::type_traits<typename Matrix::Scalar>::type;
        bool integral = (bool) detail::type_traits<typename Matrix::Scalar>::integral;

        uploadAttrib(name, (uint32_t) M.size(), (int) M.rows(), compSize,
                     glType, integral, M.data(), version);
    }

    /// Download a vertex buffer object into an Eigen matrix
    template <typename Matrix> void downloadAttrib(const std::string &name, Matrix &M) {
        uint32_t compSize = sizeof(typename Matrix::Scalar);
        GLuint glType = (GLuint) detail::type_traits<typename Matrix::Scalar>::type;

        auto it = mBufferObjects.find(name);
        if (it == mBufferObjects.end())
            throw std::runtime_error("downloadAttrib(" + mName + ", " + name + ") : buffer not found!");

        const Buffer &buf = it->second;
        M.resize(buf.dim, buf.size / buf.dim);

        downloadAttrib(name, M.size(), M.rows(), compSize, glType, M.data());
    }

    /// Upload an index buffer
    template <typename Matrix> void uploadIndices(const Matrix &M, int version = -1) {
        uploadAttrib("indices", M, version);
    }

    /// Invalidate the version numbers associated with attribute data
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

    /// Initialize a uniform parameter with a 4x4 matrix (float)
    template <typename T>
    void setUniform(const std::string &name, const Eigen::Matrix<T, 4, 4> &mat, bool warn = true) {
        glUniformMatrix4fv(uniform(name, warn), 1, GL_FALSE, mat.template cast<float>().data());
    }

    /// Initialize a uniform parameter with a 3x3 affine transform (float)
    template <typename T>
    void setUniform(const std::string &name, const Eigen::Transform<T, 3, 3> &affine, bool warn = true) {
        glUniformMatrix4fv(uniform(name, warn), 1, GL_FALSE, affine.template cast<float>().data());
    }

    /// Initialize a uniform parameter with a 3x3 matrix (float)
    template <typename T>
    void setUniform(const std::string &name, const Eigen::Matrix<T, 3, 3> &mat, bool warn = true) {
        glUniformMatrix3fv(uniform(name, warn), 1, GL_FALSE, mat.template cast<float>().data());
    }

    /// Initialize a uniform parameter with a 2x2 affine transform (float)
    template <typename T>
    void setUniform(const std::string &name, const Eigen::Transform<T, 2, 2> &affine, bool warn = true) {
        glUniformMatrix3fv(uniform(name, warn), 1, GL_FALSE, affine.template cast<float>().data());
    }

    /// Initialize a uniform parameter with a boolean value
    void setUniform(const std::string &name, bool value, bool warn = true) {
        glUniform1i(uniform(name, warn), (int)value);
    }

    /// Initialize a uniform parameter with an integer value
    template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 1, int>::type = 0>
    void setUniform(const std::string &name, T value, bool warn = true) {
        glUniform1i(uniform(name, warn), (int) value);
    }

    /// Initialize a uniform parameter with a floating point value
    template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 0, int>::type = 0>
    void setUniform(const std::string &name, T value, bool warn = true) {
        glUniform1f(uniform(name, warn), (float) value);
    }

    /// Initialize a uniform parameter with a 2D vector (int)
    template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 1, int>::type = 0>
    void setUniform(const std::string &name, const Eigen::Matrix<T, 2, 1>  &v, bool warn = true) {
        glUniform2i(uniform(name, warn), (int) v.x(), (int) v.y());
    }

    /// Initialize a uniform parameter with a 2D vector (float)
    template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 0, int>::type = 0>
    void setUniform(const std::string &name, const Eigen::Matrix<T, 2, 1>  &v, bool warn = true) {
        glUniform2f(uniform(name, warn), (float) v.x(), (float) v.y());
    }

    /// Initialize a uniform parameter with a 3D vector (int)
    template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 1, int>::type = 0>
    void setUniform(const std::string &name, const Eigen::Matrix<T, 3, 1>  &v, bool warn = true) {
        glUniform3i(uniform(name, warn), (int) v.x(), (int) v.y(), (int) v.z());
    }

    /// Initialize a uniform parameter with a 3D vector (float)
    template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 0, int>::type = 0>
    void setUniform(const std::string &name, const Eigen::Matrix<T, 3, 1>  &v, bool warn = true) {
        glUniform3f(uniform(name, warn), (float) v.x(), (float) v.y(), (float) v.z());
    }

    /// Initialize a uniform parameter with a 4D vector (int)
    template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 1, int>::type = 0>
    void setUniform(const std::string &name, const Eigen::Matrix<T, 4, 1>  &v, bool warn = true) {
        glUniform4i(uniform(name, warn), (int) v.x(), (int) v.y(), (int) v.z(), (int) v.w());
    }

    /// Initialize a uniform parameter with a 4D vector (float)
    template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 0, int>::type = 0>
    void setUniform(const std::string &name, const Eigen::Matrix<T, 4, 1>  &v, bool warn = true) {
        glUniform4f(uniform(name, warn), (float) v.x(), (float) v.y(), (float) v.z(), (float) v.w());
    }

    /// Initialize a uniform buffer with a uniform buffer object
    void setUniform(const std::string &name, const GLUniformBuffer &buf, bool warn = true);

    /// Return the size of all registered buffers in bytes
    size_t bufferSize() const {
        size_t size = 0;
        for (auto const &buf : mBufferObjects)
            size += buf.second.size;
        return size;
    }

    /**
     * \brief (Advanced) Returns a reference to the specified \ref GLShader::Buffer.
     *
     * \rst
     * .. danger::
     *
     *    Extreme caution must be exercised when using this method.  The user is
     *    discouraged from explicitly storing the reference returned, as it can
     *    change, become deprecated, or no longer reside in
     *    :member:`mBufferObjects <nanogui::GLShader::mBufferObjects>`.
     *
     *    There are generally very few use cases that justify using this method
     *    directly.  For example, if you need the version of a buffer, call
     *    :func:`attribVersion <nanogui::GLShader::attribVersion>`.  If you want
     *    to share data between :class:`GLShader <nanogui::GLShader>` objects,
     *    call :func:`shareAttrib <nanogui::GLShader::shareAttrib>`.
     *
     *    One example use case for this method is sharing data between different
     *    GPU pipelines such as CUDA or OpenCL.  When sharing data, you
     *    typically need to map pointers between the API's.  The returned
     *    buffer's :member:`Buffer::id <nanogui::GLShader::Buffer::id>` is the
     *    ``GLuint`` you will want to map to the other API.
     *
     *    In short, only use this method if you absolutely need to.
     * \endrst
     *
     * \param name
     *     The name of the desired attribute.
     *
     * \return
     *     A reference to the current buffer associated with ``name``.  Should
     *     not be explicitly stored.
     *
     * \throws std::runtime_error
     *     If ``name`` is not found.
     */
    const Buffer &attribBuffer(const std::string &name);

public:
    /* Low-level API */
    void uploadAttrib(const std::string &name, size_t size, int dim,
                       uint32_t compSize, GLuint glType, bool integral,
                       const void *data, int version = -1);
    void downloadAttrib(const std::string &name, size_t size, int dim,
                       uint32_t compSize, GLuint glType, void *data);

protected:
    /// The registered name of this GLShader.
    std::string mName;

    /// The vertex shader of this GLShader (as returned by ``glCreateShader``).
    GLuint mVertexShader;

    /// The fragment shader of this GLShader (as returned by ``glCreateShader``).
    GLuint mFragmentShader;

    /// The geometry shader (if requested) of this GLShader (as returned by ``glCreateShader``).
    GLuint mGeometryShader;

    /// The OpenGL program (as returned by ``glCreateProgram``).
    GLuint mProgramShader;

    /// The vertex array associated with this GLShader (as returned by ``glGenVertexArrays``).
    GLuint mVertexArrayObject;

    /**
     * The map of string names to buffer objects representing the various
     * attributes that have been uploaded using \ref uploadAttrib.
     */
    std::map<std::string, Buffer> mBufferObjects;

    /**
     * \rst
     * The map of preprocessor names to values (if any have been created).  If
     * a definition was added seeking to create ``#define WIDTH 256``, the key
     * would be ``"WIDTH"`` and the value would be ``"256"``.  These definitions
     * will be included automatically in the string that gets compiled for the
     * vertex, geometry, and fragment shader code.
     * \endrst
     */
    std::map<std::string, std::string> mDefinitions;
};

//  ----------------------------------------------------

/**
 * \class GLUniformBuffer glutil.h nanogui/glutil.h
 *
 * \brief Helper class for creating OpenGL Uniform Buffer objects.
 */
class NANOGUI_EXPORT GLUniformBuffer {
public:
    /// Default constructor: unusable until you call the ``init()`` method
    GLUniformBuffer() : mID(0), mBindingPoint(0) { }

    /// Create a new uniform buffer
    void init();

    /// Release underlying OpenGL object
    void free();

    /// Bind the uniform buffer to a specific binding point
    void bind(int index);

    /// Release/unbind the uniform buffer
    void release();

    /// Update content on the GPU using data
    void update(const std::vector<uint8_t> &data);

    /// Return the binding point of this uniform buffer
    int getBindingPoint() const { return mBindingPoint; }
private:
    GLuint mID;
    int mBindingPoint;
};

//  ----------------------------------------------------

/**
 * \class UniformBufferStd140 glutil.h nanogui/glutil.h
 *
 * \brief Helper class for accumulating uniform buffer data following the
 *        'std140' packing format.
 */
class UniformBufferStd140 : public std::vector<uint8_t> {
public:
    using Parent = std::vector<uint8_t>;

    using Parent::push_back;

    template <typename T, typename std::enable_if<std::is_pod<T>::value, int>::type = 0>
    void push_back(T value) {
        uint8_t *tmp = (uint8_t*) &value;
        for (int i = 0; i < sizeof(T); i++)
            Parent::push_back(tmp[i]);
    }

    template <typename Derived, typename std::enable_if<Derived::IsVectorAtCompileTime, int>::type = 0>
    void push_back(const Eigen::MatrixBase<Derived> &value) {
        const int n = (int) value.size();
        int i;
        for (i = 0; i < n; ++i)
            push_back(value[i]);
        const int pad = n == 1 ? 1 : (n == 2 ? 2 : 4);
        while ((i++) % pad != 0)
            push_back((typename Derived::Scalar) 0);
    }

    template <typename Derived, typename std::enable_if<!Derived::IsVectorAtCompileTime, int>::type = 0>
    void push_back(const Eigen::MatrixBase<Derived> &value, bool colMajor = true) {
        const int n = (int) (colMajor ? value.rows() : value.cols());
        const int m = (int) (colMajor ? value.cols() : value.rows());
        const int pad = n == 1 ? 1 : (n == 2 ? 2 : 4);

        for (int i = 0; i < m; ++i) {
            int j;
            for (j = 0; j < n; ++j)
                push_back(colMajor ? value(j, i) : value(i, j));
            while ((j++) % pad != 0)
                push_back((typename Derived::Scalar) 0);
        }
    }
};

//  ----------------------------------------------------

/**
 * \class GLFramebuffer glutil.h nanogui/glutil.h
 *
 * \brief Helper class for creating framebuffer objects.
 */
class NANOGUI_EXPORT GLFramebuffer {
public:
    /// Default constructor: unusable until you call the ``init()`` method
    GLFramebuffer() : mFramebuffer(0), mDepth(0), mColor(0), mSamples(0) { }

    /// Create a new framebuffer with the specified size and number of MSAA samples
    void init(const Vector2i &size, int nSamples);

    /// Release all associated resources
    void free();

    /// Bind the framebuffer object
    void bind();

    /// Release/unbind the framebuffer object
    void release();

    /// Blit the framebuffer object onto the screen
    void blit();

    /// Return whether or not the framebuffer object has been initialized
    bool ready() { return mFramebuffer != 0; }

    /// Return the number of MSAA samples
    int samples() const { return mSamples; }

    /// Quick and dirty method to write a TGA (32bpp RGBA) file of the framebuffer contents for debugging
    void downloadTGA(const std::string &filename);
protected:
    GLuint mFramebuffer, mDepth, mColor;
    Vector2i mSize;
    int mSamples;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

//  ----------------------------------------------------

/**
 * \struct Arcball glutil.h nanogui/glutil.h
 *
 * \brief Arcball helper class to interactively rotate objects on-screen.
 *
 * The Arcball class enables fluid interaction by representing rotations using
 * a quaternion, and is setup to be used in conjunction with the existing
 * mouse callbacks defined in \ref nanogui::Widget.  The Arcball operates by
 * maintaining an "active" state which is typically controlled using a mouse
 * button click / release.  A click pressed would call \ref Arcball::button
 * with ``down = true``, and a click released with ``down = false``.  The high
 * level mechanics are:
 *
 * 1. The Arcball is made active by calling \ref Arcball::button with a
 *    specified click location, and ``down = true``.
 * 2. As the user holds the mouse button down and drags, calls to
 *    \ref Arcball::motion are issued.  Internally, the Arcball keeps track of
 *    how far the rotation is from the start click.  During the active state,
 *    \ref mQuat is not updated, call \ref Arcball::matrix to get the current
 *    rotation for use in drawing updates.  Receiving the rotation as a matrix
 *    will usually be more convenient for traditional pipelines, however you
 *    can also acquire the active rotation using \ref Arcball::activeState.
 * 3. The user releases the mouse button, and a call to \ref Arcball::button
 *    with ``down = false``.  The Arcball is no longer active, and its internal
 *    \ref mQuat is updated.
 *
 * A very simple \ref nanogui::Screen derived class to illustrate usage:
 *
 * \rst
 * .. code-block:: cpp
 *
 *    class ArcballScreen : public nanogui::Screen {
 *    public:
 *        // Creating a 400x400 window
 *        ArcballScreen() : nanogui::Screen({400, 400}, "ArcballDemo") {
 *            mArcball.setSize(mSize);// Note 1
 *        }
 *
 *        virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override {
 *            // In this example, we are using the left mouse button
 *            // to control the arcball motion
 *            if (button == GLFW_MOUSE_BUTTON_1) {
 *                mArcball.button(p, down);// Note 2
 *                return true;
 *            }
 *            return false;
 *        }
 *
 *        virtual bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override {
 *            if (button == GLFW_MOUSE_BUTTON_1) {
 *                mArcball.motion(p);// Note 2
 *                return true;
 *            }
 *            return false;
 *        }
 *
 *        virtual void drawContents() override {
 *            // Option 1: acquire a 4x4 homogeneous rotation matrix
 *            Matrix4f rotation = mArcball.matrix();
 *            // Option 2: acquire an equivalent quaternion
 *            Quaternionf rotation = mArcball.activeState();
 *            // ... do some drawing with the current rotation ...
 *        }
 *
 *    protected:
 *        nanogui::Arcball mArcball;
 *    };
 *
 * **Note 1**
 *     The user is responsible for setting the size with
 *     :func:`Arcball::setSize <nanogui::Arcball::setSize>`, this does **not**
 *     need to be the same as the Screen dimensions (e.g., you are using the
 *     Arcball to control a specific ``glViewport``).
 *
 * **Note 2**
 *     Be aware that the input vector ``p`` to
 *     :func:`Widget::mouseButtonEvent <nanogui::Widget::mouseButtonEvent>`
 *     and :func:`Widget::mouseMotionEvent <nanogui::Widget::mouseMotionEvent>`
 *     are in the coordinates of the Screen dimensions (top left is ``(0, 0)``,
 *     bottom right is ``(width, height)``).  If you are using the Arcball to
 *     control a subregion of the Screen, you will want to transform the input
 *     ``p`` before calling :func:`Arcball::button <nanogui::Arcball::button>`
 *     or :func:`Arcball::motion <nanogui::Arcball::motion>`.  For example, if
 *     controlling the right half of the screen, you might create
 *     ``Vector2i adjusted_click(p.x() - (mSize.x() / 2), p.y())``, and then
 *     call ``mArcball.motion(adjusted_click)``.
 * \endrst
 */
struct Arcball {
    /**
     * \brief The default constructor.
     *
     * \rst
     * .. note::
     *
     *    Make sure to call :func:`Arcball::setSize <nanogui::Arcball::setSize>`
     *    after construction.
     * \endrst
     *
     * \param speedFactor
     *     The speed at which the Arcball rotates (default: ``2.0``).  See also
     *     \ref mSpeedFactor.
     */
    Arcball(float speedFactor = 2.0f)
        : mActive(false), mLastPos(Vector2i::Zero()), mSize(Vector2i::Zero()),
          mQuat(Quaternionf::Identity()),
          mIncr(Quaternionf::Identity()),
          mSpeedFactor(speedFactor) { }

    /**
     * Constructs an Arcball based off of the specified rotation.
     *
     * \rst
     * .. note::
     *
     *    Make sure to call :func:`Arcball::setSize <nanogui::Arcball::setSize>`
     *    after construction.
     * \endrst
     */
    Arcball(const Quaternionf &quat)
        : mActive(false), mLastPos(Vector2i::Zero()), mSize(Vector2i::Zero()),
          mQuat(quat),
          mIncr(Quaternionf::Identity()),
          mSpeedFactor(2.0f) { }

    /**
     * \brief The internal rotation of the Arcball.
     *
     * Call \ref Arcball::matrix for drawing loops, this method will not return
     * any updates while \ref mActive is ``true``.
     */
    Quaternionf &state() { return mQuat; }

    /// ``const`` version of \ref Arcball::state.
    const Quaternionf &state() const { return mQuat; }

    /// Sets the rotation of this Arcball.  The Arcball will be marked as **not** active.
    void setState(const Quaternionf &state) {
        mActive = false;
        mLastPos = Vector2i::Zero();
        mQuat = state;
        mIncr = Quaternionf::Identity();
    }

    /**
     * \brief Sets the size of this Arcball.
     *
     * The size of the Arcball and the positions being provided in
     * \ref Arcball::button and \ref Arcball::motion are directly related.
     */
    void setSize(Vector2i size) { mSize = size; }

    /// Returns the current size of this Arcball.
    const Vector2i &size() const { return mSize; }

    /// Sets the speed at which this Arcball rotates.  See also \ref mSpeedFactor.
    void setSpeedFactor(float speedFactor) { mSpeedFactor = speedFactor; }

    /// Returns the current speed at which this Arcball rotates.
    float speedFactor() const { return mSpeedFactor; }

    /// Returns whether or not this Arcball is currently active.
    bool active() const { return mActive; }

    /**
     * \brief Signals a state change from active to non-active, or vice-versa.
     *
     * \param pos
     *     The click location, should be in the same coordinate system as
     *     specified by \ref mSize.
     *
     * \param pressed
     *     When ``true``, this Arcball becomes active.  When ``false``, this
     *     Arcball becomes non-active, and its internal \ref mQuat is updated
     *     with the final rotation.
     */
    void button(Vector2i pos, bool pressed) {
        mActive = pressed;
        mLastPos = pos;
        if (!mActive)
            mQuat = (mIncr * mQuat).normalized();
        mIncr = Quaternionf::Identity();
    }

    /**
     * \brief When active, updates \ref mIncr corresponding to the specified
     *        position.
     *
     * \param pos
     *     Where the mouse has been dragged to.
     */
    bool motion(Vector2i pos) {
        if (!mActive)
            return false;

        /* Based on the rotation controller from AntTweakBar */
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

    /**
     * Returns the current rotation *including* the active motion, suitable for
     * use with typical homogeneous matrix transformations.  The upper left 3x3
     * block is the rotation matrix, with 0-0-0-1 as the right-most column /
     * bottom row.
     */
    Matrix4f matrix() const {
        Matrix4f result2 = Matrix4f::Identity();
        result2.block<3,3>(0, 0) = (mIncr * mQuat).toRotationMatrix();
        return result2;
    }

    /// Returns the current rotation *including* the active motion.
    Quaternionf activeState() const { return mIncr * mQuat; }

    /**
     * \brief Interrupts the current Arcball motion by calling
     *        \ref Arcball::button with ``(0, 0)`` and ``false``.
     *
     * Use this method to "close" the state of the Arcball when a mouse release
     * event is not available.  You would use this method if you need to stop
     * the Arcball from updating its internal rotation, but the event stopping
     * the rotation does **not** come from a mouse release.  For example, you
     * have a callback that created a \ref nanogui::MessageDialog which will now
     * be in focus.
     */
    void interrupt() { button(Vector2i::Zero(), false); }

protected:
    /// Whether or not this Arcball is currently active.
    bool mActive;

    /// The last click position (which triggered the Arcball to be active / non-active).
    Vector2i mLastPos;

    /// The size of this Arcball.
    Vector2i mSize;

    /**
     * The current stable state.  When this Arcball is active, represents the
     * state of this Arcball when \ref Arcball::button was called with
     * ``down = true``.
     */
    Quaternionf mQuat;

    /// When active, tracks the overall update to the state.  Identity when non-active.
    Quaternionf mIncr;

    /**
     * The speed at which this Arcball rotates.  Smaller values mean it rotates
     * more slowly, higher values mean it rotates more quickly.
     */
    float mSpeedFactor;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

//  ----------------------------------------------------

/**
 * \brief Projects the vector ``obj`` into the specified viewport.
 *
 * Performs a homogeneous transformation of a vector into "screen space", as
 * defined by the provided model and projection matrices, and the dimensions
 * of the viewport.
 *
 * \param obj
 *     The vector being transformed.
 *
 * \param model
 *     The model matrix.
 *
 * \param proj
 *     The projection matrix.
 *
 * \param viewportSize
 *     The dimensions of the viewport to project into.
 */
extern NANOGUI_EXPORT Vector3f project(const Vector3f &obj,
                                       const Matrix4f &model,
                                       const Matrix4f &proj,
                                       const Vector2i &viewportSize);

/**
 * \brief Unprojects the vector ``win`` out of the specified viewport.
 *
 * The reverse transformation of \ref project --- use the same matrices and
 * viewport dimensions to easily transition between the two spaces.
 *
 * \param win
 *     The vector being transformed out of "screen space".
 *
 * \param model
 *     The model matrix.
 *
 * \param proj
 *     The projection matrix.
 *
 * \param viewportSize
 *     The dimensions of the viewport to project out of.
 */
extern NANOGUI_EXPORT Vector3f unproject(const Vector3f &win,
                                         const Matrix4f &model,
                                         const Matrix4f &proj,
                                         const Vector2i &viewportSize);

/**
 * \brief Creates a "look at" matrix that describes the position and
 * orientation of e.g. a camera
 *
 * \param origin
 *     The position of the camera.
 *
 * \param target
 *     The gaze target of the camera.
 *
 * \param up
 *     The up vector of the camera.
 *
 * \rst
 * .. warning::
 *    These are used to form an orthonormal basis.  The first basis vector is
 *    defined as ``f = (target - origin).normalized()``.
 * \endrst
 */
extern NANOGUI_EXPORT Matrix4f lookAt(const Vector3f &origin,
                                      const Vector3f &target,
                                      const Vector3f &up);

/**
 * Creates an orthographic projection matrix.
 *
 * \param left
 *     The left border of the viewport.
 *
 * \param right
 *     The right border of the viewport.
 *
 * \param bottom
 *     The bottom border of the viewport.
 *
 * \param top
 *     The top border of the viewport.
 *
 * \param nearVal
 *     The near plane.
 *
 * \param farVal
 *     The far plane.
 */
extern NANOGUI_EXPORT Matrix4f ortho(float left, float right,
                                     float bottom, float top,
                                     float nearVal, float farVal);

/**
 * Creates a perspective projection matrix.
 *
 * \param left
 *     The left border of the viewport.
 *
 * \param right
 *     The right border of the viewport.
 *
 * \param bottom
 *     The bottom border of the viewport.
 *
 * \param top
 *     The top border of the viewport.
 *
 * \param nearVal
 *     The near plane.
 *
 * \param farVal
 *     The far plane.
 */
extern NANOGUI_EXPORT Matrix4f frustum(float left, float right,
                                       float bottom, float top,
                                       float nearVal, float farVal);
/**
 * \brief Construct homogeneous coordinate scaling matrix
 *
 * Returns a 3D homogeneous coordinate matrix that scales the X, Y, and Z
 * components with the corresponding entries of the 3D vector ``v``. The ``w``
 * component is left unchanged
 *
 * \param v
 *     The vector representing the scaling for each axis.
 */
extern NANOGUI_EXPORT Matrix4f scale(const Vector3f &v);

/**
 * \brief Construct homogeneous coordinate translation matrix
 *
 * Returns a 3D homogeneous coordinate matrix that translates the X, Y, and Z
 * components by the corresponding entries of the 3D vector ``v``. The ``w``
 * component is left unchanged
 *
 * \param v
 *     The vector representing the translation for each axis.
 */
extern NANOGUI_EXPORT Matrix4f translate(const Vector3f &v);

NAMESPACE_END(nanogui)
