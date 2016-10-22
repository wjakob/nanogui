/*
    src/example1.cpp -- C++ version of an example application that shows
    how to use the various widget classes. For a Python implementation, see
    '../python/example1.py'.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
#include <nanogui/glcanvas.h>
#include <iostream>
#include <string>

// Includes for the GLTexture class.
#include <cstdint>
#include <memory>
#include <utility>

#if defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#if defined(_WIN32)
#  pragma warning(push)
#  pragma warning(disable: 4457 4456 4005 4312)
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#if defined(_WIN32)
#  pragma warning(pop)
#endif
#if defined(_WIN32)
#  if defined(APIENTRY)
#    undef APIENTRY
#  endif
#  include <windows.h>
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::to_string;


class ExampleApplication : public nanogui::Screen {
public:
    ExampleApplication() : nanogui::Screen(Eigen::Vector2i(800, 600), "NanoGUI Test", false) {
        using namespace nanogui;

        Window *window = new Window(this, "GLCanvas Demo");
        window->setPosition(Vector2i(15, 15));
        window->setLayout(new GroupLayout());

        mCanvas = new GLCanvas(window);
        mCanvas->setBackgroundColor({100, 100, 100, 255});
	mCanvas->setSize({400, 400});

	mRotation = Vector3f(0.25, 0.5, 0.33);

        mCanvas->setGLDrawingCallback([this]() {
            mShader.bind();

            Matrix4f mvp;
            mvp.setIdentity();
            float fTime = (float)glfwGetTime();
            mvp.topLeftCorner<3,3>() = Eigen::Matrix3f(Eigen::AngleAxisf(mRotation[0]*fTime, Vector3f::UnitX()) *
                                                       Eigen::AngleAxisf(mRotation[1]*fTime,  Vector3f::UnitY()) *
                                                       Eigen::AngleAxisf(mRotation[2]*fTime, Vector3f::UnitZ())) * 0.25f;

            mShader.setUniform("modelViewProj", mvp);
            glFrontFace(GL_CW);

            /* Draw 12 triangles starting at index 0 */
            mShader.drawIndexed(GL_TRIANGLES, 0, 12);
          });

        Widget *tools = new Widget(window);
        tools->setLayout(new BoxLayout(Orientation::Horizontal,
                                       Alignment::Middle, 0, 5));

        Button *b0 = new Button(tools, "Random Color");
	b0->setCallback([this]() { mCanvas->setBackgroundColor(Vector4i(rand() % 256, rand() % 256, rand() % 256, 255)); });

        Button *b1 = new Button(tools, "Random Rotation");
	b1->setCallback([this]() { mRotation = Vector3f((rand() % 100) / 100.0, (rand() % 100) / 100.0, (rand() % 100) / 100.0); });

        performLayout();

        mShader.init(
            /* An identifying name */
            "a_simple_shader",

            /* Vertex shader */
            "#version 330\n"
            "uniform mat4 modelViewProj;\n"
            "in vec3 position;\n"
            "in vec3 normal;\n"
            "out vec4 frag_normal;\n"
            "void main() {\n"
            "    frag_normal = modelViewProj * vec4(normal, 1.0);\n"
            "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
            "}",

            /* Fragment shader */
            "#version 330\n"
            "out vec4 color;\n"
            "in vec4 frag_normal;\n"
            "uniform float intensity;\n"
            "struct SimpleDirectionalLight {\n"
            "vec4 color;\n"
            "vec3 direction;\n"
            "float amb_intensity;\n"
            "};\n"
            "uniform SimpleDirectionalLight light;\n"
            "void main() {\n"
            "    float diffuse_intensity = max(0.0, dot(normalize(frag_normal), normalize(vec4(-light.direction, 0.0))));\n"
            "    color = intensity * light.color * (light.amb_intensity + diffuse_intensity);\n"
            //"    color = vec4(vec3(intensity), 1.0);\n"
            "}"
        );

        MatrixXu indices(3, 12); /* Draw a cube */
        indices.col( 0) << 0, 1, 3;
        indices.col( 1) << 2, 3, 1;
        indices.col( 2) << 3, 2, 6;
        indices.col( 3) << 6, 7, 3;
        indices.col( 4) << 7, 6, 5;
        indices.col( 5) << 5, 4, 7;
        indices.col( 6) << 4, 5, 1;
        indices.col( 7) << 1, 0, 4;
        indices.col( 8) << 4, 0, 3;
        indices.col( 9) << 3, 7, 4;
        indices.col(10) << 5, 6, 2;
        indices.col(11) << 2, 1, 5;

        MatrixXf normals(3, 12);
        normals.col( 0) <<  0,  1,  0;
        normals.col( 1) <<  0,  1,  0;
        normals.col( 2) <<  1,  0,  0;
        normals.col( 3) <<  1,  0,  0;
        normals.col( 4) <<  0, -1,  0;
        normals.col( 5) <<  0, -1,  0;
        normals.col( 6) << -1,  0,  0;
        normals.col( 7) << -1,  0,  0;
        normals.col( 8) <<  0,  0,  1;
        normals.col( 9) <<  0,  0,  1;
        normals.col(10) <<  0,  0, -1;
        normals.col(11) <<  0,  0, -1;

        MatrixXf positions(3, 8);
        positions.col(0) << -1,  1,  1;
        positions.col(1) << -1,  1, -1;
        positions.col(2) <<  1,  1, -1;
        positions.col(3) <<  1,  1,  1;
        positions.col(4) << -1, -1,  1;
        positions.col(5) << -1, -1, -1;
        positions.col(6) <<  1, -1, -1;
        positions.col(7) <<  1, -1,  1;

        mShader.bind();
        mShader.uploadIndices(indices);

        mShader.uploadAttrib("position", positions);
        mShader.uploadAttrib("normal", normals);
        mShader.setUniform("intensity", 1.0);
        mShader.setUniform("light.color", Vector4f(1.0, 1.0, 0.0, 1.0));
        mShader.setUniform("light.direction", Vector3f(0, 0, -1.0));
        mShader.setUniform("light.amb_intensity", 0.25);
    }

    ~ExampleApplication() {
      mShader.free();
    }

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            setVisible(false);
            return true;
        }
        return false;
    }

    virtual void draw(NVGcontext *ctx) {
        /* Draw the user interface */
        Screen::draw(ctx);
    }

    virtual void drawContents() {
    }
private:
    nanogui::GLCanvas *mCanvas;
    nanogui::GLShader mShader;
    Eigen::Vector3f mRotation;
};

int main(int /* argc */, char ** /* argv */) {
    try {
        nanogui::init();

        {
            nanogui::ref<ExampleApplication> app = new ExampleApplication();
            app->drawAll();
            app->setVisible(true);
            nanogui::mainloop();
        }

        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }

    return 0;
}
