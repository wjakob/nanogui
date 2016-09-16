/*
    src/glutil.cpp -- Convenience classes for accessing OpenGL >= 3.x

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/glutil.h>
#include <iostream>
#include <fstream>
#include <malloc.h>

NAMESPACE_BEGIN(nanogui)

static GLuint createShader_helper(GLint type, const std::string &name,
                                  const std::string &defines,
                                  std::string shader_string) {
    if (shader_string.empty())
        return (GLuint) 0;

    if (!defines.empty()) {
        if (shader_string.length() > 8 && shader_string.substr(0, 8) == "#version") {
            std::istringstream iss(shader_string);
            std::ostringstream oss;
            std::string line;
            std::getline(iss, line);
            oss << line << std::endl;
            oss << defines;
            while (std::getline(iss, line))
                oss << line << std::endl;
            shader_string = oss.str();
        } else {
            shader_string = defines + shader_string;
        }
    }

    GLuint id = glCreateShader(type);
    const char *shader_string_const = shader_string.c_str();
    glShaderSource(id, 1, &shader_string_const, nullptr);
    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        char buffer[512];
        std::cerr << "Error while compiling ";
        if (type == GL_VERTEX_SHADER)
            std::cerr << "vertex shader";
        else if (type == GL_FRAGMENT_SHADER)
            std::cerr << "fragment shader";
        else if (type == GL_GEOMETRY_SHADER)
            std::cerr << "geometry shader";
        std::cerr << " \"" << name << "\":" << std::endl;
        std::cerr << shader_string << std::endl << std::endl;
        glGetShaderInfoLog(id, 512, nullptr, buffer);
        std::cerr << "Error: " << std::endl << buffer << std::endl;
        throw std::runtime_error("Shader compilation failed!");
    }

    return id;
}

bool GLShader::initFromFiles(
    const std::string &name,
    const std::string &vertex_fname,
    const std::string &fragment_fname,
    const std::string &geometry_fname) {
    auto file_to_string = [](const std::string &filename) -> std::string {
        if (filename.empty())
            return "";
        std::ifstream t(filename);
        return std::string((std::istreambuf_iterator<char>(t)),
                           std::istreambuf_iterator<char>());
    };

    return init(name,
                file_to_string(vertex_fname),
                file_to_string(fragment_fname),
                file_to_string(geometry_fname));
}

bool GLShader::init(const std::string &name,
                    const std::string &vertex_str,
                    const std::string &fragment_str,
                    const std::string &geometry_str) {
    std::string defines;
    for (auto def : mDefinitions)
        defines += std::string("#define ") + def.first + std::string(" ") + def.second + "\n";

    glGenVertexArrays(1, &mVertexArrayObject);
    mName = name;
    mVertexShader =
        createShader_helper(GL_VERTEX_SHADER, name, defines, vertex_str);
    mGeometryShader =
        createShader_helper(GL_GEOMETRY_SHADER, name, defines, geometry_str);
    mFragmentShader =
        createShader_helper(GL_FRAGMENT_SHADER, name, defines, fragment_str);

    if (!mVertexShader || !mFragmentShader)
        return false;
    if (!geometry_str.empty() && !mGeometryShader)
        return false;

    mProgramShader = glCreateProgram();

    glAttachShader(mProgramShader, mVertexShader);
    glAttachShader(mProgramShader, mFragmentShader);

    if (mGeometryShader)
        glAttachShader(mProgramShader, mGeometryShader);

    glLinkProgram(mProgramShader);

    GLint status;
    glGetProgramiv(mProgramShader, GL_LINK_STATUS, &status);

    if (status != GL_TRUE) {
        char buffer[512];
        glGetProgramInfoLog(mProgramShader, 512, nullptr, buffer);
        std::cerr << "Linker error (" << mName << "): " << std::endl << buffer << std::endl;
        mProgramShader = 0;
        throw std::runtime_error("Shader linking failed!");
    }

    return true;
}

void GLShader::bind() {
    glUseProgram(mProgramShader);
    glBindVertexArray(mVertexArrayObject);
}

GLint GLShader::attrib(const std::string &name, bool warn) const {
    GLint id = glGetAttribLocation(mProgramShader, name.c_str());
    if (id == -1 && warn)
        std::cerr << mName << ": warning: did not find attrib " << name << std::endl;
    return id;
}

void GLShader::setUniform(const std::string &name, const GLUniformBuffer &buf, bool warn) {
    GLuint blockIndex = glGetUniformBlockIndex(mProgramShader, name.c_str());
    if (blockIndex == GL_INVALID_INDEX) {
        if (warn)
            std::cerr << mName << ": warning: did not find uniform buffer " << name << std::endl;
        return;
    }
    glUniformBlockBinding(mProgramShader, blockIndex, buf.getBindingPoint());
}

GLint GLShader::uniform(const std::string &name, bool warn) const {
    GLint id = glGetUniformLocation(mProgramShader, name.c_str());
    if (id == -1 && warn)
        std::cerr << mName << ": warning: did not find uniform " << name << std::endl;
    return id;
}

void GLShader::uploadAttrib(const std::string &name, uint32_t size, int dim,
                            uint32_t compSize, GLuint glType, bool integral,
                            const uint8_t *data, int version) {
    int attribID = 0;
    if (name != "indices") {
        attribID = attrib(name);
        if (attribID < 0)
            return;
    }

    GLuint bufferID;
    auto it = mBufferObjects.find(name);
    if (it != mBufferObjects.end()) {
        Buffer &buffer = it->second;
        bufferID = it->second.id;
        buffer.version = version;
        buffer.size = size;
        buffer.compSize = compSize;
    } else {
        glGenBuffers(1, &bufferID);
        Buffer buffer;
        buffer.id = bufferID;
        buffer.glType = glType;
        buffer.dim = dim;
        buffer.compSize = compSize;
        buffer.size = size;
        buffer.version = version;
        mBufferObjects[name] = buffer;
    }
    size_t totalSize = (size_t) size * (size_t) compSize;

    if (name == "indices") {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalSize, data, GL_DYNAMIC_DRAW);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ARRAY_BUFFER, totalSize, data, GL_DYNAMIC_DRAW);
        if (size == 0) {
            glDisableVertexAttribArray(attribID);
        } else {
            glEnableVertexAttribArray(attribID);
            glVertexAttribPointer(attribID, dim, glType, integral, 0, 0);
        }
    }
}

void GLShader::downloadAttrib(const std::string &name, uint32_t size, int /* dim */,
                             uint32_t compSize, GLuint /* glType */, uint8_t *data) {
    auto it = mBufferObjects.find(name);
    if (it == mBufferObjects.end())
        throw std::runtime_error("downloadAttrib(" + mName + ", " + name + ") : buffer not found!");

    const Buffer &buf = it->second;
    if (buf.size != size || buf.compSize != compSize)
        throw std::runtime_error(mName + ": downloadAttrib: size mismatch!");

    size_t totalSize = (size_t) size * (size_t) compSize;

    if (name == "indices") {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.id);
        glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, totalSize, data);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, buf.id);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, totalSize, data);
    }
}

void GLShader::shareAttrib(const GLShader &otherShader, const std::string &name, const std::string &_as) {
    std::string as = _as.length() == 0 ? name : _as;
    auto it = otherShader.mBufferObjects.find(name);
    if (it == otherShader.mBufferObjects.end())
        throw std::runtime_error("shareAttribute(" + otherShader.mName + ", " + name + "): attribute not found!");
    const Buffer &buffer = it->second;

    if (name != "indices") {
        int attribID = attrib(as);
        if (attribID < 0)
            return;
        glEnableVertexAttribArray(attribID);
        glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
        glVertexAttribPointer(attribID, buffer.dim, buffer.glType, buffer.compSize == 1 ? GL_TRUE : GL_FALSE, 0, 0);
    } else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.id);
    }
}

void GLShader::invalidateAttribs() {
    for (auto &buffer : mBufferObjects)
        buffer.second.version = -1;
}

void GLShader::freeAttrib(const std::string &name) {
    auto it = mBufferObjects.find(name);
    if (it != mBufferObjects.end()) {
        glDeleteBuffers(1, &it->second.id);
        mBufferObjects.erase(it);
    }
}

void GLShader::drawIndexed(int type, uint32_t offset_, uint32_t count_) {
    if (count_ == 0)
        return;
    size_t offset = offset_;
    size_t count = count_;

    switch (type) {
        case GL_TRIANGLES: offset *= 3; count *= 3; break;
        case GL_LINES: offset *= 2; count *= 2; break;
    }

    glDrawElements(type, (GLsizei) count, GL_UNSIGNED_INT,
                   (const void *)(offset * sizeof(uint32_t)));
}

void GLShader::drawArray(int type, uint32_t offset, uint32_t count) {
    if (count == 0)
        return;

    glDrawArrays(type, offset, count);
}

void GLShader::free() {
    for (auto &buf: mBufferObjects)
        glDeleteBuffers(1, &buf.second.id);

    if (mVertexArrayObject)
        glDeleteVertexArrays(1, &mVertexArrayObject);

    glDeleteProgram(mProgramShader); mProgramShader = 0;
    glDeleteShader(mVertexShader);   mVertexShader = 0;
    glDeleteShader(mFragmentShader); mFragmentShader = 0;
    glDeleteShader(mGeometryShader); mGeometryShader = 0;
}

//  ----------------------------------------------------

void GLUniformBuffer::init() {
    glGenBuffers(1, &mID);
}

void GLUniformBuffer::bind(int bindingPoint) {
    mBindingPoint = bindingPoint;
    glBindBufferBase(GL_UNIFORM_BUFFER, mBindingPoint, mID);
}

void GLUniformBuffer::release() {
    glBindBufferBase(GL_UNIFORM_BUFFER, mBindingPoint, 0);
}

void GLUniformBuffer::free() {
    glDeleteBuffers(1, &mID);
}

void GLUniformBuffer::update(const std::vector<uint8_t> &data) {
    glBindBuffer(GL_UNIFORM_BUFFER, mID);
    glBufferData(GL_UNIFORM_BUFFER, data.size(), data.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//  ----------------------------------------------------

void GLFramebuffer::init(const Vector2i &size, int nSamples) {
    mSize = size;
    mSamples = nSamples;

    glGenRenderbuffers(1, &mColor);
    glBindRenderbuffer(GL_RENDERBUFFER, mColor);

    if (nSamples <= 1)
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, size.x(), size.y());
    else
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, nSamples, GL_RGBA8, size.x(), size.y());

    glGenRenderbuffers(1, &mDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepth);

    if (nSamples <= 1)
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x(), size.y());
    else
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, nSamples, GL_DEPTH24_STENCIL8, size.x(), size.y());

    glGenFramebuffers(1, &mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mColor);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepth);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepth);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Could not create framebuffer object!");

    release();
}

void GLFramebuffer::free() {
    glDeleteRenderbuffers(1, &mColor);
    glDeleteRenderbuffers(1, &mDepth);
}

void GLFramebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    if (mSamples > 1)
        glEnable(GL_MULTISAMPLE);
}

void GLFramebuffer::release() {
    if (mSamples > 1)
        glDisable(GL_MULTISAMPLE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::blit() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    glBlitFramebuffer(0, 0, mSize.x(), mSize.y(), 0, 0, mSize.x(), mSize.y(),
                      GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::downloadTGA(const std::string &filename) {
    uint8_t *temp = new uint8_t[mSize.prod() * 4];

    std::cout << "Writing \"" << filename  << "\" (" << mSize.x() << "x" << mSize.y() << ") .. ";
    std::cout.flush();
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebuffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glReadPixels(0, 0, mSize.x(), mSize.y(), GL_BGRA, GL_UNSIGNED_BYTE, temp);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    uint32_t rowSize = mSize.x() * 4;
    uint32_t halfHeight = mSize.y() / 2;
    uint8_t *line = (uint8_t *) alloca(rowSize);
    for (uint32_t i=0, j=mSize.y()-1; i<halfHeight; ++i) {
        memcpy(line, temp + i * rowSize, rowSize);
        memcpy(temp + i * rowSize, temp + j * rowSize, rowSize);
        memcpy(temp + j * rowSize, line, rowSize);
        j--;
    }

    FILE *tga = fopen(filename.c_str(), "wb");
    if (tga == nullptr)
        throw std::runtime_error("GLFramebuffer::downloadTGA(): Could not open output file");
    fputc(0, tga); /* ID */
    fputc(0, tga); /* Color map */
    fputc(2, tga); /* Image type */
    fputc(0, tga); fputc(0, tga); /* First entry of color map (unused) */
    fputc(0, tga); fputc(0, tga); /* Length of color map (unused) */
    fputc(0, tga); /* Color map entry size (unused) */
    fputc(0, tga); fputc(0, tga);  /* X offset */
    fputc(0, tga); fputc(0, tga);  /* Y offset */
    fputc(mSize.x() % 256, tga); /* Width */
    fputc(mSize.x() / 256, tga); /* continued */
    fputc(mSize.y() % 256, tga); /* Height */
    fputc(mSize.y() / 256, tga); /* continued */
    fputc(32, tga);   /* Bits per pixel */
    fputc(0x20, tga); /* Scan from top left */
    fwrite(temp, mSize.prod() * 4, 1, tga);
    fclose(tga);

    delete[] temp;
    std::cout << "done." << std::endl;
}

//  ----------------------------------------------------

Eigen::Vector3f project(const Eigen::Vector3f &obj,
                        const Eigen::Matrix4f &model,
                        const Eigen::Matrix4f &proj,
                        const Vector2i &viewportSize) {
    Eigen::Vector4f tmp;
    tmp << obj, 1;

    tmp = model * tmp;

    tmp = proj * tmp;

    tmp = tmp.array() / tmp(3);
    tmp = tmp.array() * 0.5f + 0.5f;
    tmp(0) = tmp(0) * viewportSize.x();
    tmp(1) = tmp(1) * viewportSize.y();

    return tmp.head(3);
}

Eigen::Vector3f unproject(const Eigen::Vector3f &win,
                          const Eigen::Matrix4f &model,
                          const Eigen::Matrix4f &proj,
                          const Vector2i &viewportSize) {
    Eigen::Matrix4f Inverse = (proj * model).inverse();

    Eigen::Vector4f tmp;
    tmp << win, 1;
    tmp(0) = tmp(0) / viewportSize.x();
    tmp(1) = tmp(1) / viewportSize.y();
    tmp = tmp.array() * 2.0f - 1.0f;

    Eigen::Vector4f obj = Inverse * tmp;
    obj /= obj(3);

    return obj.head(3);
}

Eigen::Matrix4f lookAt(const Eigen::Vector3f &eye,
                       const Eigen::Vector3f &center,
                       const Eigen::Vector3f &up) {
    Eigen::Vector3f f = (center - eye).normalized();
    Eigen::Vector3f s = f.cross(up).normalized();
    Eigen::Vector3f u = s.cross(f);

    Eigen::Matrix4f Result = Eigen::Matrix4f::Identity();
    Result(0, 0) = s(0);
    Result(0, 1) = s(1);
    Result(0, 2) = s(2);
    Result(1, 0) = u(0);
    Result(1, 1) = u(1);
    Result(1, 2) = u(2);
    Result(2, 0) = -f(0);
    Result(2, 1) = -f(1);
    Result(2, 2) = -f(2);
    Result(0, 3) = -s.transpose() * eye;
    Result(1, 3) = -u.transpose() * eye;
    Result(2, 3) = f.transpose() * eye;
    return Result;
}

Eigen::Matrix4f ortho(const float left, const float right, const float bottom,
                      const float top, const float zNear, const float zFar) {
    Eigen::Matrix4f Result = Eigen::Matrix4f::Identity();
    Result(0, 0) = 2.0f / (right - left);
    Result(1, 1) = 2.0f / (top - bottom);
    Result(2, 2) = -2.0f / (zFar - zNear);
    Result(0, 3) = -(right + left) / (right - left);
    Result(1, 3) = -(top + bottom) / (top - bottom);
    Result(2, 3) = -(zFar + zNear) / (zFar - zNear);
    return Result;
}

Eigen::Matrix4f frustum(const float left, const float right, const float bottom,
                        const float top, const float nearVal,
                        const float farVal) {
    Eigen::Matrix4f Result = Eigen::Matrix4f::Zero();
    Result(0, 0) = (2.0f * nearVal) / (right - left);
    Result(1, 1) = (2.0f * nearVal) / (top - bottom);
    Result(0, 2) = (right + left) / (right - left);
    Result(1, 2) = (top + bottom) / (top - bottom);
    Result(2, 2) = -(farVal + nearVal) / (farVal - nearVal);
    Result(3, 2) = -1.0f;
    Result(2, 3) = -(2.0f * farVal * nearVal) / (farVal - nearVal);
    return Result;
}

Eigen::Matrix4f scale(const Eigen::Matrix4f &m, const Eigen::Vector3f &v) {
    Eigen::Matrix4f Result;
    Result.col(0) = m.col(0).array() * v(0);
    Result.col(1) = m.col(1).array() * v(1);
    Result.col(2) = m.col(2).array() * v(2);
    Result.col(3) = m.col(3);
    return Result;
}

Eigen::Matrix4f translate(const Eigen::Matrix4f &m, const Eigen::Vector3f &v) {
    Eigen::Matrix4f Result = m;
    Result.col(3) = m.col(0).array() * v(0) + m.col(1).array() * v(1) +
                    m.col(2).array() * v(2) + m.col(3).array();
    return Result;
}

NAMESPACE_END(nanogui)
