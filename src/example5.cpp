/*
    src/example5.cpp -- example of how to use the same serialzer to save the
    state of many different widgets using the `push` and `pop` feature of
    the Serializer class to create "namespaces" for each widget being saved.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

// this define just makes it so when we include the example1.cpp file,
// the `main` method is not included
// tip: including source files is BAD PRACTICE!  this was done out of laziness
#define NANOGUI_EXAMPLE_5_BYPASS
#include "example1.cpp"

#include <nanogui/serializer/core.h>
#include <nanogui/serializer/opengl.h> // unused in this example
#include <nanogui/serializer/sparse.h> // unused in this example

#include <map>

/*
 * NOTE: not everything appears to get saved / restored.  The image selected,
 *       what the combobox has selected, etc.  Feel free to investigate and
 *       submit a PR if those clases are not serializing fully.  The original
 *       author of this file is not willing to indulge anymore.
 */

// ExampleApplication is what is defined in example 1
class ExtendedExampleApplication : public ExampleApplication {
public:
    ExtendedExampleApplication() : ExampleApplication() {
        // At this point, all of the widgets we are going to save / load are
        // now created.  This is pretty silly, but since Example 1 uses many
        // of the possible widgets, it's also a good test.
        //
        // In your own code, you'll likely only need to serialize one or two
        // widgets or a shader.  In this code, we're going to serialize every
        // child widget, as well as the shader.

        // first add all direct descendents of this ExtendedExampleApplication
        std::vector<nanogui::Widget*> all_kids;
        for (auto *c : mChildren) {
            all_kids.emplace_back(c);
        }

        unsigned idx = 0;
        while(all_kids.size() > 0) {
            // grab the next widget
            auto *c = all_kids.back();
            all_kids.pop_back();

            // add it to the map
            auto name = mSerialFilename + std::string("_child_") + std::to_string(idx++);
            mSerialMap[name] = c;

            // add all children of this one to the list of all_kids
            for (auto *child : c->children()) {
                all_kids.emplace_back(child);
            }
        }

        auto window = new nanogui::Window(this, "Serialization");
        window->setLayout(new nanogui::GroupLayout());

        // example1 is 1024x768
        window->setPosition({600, 600});

        auto b = new nanogui::Button(window, "Save Example 5 state now?");
        b->setCallback([&]() {
            nanogui::Serializer serializer(mSerialFilename, true);
            for (const auto& x : mSerialMap) {
                // x.first is the string name
                serializer.push(x.first);
                // x.second is the widget pointer
                x.second->save(serializer);
                serializer.pop();
            }

            // the shader MVP is set in the draw loop using the time
            // but in real code you would `mShader.downloadAttrib` to
            // actually capture the state of something
            //
            // basically, if there is no `save` method, push a name onto
            // the serializer, and explicitly set things.  See the Serializer
            // docs, set and get are template functions.
            serializer.push("the_shader_time");
            serializer.set<float>("glfwTime", (float) glfwGetTime());
            serializer.pop();

            atLeastOneSave = true;
            new nanogui::MessageDialog(
                this, nanogui::MessageDialog::Type::Information, "Saved!",
                "Go make some changes e.g. check boxes or change colors.  Then load."
            );

        });

        b = new nanogui::Button(window, "Restore previous Example 5 state now?");
        b->setCallback([&]() {
            if (!atLeastOneSave) {
                new nanogui::MessageDialog(
                    this, nanogui::MessageDialog::Type::Information, "Oops!",
                    "You have to save the state before it can be restored ;)"
                );
            }
            else {
                nanogui::Serializer serializer(mSerialFilename, false);
                for (const auto& x : mSerialMap) {
                    // x.first is the string name
                    serializer.push(x.first);
                    // x.second is the widget pointer
                    x.second->load(serializer);
                    serializer.pop();
                }

                // reset the time to alter the MVP of mShader based on what
                // was stored previously
                serializer.push("the_shader_time");
                float t = 0.0f;
                serializer.get<float>("glfwTime", t);
                glfwSetTime(t);
                serializer.pop();
            }
        });

        /* NOTE: none of these are used explicitly in the drawing code, this is
         * an internal test to ensure that the various kinds of uploads are
         * working in GLShader, as they rely on some of the serialization types.
         */
        mSerialShader.init(
            /* An identifying name */
            "fake_shader",

            /* Vertex shader */
            "#version 330\n"

            "uniform uint uint32_t_test    = 0;\n"
            "uniform int int32_t_test      = 0;\n"
            "uniform uint uint16_t_test    = 0;\n"
            "uniform int int16_t_test      = 0;\n"
            "uniform uint uint8_t_test     = 0;\n"
            "uniform int int8_t_test       = 0;\n"
            "uniform float floating64_test = 0;\n"
            "uniform float floating32_test = 0;\n"
            "uniform float half_test       = 0;\n"

            // NOTE: adding all the `if` to force glsl not to optimize out!
            "void main() {\n"
            "    if (uint32_t_test > uint(1))\n"
            "        gl_Position = vec4(0, 0, 0, 1);\n"
            "    else if (int32_t_test > int(1))\n"
            "        gl_Position = vec4(0.1, 0.1, 0.1, 1);\n"
            "    else if (uint16_t_test > uint(1))\n"
            "        gl_Position = vec4(0.2, 0.2, 0.2, 1);\n"
            "    else if (int16_t_test > int(1))\n"
            "        gl_Position = vec4(0.3, 0.3, 0.3, 1);\n"
            "    else if (uint8_t_test > uint(1))\n"
            "        gl_Position = vec4(0.4, 0.4, 0.4, 1);\n"
            "    else if (int8_t_test > int(1))\n"
            "        gl_Position = vec4(0.5, 0.5, 0.5, 1);\n"
            "    else if (floating64_test > float(1))\n"
            "        gl_Position = vec4(0.6, 0.6, 0.6, 1);\n"
            "    else if (floating32_test > float(1))\n"
            "        gl_Position = vec4(0.7, 0.7, 0.7, 1);\n"
            "    else if (half_test > float(1))\n"
            "        gl_Position = vec4(0.8, 0.8, 0.8, 1);\n"
            "    else\n"
            "        gl_Position = vec4(0);\n"
            "}",

            /* Fragment shader */
            "#version 330\n"
            "out vec4 color;\n"
            "void main() {\n"
            "    color = vec4(0.0, 0.0, 0.0, 1.0);\n"
            "}"
        );

        mSerialShader.bind();
        // upload uniform, downloading is non-trivial and so is avoided.
        // this tests the internal serialization structs just by compiling
        mSerialShader.setUniform<uint32_t>("uint32_t_test", (uint32_t) 2);
        mSerialShader.setUniform<int32_t>("int32_t_test",   (int32_t) -2);
        mSerialShader.setUniform<uint16_t>("uint16_t_test", (uint16_t) 3);
        mSerialShader.setUniform<int16_t>("int16_t_test",   (int16_t) -3);
        mSerialShader.setUniform<uint8_t>("uint8_t_test",    (uint8_t) 4);
        mSerialShader.setUniform<int8_t>("int8_t_test",      (int8_t) -4);
        mSerialShader.setUniform<double>("floating64_test",   (double) 5);
        mSerialShader.setUniform<float>("floating32_test",     (float) 6);
        // vvv does not compile, incomplete type
        // mSerialShader.setUniform<half_float::half>("half_test",  7);

        this->performLayout();
    }

    ~ExtendedExampleApplication() { mSerialShader.free(); }

protected:
    // Map of names to widgets.  We're going to save each widget in their own
    // "namespace" in the serializer so that everything is saved rather than
    // potentially overwriting some things
    std::map<std::string, nanogui::Widget*> mSerialMap;

    // this example will continuously overwrite the same file, in your own code
    // you'll probably want to create new files or something if you want to
    // be able to save and load multiple different snapshots
    std::string mSerialFilename = "nanogui_example_5_serialized";

    // since we're using the same file each time, make sure that we've saved
    // at least once before loading
    bool atLeastOneSave = false;

    // serialization shader test, does nothing
    nanogui::GLShader mSerialShader;
};

// same as example1 loop, only uses `ExtendedExampleApplication` instead of
// the original `ExampleApplication`.
int main(int /* argc */, char ** /* argv */) {
    try {
        nanogui::init();

        /* scoped variables */ {
            nanogui::ref<ExtendedExampleApplication> app = new ExtendedExampleApplication();
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
