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
#if defined(WIN32)
#include <windows.h>
#endif
#include <nanogui/glutil.h>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

class ExampleApplication : public nanogui::Screen {
public:
    ExampleApplication() : nanogui::Screen(Eigen::Vector2i(800, 600), "NanoGUI Test") {
        using namespace nanogui;

        Window *window = new Window(this, "Button demo");
        window->setPosition(Vector2i(15, 15));
        window->setLayout(new GroupLayout());

        /* No need to store a pointer, the data structure will be automatically
           freed when the parent window is deleted */
        new Label(window, "Push buttons", "sans-bold");

        Button *b = new Button(window, "Plain button");
        b->setCallback([] { cout << "pushed!" << endl; });
        b = new Button(window, "Styled", ENTYPO_ICON_ROCKET);
        b->setBackgroundColor(Color(0, 0, 255, 255));
        b->setCallback([] { cout << "pushed!" << endl; });

        new Label(window, "Toggle buttons", "sans-bold");
        b = new Button(window, "Toggle me");
        b->setButtonFlags(Button::ToggleButton);
        b->setChangeCallback([](bool state) { cout << "Toggle button state: " << state << endl; });

        new Label(window, "Radio buttons", "sans-bold");
        b = new Button(window, "Radio button 1");
        b->setButtonFlags(Button::RadioButton);
        b = new Button(window, "Radio button 2");
        b->setButtonFlags(Button::RadioButton);

        new Label(window, "A tool palette", "sans-bold");
        Widget *tools = new Widget(window);
        tools->setLayout(new BoxLayout(BoxLayout::Horizontal, BoxLayout::Middle, 0, 6));

        b = new ToolButton(tools, ENTYPO_ICON_CLOUD);
        b = new ToolButton(tools, ENTYPO_ICON_FF);
        b = new ToolButton(tools, ENTYPO_ICON_COMPASS);
        b = new ToolButton(tools, ENTYPO_ICON_INSTALL);

        new Label(window, "Popup buttons", "sans-bold");
        PopupButton *popupBtn = new PopupButton(window, "Popup", ENTYPO_ICON_EXPORT);
        Popup *popup = popupBtn->popup();
        popup->setLayout(new GroupLayout());
        new Label(popup, "Arbitrary widgets can be placed here");
        new CheckBox(popup, "A check box");
        popupBtn = new PopupButton(popup, "Recursive popup", ENTYPO_ICON_FLASH);
        popup = popupBtn->popup();
        popup->setLayout(new GroupLayout());
        new CheckBox(popup, "Another check box");

        window = new Window(this, "Basic widgets");
        window->setPosition(Vector2i(200, 15));
        window->setLayout(new GroupLayout());

        new Label(window, "Message dialog", "sans-bold");
        tools = new Widget(window);
        tools->setLayout(new BoxLayout(BoxLayout::Horizontal, BoxLayout::Middle, 0, 6));
        b = new Button(tools, "Info");
        b->setCallback([&] {
            auto dlg = new MessageDialog(this, MessageDialog::Information, "Title", "This is an information message");
            dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
        });
        b = new Button(tools, "Warn");
        b->setCallback([&] {
            auto dlg = new MessageDialog(this, MessageDialog::Warning, "Title", "This is a warning message");
            dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
        });
        b = new Button(tools, "Ask");
        b->setCallback([&] {
            auto dlg = new MessageDialog(this, MessageDialog::Warning, "Title", "This is a question message", "Yes", "No", true);
            dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
        });

        std::vector<std::pair<int, std::string>>
            icons = loadImageDirectory(mNVGContext, "icons");

        new Label(window, "Image panel & scroll panel", "sans-bold");
        PopupButton *imagePanelBtn = new PopupButton(window, "Image Panel");
        imagePanelBtn->setIcon(ENTYPO_ICON_FOLDER);
        popup = imagePanelBtn->popup();
        VScrollPanel *vscroll = new VScrollPanel(popup);
        ImagePanel *imgPanel = new ImagePanel(vscroll);
        imgPanel->setImageData(icons);
        popup->setFixedSize(Vector2i(245, 150));
        new Label(window, "Selected image", "sans-bold");
        auto img = new ImageView(window);
        img->setFixedSize(Vector2i(40, 40));

        new Label(window, "File dialog", "sans-bold");
        tools = new Widget(window);
        tools->setLayout(new BoxLayout(BoxLayout::Horizontal, BoxLayout::Middle, 0, 6));
        b = new Button(tools, "Open");
        b->setCallback([&] {
            cout << "File dialog result: " << file_dialog(
                    { {"png", "Portable Network Graphics"}, {"txt", "Text file"} }, false) << endl;
        });
        b = new Button(tools, "Save");
        b->setCallback([&] {
            cout << "File dialog result: " << file_dialog(
                    { {"png", "Portable Network Graphics"}, {"txt", "Text file"} }, true) << endl;
        });

        img->setImage(icons[0].first);
        imgPanel->setCallback([&, img, imgPanel, imagePanelBtn](int i) {
            img->setImage(imgPanel->images()[i].first); cout << "Selected item " << i << endl;
        });

        new Label(window, "Combo box", "sans-bold");
        new ComboBox(window, { "Combo box item 1", "Combo box item 2", "Combo box item 3"});
        new Label(window, "Check box", "sans-bold");
        CheckBox *cb = new CheckBox(window, "Flag 1",
            [](bool state) { cout << "Check box 1 state: " << state << endl; }
        );
        cb->setChecked(true);
        new CheckBox(window, "Flag 2",
            [](bool state) { cout << "Check box 2 state: " << state << endl; }
        );
        new Label(window, "Progress bar", "sans-bold");
        mProgress = new ProgressBar(window);

        new Label(window, "Slider and text box", "sans-bold");

        Widget *panel = new Widget(window);
        panel->setLayout(
            new BoxLayout(BoxLayout::Horizontal, BoxLayout::Middle, 0, 20));

        Slider *slider = new Slider(panel);
        slider->setValue(0.5f);
        slider->setFixedWidth(80);

        TextBox *textBox = new TextBox(panel);
        textBox->setFixedSize(Vector2i(60, 25));
        textBox->setValue("50");
        textBox->setUnits("%");
        slider->setCallback([textBox](float value) {
            textBox->setValue(std::to_string((int) (value * 100)));
        });
        slider->setFinalCallback([&](float value) {
            cout << "Final slider value: " << (int) (value * 100) << endl;
        });

        window = new Window(this, "Misc. widgets");
        window->setPosition(Vector2i(425, 15));
        window->setLayout(new GroupLayout());
        new Label(window, "Color wheel", "sans-bold");
        new ColorWheel(window);


        performLayout(mNVGContext);

        /* All NanoGUI widgets are initialized at this point. Now
           create an OpenGL shader to draw the main window contents.

           NanoGUI comes with a simple Eigen-based wrapper around OpenGL 3,
           which eliminates most of the tedious and error-prone shader and
           buffer object management.
        */

        mShader.init(
            /* An identifying name */
            "a_simple_shader",

            /* Vertex shader */
            "#version 330\n"
            "uniform mat4 modelViewProj;\n"
            "in vec3 position;\n"
            "void main() {\n"
            "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
            "}",

            /* Fragment shader */
            "#version 330\n"
            "out vec4 color;\n"
            "uniform float intensity;\n"
            "void main() {\n"
            "    color = vec4(vec3(intensity), 1.0);\n"
            "}"
        );

        MatrixXu indices(3, 2); /* Draw 2 triangles */
        indices.col(0) << 0, 1, 2;
        indices.col(1) << 2, 3, 0;

        MatrixXf positions(3, 4);
        positions.col(0) << -1, -1, 0;
        positions.col(1) <<  1, -1, 0;
        positions.col(2) <<  1,  1, 0;
        positions.col(3) << -1,  1, 0;

        mShader.bind();
        mShader.uploadIndices(indices);
        mShader.uploadAttrib("position", positions);
        mShader.setUniform("intensity", 0.5f);
    }

    ~ExampleApplication() {
        mShader.free();
    }

    virtual void draw(NVGcontext *ctx) {
        /* Animate the scrollbar */
        mProgress->setValue(std::fmod((float) glfwGetTime() / 10, 1.0f));

        /* Draw the user interface */
        Screen::draw(ctx);
    }

    virtual void drawContents() {
        using namespace nanogui;

        /* Draw the window contents using OpenGL */
        mShader.bind();

        Matrix4f mvp;
        mvp.setIdentity();
        mvp.topLeftCorner<3,3>() = Matrix3f(Eigen::AngleAxisf((float) glfwGetTime(),  Vector3f::UnitZ())) * 0.25f;

        mvp.row(0) *= (float) mSize.y() / (float) mSize.x();

        mShader.setUniform("modelViewProj", mvp);

        /* Draw 2 triangles starting at index 0 */
        mShader.drawIndexed(GL_TRIANGLES, 0, 2);
    }
private:
    nanogui::ProgressBar *mProgress;
    nanogui::GLShader mShader;
};

int main(int argc, char *argv[]) {
    try {
        nanogui::init();

        ExampleApplication *app = new ExampleApplication();
        app->drawAll();
        app->setVisible(true);

        nanogui::mainloop();

        delete app;

        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }

    return 0;
}
