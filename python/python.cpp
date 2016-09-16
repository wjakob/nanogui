#ifdef NANOGUI_PYTHON

#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "python.h"

#if defined(__APPLE__) || defined(__linux__)
#  include <coro.h>
#endif

using namespace nanogui;
namespace py = pybind11;

typedef FloatBox<double> DoubleBox;
typedef IntBox<int64_t> Int64Box;

DECLARE_LAYOUT(Layout);
DECLARE_LAYOUT(GroupLayout);
DECLARE_LAYOUT(BoxLayout);
DECLARE_LAYOUT(GridLayout);
DECLARE_LAYOUT(AdvancedGridLayout);
DECLARE_SCREEN(Screen);
DECLARE_WIDGET(Widget);
DECLARE_WIDGET(Window);
DECLARE_WIDGET(Label);
DECLARE_WIDGET(Button);
DECLARE_WIDGET(ToolButton);
DECLARE_WIDGET(Popup);
DECLARE_WIDGET(PopupButton);
DECLARE_WIDGET(CheckBox);
DECLARE_WIDGET(MessageDialog);
DECLARE_WIDGET(VScrollPanel);
DECLARE_WIDGET(ImagePanel);
DECLARE_WIDGET(ImageView);
DECLARE_WIDGET(ComboBox);
DECLARE_WIDGET(ProgressBar);
DECLARE_WIDGET(Slider);
DECLARE_WIDGET(TextBox);
DECLARE_WIDGET(ColorWheel);
DECLARE_WIDGET(Graph);
DECLARE_WIDGET(DoubleBox);
DECLARE_WIDGET(Int64Box);
DECLARE_WIDGET(ColorPicker);
DECLARE_WIDGET(StackedWidget);
DECLARE_WIDGET(TabHeader);
DECLARE_WIDGET(TabWidget);


/// Make pybind aware of the ref-counted wrapper type
PYBIND11_DECLARE_HOLDER_TYPE(T, ref<T>);

#if defined(__APPLE__) || defined(__linux__)
namespace {
    class semaphore {
    public:
        semaphore(int count = 0) : count(count) { }

        void notify() {
            std::unique_lock<std::mutex> lck(mtx);
            ++count;
            cv.notify_one();
        }

        void wait() {
            std::unique_lock<std::mutex> lck(mtx);
            while (count == 0)
                cv.wait(lck);
            --count;
        }

    private:
        std::mutex mtx;
        std::condition_variable cv;
        int count;
    };
}
#endif

class MainloopHandle;
static MainloopHandle *handle = nullptr;

class MainloopHandle {
public:
    bool active = false;
    bool detached = false;
    int refresh = 0;
    std::thread thread;

    #if defined(__APPLE__) || defined(__linux__)
        coro_context ctx_helper, ctx_main, ctx_thread;
        coro_stack stack;
        semaphore sema;
    #endif

    ~MainloopHandle() {
        join();
        handle = nullptr;
    }

    void join() {
        if (!detached)
            return;

        #if defined(__APPLE__) || defined(__linux__)
            /* Release GIL and disassociate from thread state (which was originally
               associated with the main Python thread) */
            py::gil_scoped_release thread_state(true);

            coro_transfer(&ctx_main, &ctx_thread);
            coro_stack_free(&stack);

            /* Destroy the thread state that was created in mainloop() */
            {
                py::gil_scoped_acquire acquire;
                acquire.dec_ref();
            }
        #endif

        thread.join();
        detached = false;

        #if defined(__APPLE__) || defined(__linux__)
            /* Reacquire GIL and reassociate with thread state
               [via RAII destructor in 'thread_state'] */
        #endif
    }
};

PYBIND11_PLUGIN(nanogui) {
    py::module m("nanogui", "NanoGUI plugin");

    py::class_<MainloopHandle>(m, "MainloopHandle")
        .def("join", &MainloopHandle::join);

    m.def("init", &nanogui::init, D(init));
    m.def("shutdown", &nanogui::shutdown, D(shutdown));
    m.def("mainloop", [](int refresh, py::object detach) -> MainloopHandle* {
        if (detach != py::none()) {
            if (handle)
                throw std::runtime_error("Main loop is already running!");

            handle = new MainloopHandle();
            handle->detached = true;
            handle->refresh = refresh;

            #if defined(__APPLE__) || defined(__linux__)
                /* Release GIL and completely disassociate the calling thread
                   from its associated Python thread state data structure */
                py::gil_scoped_release thread_state(true);

                /* Create a new thread state for the nanogui main loop
                   and reference it once (to keep it from being constructed and
                   destructed at every callback invocation) */
                {
                    py::gil_scoped_acquire acquire;
                    acquire.inc_ref();
                }

                handle->thread = std::thread([]{
                    /* Handshake 1: wait for signal from detach_helper */
                    handle->sema.wait();

                    /* Swap context with main thread */
                    coro_transfer(&handle->ctx_thread, &handle->ctx_main);

                    /* Handshake 2: wait for signal from detach_helper */
                    handle->sema.notify();
                });

                void (*detach_helper)(void *) = [](void *ptr) -> void {
                    MainloopHandle *handle = (MainloopHandle *) ptr;

                    /* Handshake 1: Send signal to new thread  */
                    handle->sema.notify();

                    /* Enter main loop */
                    mainloop(handle->refresh);

                    /* Handshake 2: Wait for signal from new thread */
                    handle->sema.wait();

                    /* Return back to Python */
                    coro_transfer(&handle->ctx_helper, &handle->ctx_main);
                };

                /* Allocate an 8MB stack and transfer context to the
                   detach_helper function */
                coro_stack_alloc(&handle->stack, 8 * 1024 * 1024);
                coro_create(&handle->ctx_helper, detach_helper, handle,
                            handle->stack.sptr, handle->stack.ssze);
                coro_transfer(&handle->ctx_main, &handle->ctx_helper);
            #else
                handle->thread = std::thread([]{
                    mainloop(handle->refresh);
                });
            #endif

            #if defined(__APPLE__) || defined(__linux__)
                /* Reacquire GIL and reassociate with thread state on newly
                   created thread [via RAII destructor in 'thread_state'] */
            #endif

            return handle;
        } else {
            mainloop(refresh);
            return nullptr;
        }
    }, py::arg("refresh") = 50, py::arg("detach") = py::none(),
       D(mainloop), py::keep_alive<0, 2>());

    m.def("leave", &nanogui::leave, D(leave));
    m.def("file_dialog", &nanogui::file_dialog, D(file_dialog));
    #if defined(__APPLE__)
        m.def("chdir_to_bundle_parent", &nanogui::chdir_to_bundle_parent);
    #endif
    m.def("utf8", [](int c) { return std::string(utf8(c).data()); }, D(utf8));
    m.def("loadImageDirectory", &nanogui::loadImageDirectory, D(loadImageDirectory));

    py::handle vector2i = py::detail::get_type_handle(typeid(Vector2i));
    if (!vector2i) {
        py::class_<Vector2i>(m, "Vector2i")
            .def(py::init<int, int>())
            .def_property("x", [](const Vector2i &v) { return v.x();}, [](Vector2i &v, int x) { v.x() = x; })
            .def_property("y", [](const Vector2i &v) { return v.y();}, [](Vector2i &v, int y) { v.y() = y; })
            .def("__getitem__", [](const Vector2i &m, size_t i) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                return m[i];
             })
            .def("__setitem__", [](Vector2i &m, size_t i, int v) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                m[i] = v;
             });
    } else {
        /* Don't create a new type if some other library has already
           exposed (potentially much fancier) Eigen Python bindings */
        m.attr("Vector2i") = vector2i;
    }

    py::handle vector2f = py::detail::get_type_handle(typeid(Vector2f));
    if (!vector2f) {
        py::class_<Vector2f>(m, "Vector2f")
            .def(py::init<int, int>())
            .def_property("x", [](const Vector2f &v) { return v.x();}, [](Vector2f &v, float x) { v.x() = x; })
            .def_property("y", [](const Vector2f &v) { return v.y();}, [](Vector2f &v, float y) { v.y() = y; })
            .def("__getitem__", [](const Vector2f &m, size_t i) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                return m[i];
             })
            .def("__setitem__", [](Vector2f &m, size_t i, float v) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                m[i] = v;
             });
    } else {
        /* Don't create a new type if some other library has already
           exposed (potentially much fancier) Eigen Python bindings */
        m.attr("Vector2f") = vector2f;
    }

    py::handle vectorXf = py::detail::get_type_handle(typeid(VectorXf));
    if (!vectorXf) {
        py::class_<VectorXf>(m, "VectorXf")
            .def(py::init<>())
            .def(py::init<int>())
            .def("resize", [](VectorXf &v, int i) { v.resize(i); })
            .def("__init__", [](VectorXf &v, const std::vector<float> &v2) {
                new (&v) VectorXf(v2.size());
                memcpy(v.data(), &v2[0], sizeof(float) * v2.size());
            })
            .def("__init__", [](VectorXf &v, py::buffer b) {
                py::buffer_info info = b.request();
                if (info.format != py::format_descriptor<float>::value) {
                    throw std::runtime_error("Incompatible buffer format!");
                } else if (info.ndim == 1 && info.strides[0] == sizeof(float)) {
                    new (&v) VectorXf(info.shape[0]);
                    memcpy(v.data(), info.ptr, sizeof(float) * info.shape[0]);
                } else if (info.ndim == 2 && ((info.shape[0] == 1 && info.strides[0] == sizeof(float))
                                           || (info.shape[1] == 1 && info.strides[1] == sizeof(float)))) {
                    new (&v) VectorXf(info.shape[0] * info.shape[1]);
                    memcpy(v.data(), info.ptr, sizeof(float) * info.shape[0] * info.shape[1]);
                } else {
                    throw std::runtime_error("Incompatible buffer dimension!");
                }
            })
            .def("size", [](const VectorXf &v) { return v.size(); })
            .def("__repr__", [](const VectorXf &v) {
                std::ostringstream oss;
                oss << v.transpose();
                return oss.str();
            })
            .def("__getitem__", [](const VectorXf &m, size_t i) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                return m[i];
             })
            .def("__setitem__", [](VectorXf &m, size_t i, float v) {
                if (i >= (size_t) m.size())
                    throw py::index_error();
                m[i] = v;
             })

            /* Buffer access for interacting with NumPy */
            .def_buffer([](VectorXf &m) -> py::buffer_info {
                return py::buffer_info(
                    m.data(),               /* Pointer to buffer */
                    sizeof(float),          /* Size of one scalar */
                    /* Python struct-style format descriptor */
                    py::format_descriptor<float>::value,
                    1,                      /* Number of dimensions */
                    { (size_t) m.size() },  /* Buffer dimensions */
                    { sizeof(float) }       /* Strides (in bytes) for each index */
                );
             });
    } else {
        /* Don't create a new type if some other library has already
           exposed (potentially much fancier) Eigen Python bindings */
        m.attr("VectorXf") = vectorXf;
    }

    py::enum_<Cursor>(m, "Cursor", D(Cursor))
        .value("Arrow", Cursor::Arrow)
        .value("IBeam", Cursor::IBeam)
        .value("Crosshair", Cursor::Crosshair)
        .value("Hand", Cursor::Hand)
        .value("HResize", Cursor::HResize)
        .value("VResize", Cursor::VResize);

    py::class_<Color>(m, "Color", D(Color))
        .def(py::init<float, float, float, float>(), D(Color, Color, 7))
        .def(py::init<float, float>(), D(Color, Color, 5))
        .def("contrastingColor", &Color::contrastingColor, D(Color, contrastingColor))
        .def_property("r",
            [](const Color &c) { return c.r(); },
            [](Color &c, float v) { c.r() = v; },
            D(Color, r))
        .def_property("g",
            [](const Color &c) { return c.g(); },
            [](Color &c, float v) { c.g() = v; },
            D(Color, g))
        .def_property("b",
            [](const Color &c) { return c.b(); },
            [](Color &c, float v) { c.b() = v; },
            D(Color, b));

    py::class_<NVGcontext> context(m, "NVGcontext");

    py::class_<Widget, ref<Widget>, PyWidget> widget(m, "Widget", D(Widget));
    widget
        .def(py::init<Widget *>(), D(Widget, Widget))
        .def("parent", (Widget *(Widget::*)(void)) &Widget::parent, D(Widget, parent))
        .def("setParent", &Widget::setParent, D(Widget, setParent))
        .def("layout", (Layout *(Widget::*)(void)) &Widget::layout, D(Widget, layout))
        .def("setLayout", &Widget::setLayout, D(Widget, setLayout))
        .def("theme", (Theme *(Widget::*)(void)) &Widget::theme, D(Widget, theme))
        .def("setTheme", &Widget::setTheme, D(Widget, setTheme))
        .def("position", &Widget::position, D(Widget, position))
        .def("setPosition", &Widget::setPosition, D(Widget, setPosition))
        .def("absolutePosition", &Widget::absolutePosition, D(Widget, absolutePosition))
        .def("size", &Widget::size, D(Widget, size))
        .def("setSize", &Widget::setSize, D(Widget, setSize))
        .def("width", &Widget::width, D(Widget, width))
        .def("setWidth", &Widget::setWidth, D(Widget, setWidth))
        .def("height", &Widget::height, D(Widget, height))
        .def("setHeight", &Widget::setHeight, D(Widget, setHeight))
        .def("fixedSize", &Widget::fixedSize, D(Widget, fixedSize))
        .def("setFixedSize", &Widget::setFixedSize, D(Widget, setFixedSize))
        .def("fixedWidth", &Widget::fixedWidth, D(Widget, fixedWidth))
        .def("setFixedWidth", &Widget::setFixedWidth, D(Widget, setFixedWidth))
        .def("fixedHeight", &Widget::fixedHeight, D(Widget, fixedHeight))
        .def("setFixedHeight", &Widget::setFixedHeight, D(Widget, setFixedHeight))
        .def("visible", &Widget::visible, D(Widget, visible))
        .def("setVisible", &Widget::setVisible, D(Widget, setVisible))
        .def("visibleRecursive", &Widget::visibleRecursive, D(Widget, visibleRecursive))
        .def("showBorder", &Widget::showBorder, D(Widget, showBorder))
        .def("setShowBorder", &Widget::setShowBorder, D(Widget, setShowBorder))
        .def("setBorderColor", &Widget::setBorderColor, D(Widget, setBorderColor))
        .def("borderColor", &Widget::borderColor, D(Widget, borderColor))
        .def("children", (std::vector<Widget *>&(Widget::*)(void)) &Widget::children,
             D(Widget, children), py::return_value_policy::reference)
        .def("addChild", (void (Widget::*) (int, Widget *)) &Widget::addChild, D(Widget, addChild))
        .def("addChild", (void (Widget::*) (Widget *)) &Widget::addChild, D(Widget, addChild, 2))
        .def("childCount", &Widget::childCount, D(Widget, childCount))
        .def("__len__", &Widget::childCount, D(Widget, childCount))
        .def("__iter__", [](const Widget &w) {
                return py::make_iterator(w.children().begin(), w.children().end());
            }, py::keep_alive<0, 1>())
        .def("childIndex", &Widget::childIndex, D(Widget, childIndex))
        .def("__getitem__", (Widget* (Widget::*)(int)) &Widget::childAt, D(Widget, childAt))
        .def("removeChild", (void(Widget::*)(int)) &Widget::removeChild, D(Widget, removeChild))
        .def("removeChild", (void(Widget::*)(const Widget *)) &Widget::removeChild, D(Widget, removeChild, 2))
        .def("window", &Widget::window, D(Widget, window))
        .def("setId", &Widget::setId, D(Widget, setId))
        .def("id", &Widget::id, D(Widget, id))
        .def("enabled", &Widget::enabled, D(Widget, enabled))
        .def("setEnabled", &Widget::setEnabled, D(Widget, setEnabled))
        .def("focused", &Widget::focused, D(Widget, focused))
        .def("setFocused", &Widget::setFocused, D(Widget, setFocused))
        .def("requestFocus", &Widget::requestFocus, D(Widget, requestFocus))
        .def("tooltip", &Widget::tooltip, D(Widget, tooltip))
        .def("setTooltip", &Widget::setTooltip, D(Widget, setTooltip))
        .def("fontSize", &Widget::fontSize, D(Widget, fontSize))
        .def("setFontSize", &Widget::setFontSize, D(Widget, setFontSize))
        .def("hasFontSize", &Widget::hasFontSize, D(Widget, hasFontSize))
        .def("cursor", &Widget::cursor, D(Widget, cursor))
        .def("setCursor", &Widget::setCursor, D(Widget, setCursor))
        .def("findWidget", &Widget::findWidget, D(Widget, findWidget))
        .def("contains", &Widget::contains, D(Widget, contains))
        .def("mouseButtonEvent", &Widget::mouseButtonEvent, py::arg("p"), py::arg("button"),
             py::arg("down"), py::arg("modifiers"), D(Widget, mouseButtonEvent))
        .def("mouseMotionEvent", &Widget::mouseMotionEvent, py::arg("p"), py::arg("rel"),
             py::arg("button"), py::arg("modifiers"), D(Widget, mouseMotionEvent))
        .def("mouseDragEvent", &Widget::mouseDragEvent, py::arg("p"), py::arg("rel"),
             py::arg("button"), py::arg("modifiers"), D(Widget, mouseDragEvent))
        .def("mouseEnterEvent", &Widget::mouseEnterEvent, py::arg("p"), py::arg("enter"),
             D(Widget, mouseEnterEvent))
        .def("scrollEvent", &Widget::scrollEvent, py::arg("p"), py::arg("rel"),
             D(Widget, scrollEvent))
        .def("focusEvent", &Widget::focusEvent, py::arg("focused"), D(Widget, focusEvent))
        .def("keyboardEvent", &Widget::keyboardEvent, py::arg("key"), py::arg("scancode"),
             py::arg("action"), py::arg("modifiers"), D(Widget, keyboardEvent))
        .def("keyboardCharacterEvent", &Widget::keyboardCharacterEvent,
             D(Widget, keyboardCharacterEvent))
        .def("preferredSize", &Widget::preferredSize, D(Widget, preferredSize))
        .def("performLayout", &Widget::performLayout, D(Widget, performLayout))
        .def("draw", &Widget::draw, D(Widget, draw));

    py::class_<Screen, ref<Screen>, PyScreen>(m, "Screen", widget, D(Screen))
        .def(py::init<const Vector2i &, const std::string &, bool, bool, int, int, int, int, int, unsigned int, unsigned int>(),
            py::arg("size"), py::arg("caption"), py::arg("resizable") = true, py::arg("fullscreen") = false,
            py::arg("colorBits") = 8, py::arg("alphaBits") = 8, py::arg("depthBits") = 24, py::arg("stencilBits") = 8,
            py::arg("nSamples") = 0, py::arg("glMajor") = 3, py::arg("glMinor") = 3, D(Screen, Screen))
        .def("caption", &Screen::caption, D(Screen, caption))
        .def("setCaption", &Screen::setCaption, D(Screen, setCaption))
        .def("background", &Screen::background, D(Screen, background))
        .def("setBackground", &Screen::setBackground, D(Screen, setBackground))
        .def("setVisible", &Screen::setVisible, D(Screen, setVisible))
        .def("setSize", &Screen::setSize, D(Screen, setSize))
        .def("performLayout", (void(Screen::*)(void)) &Screen::performLayout, D(Screen, performLayout))
        .def("drawAll", &Screen::drawAll, D(Screen, drawAll))
        .def("drawContents", &Screen::drawContents, D(Screen, drawContents))
        .def("resizeEvent", &Screen::resizeEvent, py::arg("size"), D(Screen, resizeEvent))
        .def("dropEvent", &Screen::dropEvent, D(Screen, dropEvent))
        .def("mousePos", &Screen::mousePos, D(Screen, mousePos))
        .def("glfwWindow", &Screen::glfwWindow, D(Screen, glfwWindow),
                py::return_value_policy::reference)
        .def("nvgContext", &Screen::nvgContext, D(Screen, nvgContext),
                py::return_value_policy::reference);

    py::class_<Window, ref<Window>, PyWindow> window(m, "Window", widget, D(Window));
    window
        .def(py::init<Widget *, const std::string>(), py::arg("parent"),
             py::arg("title") = std::string("Untitled"), D(Window, Window))
        .def("title", &Window::title, D(Window, title))
        .def("setTitle", &Window::setTitle, D(Window, setTitle))
        .def("modal", &Window::modal, D(Window, modal))
        .def("setModal", &Window::setModal, D(Window, setModal))
        .def("dispose", &Window::dispose, D(Window, dispose))
        .def("buttonPanel", &Window::buttonPanel, D(Window, buttonPanel))
        .def("center", &Window::center, D(Window, center));

    py::enum_<Alignment>(m, "Alignment", D(Alignment))
        .value("Minimum", Alignment::Minimum)
        .value("Middle", Alignment::Middle)
        .value("Maximum", Alignment::Maximum)
        .value("Fill", Alignment::Fill);

    py::enum_<Orientation>(m, "Orientation", D(Orientation))
        .value("Horizontal", Orientation::Horizontal)
        .value("Vertical", Orientation::Vertical);

    py::class_<Layout, ref<Layout>, PyLayout> layout(m, "Layout", D(Layout));
    layout
        .def("preferredSize", &Layout::preferredSize, D(Layout, preferredSize))
        .def("performLayout", &Layout::performLayout, D(Layout, performLayout));

    py::class_<BoxLayout, ref<BoxLayout>, PyBoxLayout>(m, "BoxLayout", layout, D(BoxLayout))
        .def(py::init<Orientation, Alignment, int, int>(),
             py::arg("orientation"), py::arg("alignment") = Alignment::Middle,
             py::arg("margin") = 0, py::arg("spacing") = 0, D(BoxLayout, BoxLayout))
        .def("orientation", &BoxLayout::orientation, D(BoxLayout, orientation))
        .def("setOrientation", &BoxLayout::setOrientation, D(BoxLayout, setOrientation))
        .def("alignment", &BoxLayout::alignment, D(BoxLayout, alignment))
        .def("setAlignment", &BoxLayout::setAlignment, D(BoxLayout, setAlignment))
        .def("margin", &BoxLayout::margin, D(BoxLayout, margin))
        .def("setMargin", &BoxLayout::setMargin, D(BoxLayout, setMargin))
        .def("spacing", &BoxLayout::spacing, D(BoxLayout, spacing))
        .def("setSpacing", &BoxLayout::setSpacing, D(BoxLayout, setSpacing));

    py::class_<GroupLayout, ref<GroupLayout>, PyGroupLayout>(m, "GroupLayout", layout, D(GroupLayout))
        .def(py::init<int, int, int, int>(),
             py::arg("margin") = 15, py::arg("spacing") = 6,
             py::arg("groupSpacing") = 14, py::arg("groupIndent") = 20,
             D(GroupLayout, GroupLayout))
        .def("margin", &GroupLayout::margin, D(GroupLayout, margin))
        .def("setMargin", &GroupLayout::setMargin, D(GroupLayout, setMargin))
        .def("spacing", &GroupLayout::spacing, D(GroupLayout, spacing))
        .def("setSpacing", &GroupLayout::setSpacing, D(GroupLayout, setSpacing))
        .def("groupIndent", &GroupLayout::groupIndent, D(GroupLayout, groupIndent))
        .def("setGroupIndent", &GroupLayout::setGroupIndent, D(GroupLayout, setGroupIndent))
        .def("groupSpacing", &GroupLayout::groupSpacing, D(GroupLayout, groupSpacing))
        .def("setGroupSpacing", &GroupLayout::setGroupSpacing, D(GroupLayout, setGroupSpacing));

    py::class_<GridLayout, ref<GridLayout>, PyGridLayout>(m, "GridLayout", layout, D(GridLayout))
        .def(py::init<Orientation, int, Alignment, int, int>(),
             py::arg("orientation") = Orientation::Horizontal,
             py::arg("resolution") = 2, py::arg("alignment") = Alignment::Middle,
             py::arg("margin") = 0, py::arg("spacing") = 0,
             D(GridLayout, GridLayout))
        .def("orientation", &GridLayout::orientation, D(GridLayout, orientation))
        .def("setOrientation", &GridLayout::setOrientation, D(GridLayout, setOrientation))
        .def("resolution", &GridLayout::resolution, D(GridLayout, resolution))
        .def("setResolution", &GridLayout::setResolution, D(GridLayout, setResolution))
        .def("margin", &GridLayout::margin, D(GridLayout, margin))
        .def("setMargin", &GridLayout::setMargin, D(GridLayout, setMargin))
        .def("spacing", &GridLayout::spacing, D(GridLayout, spacing))
        .def("setSpacing", (void(GridLayout::*)(int)) &GridLayout::setSpacing, D(GridLayout, setSpacing))
        .def("setSpacing", (void(GridLayout::*)(int, int)) &GridLayout::setSpacing, D(GridLayout, setSpacing, 2))
        .def("alignment", &GridLayout::alignment, D(GridLayout, alignment))
        .def("setColAlignment", (void(GridLayout::*)(Alignment)) &GridLayout::setColAlignment, D(GridLayout, setColAlignment))
        .def("setRowAlignment", (void(GridLayout::*)(Alignment)) &GridLayout::setRowAlignment, D(GridLayout, setRowAlignment))
        .def("setColAlignment", (void(GridLayout::*)(const std::vector<Alignment>&)) &GridLayout::setColAlignment/*, D(GridLayout, setColAlignment, 2)*/)
        .def("setRowAlignment", (void(GridLayout::*)(const std::vector<Alignment>&)) &GridLayout::setRowAlignment/*, D(GridLayout, setRowAlignment, 2)*/);

    py::class_<AdvancedGridLayout, ref<AdvancedGridLayout>, PyAdvancedGridLayout> advGridLayout(
        m, "AdvancedGridLayout", layout, D(AdvancedGridLayout));

    advGridLayout
        .def(py::init<const std::vector<int> &, const std::vector<int> &>(),
             py::arg("widths"), py::arg("heights"),
             D(AdvancedGridLayout, AdvancedGridLayout))
        .def("rowCount", &AdvancedGridLayout::rowCount, D(AdvancedGridLayout, rowCount))
        .def("colCount", &AdvancedGridLayout::colCount, D(AdvancedGridLayout, colCount))
        .def("margin", &AdvancedGridLayout::margin, D(AdvancedGridLayout, margin))
        .def("setMargin", &AdvancedGridLayout::setMargin, D(AdvancedGridLayout, setMargin))
        .def("appendRow", &AdvancedGridLayout::appendRow, py::arg("size"),
             py::arg("stretch") = 0, D(AdvancedGridLayout, appendRow))
        .def("appendCol", &AdvancedGridLayout::appendCol, py::arg("size"),
             py::arg("stretch") = 0, D(AdvancedGridLayout, appendCol))
        .def("setRowStretch", &AdvancedGridLayout::setRowStretch, D(AdvancedGridLayout, setRowStretch))
        .def("setColStretch", &AdvancedGridLayout::setColStretch, D(AdvancedGridLayout, setColStretch))
        .def("setAnchor", &AdvancedGridLayout::setAnchor, D(AdvancedGridLayout, setAnchor))
        .def("anchor", &AdvancedGridLayout::anchor, D(AdvancedGridLayout, anchor));

    py::class_<AdvancedGridLayout::Anchor>(advGridLayout, "Anchor")
        .def(py::init<int, int, Alignment, Alignment>(),
             py::arg("x"), py::arg("y"),
             py::arg("horiz") = Alignment::Fill,
             py::arg("vert") = Alignment::Fill,
             D(AdvancedGridLayout, Anchor, Anchor, 2))
        .def(py::init<int, int, int, int, Alignment, Alignment>(),
             py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"),
             py::arg("horiz") = Alignment::Fill,
             py::arg("vert") = Alignment::Fill,
             D(AdvancedGridLayout, Anchor, Anchor, 3));

    py::class_<Label, ref<Label>, PyLabel>(m, "Label", widget, D(Label))
        .def(py::init<Widget *, const std::string &, const std::string &, int>(),
             py::arg("parent"), py::arg("caption"), py::arg("font") = std::string("sans"),
             py::arg("fontSize") = -1,
             D(Label, Label))
        .def("caption", &Label::caption, D(Label, caption))
        .def("setCaption", &Label::setCaption, D(Label, setCaption))
        .def("font", &Label::font, D(Label, font))
        .def("setFont", &Label::setFont, D(Label, setFont))
        .def("color", &Label::color, D(Label, color))
        .def("setColor", &Label::setColor, D(Label, setColor));

    py::class_<Button, ref<Button>, PyButton> button(m, "Button", widget, D(Button));
    button
        .def(py::init<Widget *, const std::string &, int>(),
             py::arg("parent"), py::arg("caption") = std::string("Untitled"), py::arg("icon") = 0, D(Button, Button))
        .def("caption", &Button::caption, D(Button, caption))
        .def("setCaption", &Button::setCaption, D(Button, setCaption))
        .def("backgroundColor", &Button::backgroundColor, D(Button, backgroundColor))
        .def("setBackgroundColor", &Button::setBackgroundColor, D(Button, setBackgroundColor))
        .def("textColor", &Button::textColor, D(Button, textColor))
        .def("setTextColor", &Button::setTextColor, D(Button, setTextColor))
        .def("icon", &Button::icon, D(Button, icon))
        .def("setIcon", &Button::setIcon, D(Button, setIcon))
        .def("flags", &Button::flags, D(Button, flags))
        .def("setFlags", &Button::setFlags, D(Button, setFlags))
        .def("iconPosition", &Button::iconPosition, D(Button, iconPosition))
        .def("setIconPosition", &Button::setIconPosition, D(Button, setIconPosition))
        .def("pushed", &Button::pushed, D(Button, pushed))
        .def("setPushed", &Button::setPushed, D(Button, setPushed))
        .def("callback", &Button::callback, D(Button, callback))
        .def("setCallback", &Button::setCallback, D(Button, setCallback))
        .def("changeCallback", &Button::changeCallback, D(Button, changeCallback))
        .def("setChangeCallback", &Button::setChangeCallback, D(Button, setChangeCallback))
        .def("buttonGroup", &Button::buttonGroup, D(Button, buttonGroup))
        .def("setButtonGroup", &Button::setButtonGroup, D(Button, setButtonGroup));

    py::enum_<Button::IconPosition>(button, "IconPosition", D(Button, IconPosition))
        .value("Left", Button::IconPosition::Left)
        .value("LeftCentered", Button::IconPosition::LeftCentered)
        .value("RightCentered", Button::IconPosition::RightCentered)
        .value("Right", Button::IconPosition::Right);

    py::enum_<Button::Flags>(button, "Flags", D(Button, Flags))
        .value("NormalButton", Button::Flags::NormalButton)
        .value("RadioButton", Button::Flags::RadioButton)
        .value("ToggleButton", Button::Flags::ToggleButton)
        .value("PopupButton", Button::Flags::PopupButton);

    py::class_<ToolButton, ref<ToolButton>, PyToolButton>(m, "ToolButton", button, D(ToolButton))
        .def(py::init<Widget *,int, const std::string &>(),
             py::arg("parent"), py::arg("icon"), py::arg("caption") = std::string(""),
             D(ToolButton, ToolButton));

    py::class_<Popup, ref<Popup>, PyPopup>(m, "Popup", window, D(Popup))
        .def(py::init<Widget *, Window *>(), py::arg("parent"), py::arg("parentWindow"), D(Popup, Popup))
        .def("anchorPos", &Popup::anchorPos, D(Popup, anchorPos))
        .def("setAnchorPos", &Popup::setAnchorPos, D(Popup, setAnchorPos))
        .def("anchorHeight", &Popup::anchorHeight, D(Popup, anchorHeight))
        .def("setAnchorHeight", &Popup::setAnchorHeight, D(Popup, setAnchorHeight))
        .def("parentWindow", (Window*(Popup::*)(void)) &Popup::parentWindow, D(Popup, parentWindow));

    py::class_<PopupButton, ref<PopupButton>, PyPopupButton> popupBtn(m, "PopupButton", button, D(PopupButton));
    popupBtn
        .def(py::init<Widget *, const std::string&, int, int>(),
                py::arg("parent"), py::arg("caption") = std::string("Untitled"),
                py::arg("buttonIcon") = 0, py::arg("chevronIcon") = ENTYPO_ICON_CHEVRON_SMALL_RIGHT,
                D(PopupButton, PopupButton))
        .def("popup", (Popup*(PopupButton::*)(void)) &PopupButton::popup, D(PopupButton, popup))
        .def("chevronIcon", &PopupButton::chevronIcon, D(PopupButton, chevronIcon))
        .def("setChevronIcon", &PopupButton::setChevronIcon, D(PopupButton, setChevronIcon));

    py::class_<CheckBox, ref<CheckBox>, PyCheckBox>(m, "CheckBox", widget, D(CheckBox))
        .def(py::init<Widget *, const std::string &>(), py::arg("parent"),
             py::arg("caption") = std::string("Untitled"),
             D(CheckBox, CheckBox))
        .def(py::init<Widget *, const std::string &, const std::function<void(bool)>&>(),
             py::arg("parent"), py::arg("caption"), py::arg("callback"),
             D(CheckBox, CheckBox))
        .def("caption", &CheckBox::caption, D(CheckBox, caption))
        .def("setCaption", &CheckBox::setCaption, D(CheckBox, setCaption))
        .def("checked", &CheckBox::checked, D(CheckBox, checked))
        .def("setChecked", &CheckBox::setChecked, D(CheckBox, setChecked))
        .def("pushed", &CheckBox::pushed, D(CheckBox, pushed))
        .def("setPushed", &CheckBox::setPushed, D(CheckBox, setPushed))
        .def("callback", &CheckBox::callback, D(CheckBox, callback))
        .def("setCallback", &CheckBox::setCallback, D(CheckBox, setCallback));

    py::class_<MessageDialog, ref<MessageDialog>, PyMessageDialog> mdlg(m, "MessageDialog", window, D(MessageDialog));
    mdlg
        .def(py::init<Widget *, MessageDialog::Type, const std::string&,
                      const std::string&, const std::string&, const std::string&, bool>(),
            py::arg("parent"), py::arg("type"), py::arg("title") = std::string("Untitled"),
            py::arg("message") = std::string("Message"), py::arg("buttonText") = std::string("OK"),
            py::arg("altButtonText") = std::string("Cancel"), py::arg("altButton") = false,
            D(MessageDialog, MessageDialog))
        .def("messageLabel", (Label * (MessageDialog::*)()) &MessageDialog::messageLabel, D(MessageDialog, messageLabel))
        .def("callback", &MessageDialog::callback, D(MessageDialog, callback))
        .def("setCallback", &MessageDialog::setCallback, D(MessageDialog, setCallback));

    py::enum_<MessageDialog::Type>(mdlg, "Type", D(MessageDialog, Type))
        .value("Information", MessageDialog::Type::Information)
        .value("Question", MessageDialog::Type::Question)
        .value("Warning", MessageDialog::Type::Warning);

    py::class_<VScrollPanel, ref<VScrollPanel>, PyVScrollPanel>(m, "VScrollPanel", widget, D(VScrollPanel))
        .def(py::init<Widget *>(), py::arg("parent"), D(VScrollPanel, VScrollPanel));

    py::class_<ImagePanel, ref<ImagePanel>, PyImagePanel>(m, "ImagePanel", widget, D(ImagePanel))
        .def(py::init<Widget *>(), py::arg("parent"), D(ImagePanel, ImagePanel))
        .def("images", &ImagePanel::images, D(ImagePanel, images))
        .def("setImages", &ImagePanel::setImages, D(ImagePanel, setImages))
        .def("callback", &ImagePanel::callback, D(ImagePanel, callback))
        .def("setCallback", &ImagePanel::setCallback, D(ImagePanel, setCallback));

    py::class_<ComboBox, ref<ComboBox>, PyComboBox>(m, "ComboBox", widget, D(ComboBox))
        .def(py::init<Widget *>(), py::arg("parent"), D(ComboBox, ComboBox))
        .def(py::init<Widget *, const std::vector<std::string> &>(),
             py::arg("parent"), py::arg("items")/*, D(ComboBox, ComboBox, 2)*/)
        .def(py::init<Widget *, const std::vector<std::string> &,
                      const std::vector<std::string> &>(),
             py::arg("parent"), py::arg("items"), py::arg("itemsShort")/* ,D(ComboBox, ComboBox, 3)*/)
        .def("callback", &ComboBox::callback, D(ComboBox, callback))
        .def("setCallback", &ComboBox::setCallback, D(ComboBox, setCallback))
        .def("selectedIndex", &ComboBox::selectedIndex, D(ComboBox, selectedIndex))
        .def("setSelectedIndex", &ComboBox::setSelectedIndex, D(ComboBox, setSelectedIndex))
        .def("setItems", (void(ComboBox::*)(const std::vector<std::string>&)) &ComboBox::setItems, D(ComboBox, setItems))
        .def("setItems", (void(ComboBox::*)(const std::vector<std::string>&,
                          const std::vector<std::string>&)) &ComboBox::setItems/*, D(ComboBox, setItems, 2)*/)
        .def("items", &ComboBox::items, D(ComboBox, items))
        .def("itemsShort", &ComboBox::itemsShort, D(ComboBox, itemsShort));

    py::class_<ProgressBar, ref<ProgressBar>, PyProgressBar>(m, "ProgressBar", widget, D(ProgressBar))
        .def(py::init<Widget *>(), py::arg("parent"), D(ProgressBar, ProgressBar))
        .def("value", &ProgressBar::value, D(ProgressBar, value))
        .def("setValue", &ProgressBar::setValue, D(ProgressBar, setValue));

    py::class_<Slider, ref<Slider>, PySlider>(m, "Slider", widget, D(Slider))
        .def(py::init<Widget *>(), py::arg("parent"), D(Slider, Slider))
        .def("value", &Slider::value, D(Slider, value))
        .def("setValue", &Slider::setValue, D(Slider, setValue))
        .def("highlightColor", &Slider::highlightColor, D(Slider, highlightColor))
        .def("setHighlightColor", &Slider::setHighlightColor, D(Slider, setHighlightColor))
        .def("highlightedRange", &Slider::highlightedRange, D(Slider, highlightedRange))
        .def("setHighlightedRange", &Slider::setHighlightedRange, D(Slider, setHighlightedRange))
        .def("setCallback", &Slider::setCallback, D(Slider, setCallback))
        .def("callback", &Slider::callback, D(Slider, callback))
        .def("setFinalCallback", &Slider::setFinalCallback, D(Slider, setFinalCallback))
        .def("finalCallback", &Slider::finalCallback, D(Slider, finalCallback));

    py::class_<TextBox, ref<TextBox>, PyTextBox> tbox(m, "TextBox", widget, D(TextBox));
    tbox
        .def(py::init<Widget *, const std::string &>(), py::arg("parent"),
            py::arg("value") = std::string("Untitled"), D(TextBox, TextBox))
        .def("editable", &TextBox::editable, D(TextBox, editable))
        .def("setEditable", &TextBox::setEditable, D(TextBox, setEditable))
        .def("spinnable", &TextBox::spinnable, D(TextBox, spinnable))
        .def("setSpinnable", &TextBox::setSpinnable, D(TextBox, setSpinnable))
        .def("value", &TextBox::value, D(TextBox, value))
        .def("setValue", &TextBox::setValue, D(TextBox, setValue))
        .def("defaultValue", &TextBox::defaultValue, D(TextBox, defaultValue))
        .def("setDefaultValue", &TextBox::setDefaultValue, D(TextBox, setDefaultValue))
        .def("alignment", &TextBox::alignment, D(TextBox, alignment))
        .def("setAlignment", &TextBox::setAlignment, D(TextBox, setAlignment))
        .def("units", &TextBox::units, D(TextBox, units))
        .def("setUnits", &TextBox::setUnits, D(TextBox, setUnits))
        .def("unitsImage", &TextBox::unitsImage, D(TextBox, unitsImage))
        .def("setUnitsImage", &TextBox::setUnitsImage, D(TextBox, setUnitsImage))
        .def("format", &TextBox::format, D(TextBox, format))
        .def("setFormat", &TextBox::setFormat, D(TextBox, setFormat))
        .def("callback", &TextBox::callback, D(TextBox, callback))
        .def("setCallback", &TextBox::setCallback, D(TextBox, setCallback));

    py::enum_<TextBox::Alignment>(tbox, "Alignment", D(TextBox, Alignment))
        .value("Left", TextBox::Alignment::Left)
        .value("Center", TextBox::Alignment::Center)
        .value("Right", TextBox::Alignment::Right);

    py::class_<Int64Box, ref<Int64Box>, PyInt64Box>(m, "IntBox", tbox, D(IntBox))
        .def(py::init<Widget *, int64_t>(), py::arg("parent"), py::arg("value") = (int64_t) 0, D(IntBox, IntBox))
        .def("value", &Int64Box::value, D(IntBox, value))
        .def("setValue", (void (Int64Box::*)(int64_t)) &Int64Box::setValue, D(IntBox, setValue))
        .def("setCallback", (void (Int64Box::*)(const std::function<void(int64_t)>&))
                &Int64Box::setCallback, D(IntBox, setCallback))
        .def("setValueIncrement", &Int64Box::setValueIncrement, D(IntBox, setValueIncrement))
        .def("setMinValue", &Int64Box::setMinValue, D(IntBox, setMinValue))
        .def("setMaxValue", &Int64Box::setMaxValue, D(IntBox, setMaxValue))
        .def("setMinValue", &Int64Box::setMinMaxValues, D(IntBox, setMinMaxValues));

    py::class_<DoubleBox, ref<DoubleBox>, PyDoubleBox>(m, "FloatBox", tbox, D(FloatBox))
        .def(py::init<Widget *, double>(), py::arg("parent"), py::arg("value") = 0.0)
        .def("value", &DoubleBox::value, D(FloatBox, value))
        .def("setValue", (void (DoubleBox::*)(double)) &DoubleBox::setValue, D(FloatBox, setValue))
        .def("setCallback", (void (DoubleBox::*)(const std::function<void(double)>&))
                &DoubleBox::setCallback, D(FloatBox, setCallback))
        .def("setValueIncrement", &DoubleBox::setValueIncrement, D(FloatBox, setValueIncrement))
        .def("setMinValue", &DoubleBox::setMinValue, D(FloatBox, setMinValue))
        .def("setMaxValue", &DoubleBox::setMaxValue, D(FloatBox, setMaxValue))
        .def("setMinValue", &DoubleBox::setMinMaxValues, D(FloatBox, setMinMaxValues));

    py::class_<ColorWheel, ref<ColorWheel>, PyColorWheel>(m, "ColorWheel", widget, D(ColorWheel))
        .def(py::init<Widget *>(), py::arg("parent"), D(ColorWheel, ColorWheel))
        .def(py::init<Widget *, const Color &>(), py::arg("parent"), py::arg("Color"))
        .def("color", &ColorWheel::color, D(ColorWheel, color))
        .def("setColor", &ColorWheel::setColor, D(ColorWheel, setColor))
        .def("callback", &ColorWheel::callback, D(ColorWheel, callback))
        .def("setCallback", &ColorWheel::setCallback, D(ColorWheel, setCallback));

    py::class_<ColorPicker, ref<ColorPicker>, PyColorPicker>(m, "ColorPicker", popupBtn, D(ColorPicker))
        .def(py::init<Widget *>(), py::arg("parent"), D(ColorPicker, ColorPicker))
        .def(py::init<Widget *, const Color &>(), py::arg("parent"), py::arg("Color"))
        .def("color", &ColorPicker::color, D(ColorPicker, color))
        .def("setColor", &ColorPicker::setColor, D(ColorPicker, setColor))
        .def("callback", &ColorPicker::callback, D(ColorPicker, callback))
        .def("setCallback", &ColorPicker::setCallback, D(ColorPicker, setCallback));

    py::class_<Graph, ref<Graph>, PyGraph>(m, "Graph", widget, D(Graph))
        .def(py::init<Widget *, const std::string &>(), py::arg("parent"),
             py::arg("caption") = std::string("Untitled"), D(Graph, Graph))
        .def("caption", &Graph::caption, D(Graph, caption))
        .def("setCaption", &Graph::setCaption, D(Graph, setCaption))
        .def("header", &Graph::header, D(Graph, header))
        .def("setHeader", &Graph::setHeader, D(Graph, setHeader))
        .def("footer", &Graph::footer, D(Graph, footer))
        .def("setFooter", &Graph::setFooter, D(Graph, setFooter))
        .def("backgroundColor", &Graph::backgroundColor, D(Graph, backgroundColor))
        .def("setBackgroundColor", &Graph::setBackgroundColor, D(Graph, setBackgroundColor))
        .def("foregroundColor", &Graph::foregroundColor, D(Graph, foregroundColor))
        .def("setForegroundColor", &Graph::setForegroundColor, D(Graph, setForegroundColor))
        .def("textColor", &Graph::textColor, D(Graph, textColor))
        .def("setTextColor", &Graph::setTextColor, D(Graph, setTextColor))
        .def("values", (VectorXf &(Graph::*)(void)) &Graph::values, D(Graph, values))
        .def("setValues", &Graph::setValues, D(Graph, setValues));

    py::class_<StackedWidget, ref<StackedWidget>, PyStackedWidget>(m, "StackedWidget", widget, D(StackedWidget))
        .def(py::init<Widget *>(), D(StackedWidget, StackedWidget))
        .def("selectedIndex", &StackedWidget::selectedIndex, D(StackedWidget, selectedIndex))
        .def("setSelectedIndex", &StackedWidget::setSelectedIndex, D(StackedWidget, setSelectedIndex));

    py::class_<TabHeader, ref<TabHeader>, PyTabHeader>(m, "TabHeader", widget, D(TabHeader))
        .def(py::init<Widget *, const std::string &>(), D(TabHeader, TabHeader))
        .def("setFont", &TabHeader::setFont, D(TabHeader, setFont))
        .def("font", &TabHeader::font, D(TabHeader, font))
        .def("overflowing", &TabHeader::overflowing, D(TabHeader, overflowing))
        .def("callback", &TabHeader::callback, D(TabHeader, callback))
        .def("setCallback", &TabHeader::setCallback, D(TabHeader, setCallback))
        .def("activeTab", &TabHeader::activeTab, D(TabHeader, activeTab))
        .def("setActiveTab", &TabHeader::setActiveTab, D(TabHeader, setActiveTab))
        .def("addTab", (void (TabHeader::*)(const std::string &)) &TabHeader::addTab, D(TabHeader, addTab))
        .def("addTab", (void (TabHeader::*)(int index, const std::string &)) &TabHeader::addTab, D(TabHeader, addTab, 2))
        .def("removeTab", (int (TabHeader::*)(const std::string &)) &TabHeader::removeTab, D(TabHeader, removeTab))
        .def("removeTab", (void (TabHeader::*)(int index)) &TabHeader::removeTab, D(TabHeader, removeTab, 2))
        .def("tabLabelAt", &TabHeader::tabLabelAt, D(TabHeader, tabLabelAt))
        .def("tabIndex", &TabHeader::tabIndex, D(TabHeader, tabIndex))
        .def("ensureTabVisible", &TabHeader::ensureTabVisible, D(TabHeader, ensureTabVisible));

    py::class_<TabWidget, ref<TabWidget>, PyTabWidget>(m, "TabWidget", widget, D(TabWidget))
        .def(py::init<Widget *>(), D(TabWidget, TabWidget))
        .def("setActiveTab", &TabWidget::setActiveTab, D(TabWidget, setActiveTab))
        .def("activeTab", &TabWidget::activeTab, D(TabWidget, activeTab))
        .def("tabCount", &TabWidget::tabCount, D(TabWidget, tabCount))
        .def("setCallback", &TabWidget::setCallback, D(TabWidget, setCallback))
        .def("callback", &TabWidget::callback, D(TabWidget, callback))
        .def("addTab", (void (TabWidget::*)(const std::string &, Widget *)) &TabWidget::addTab, D(TabWidget, addTab))
        .def("addTab", (void (TabWidget::*)(int index, const std::string &, Widget *)) &TabWidget::addTab, D(TabWidget, addTab, 2))
        .def("createTab", (Widget *(TabWidget::*)(const std::string &)) &TabWidget::createTab, D(TabWidget, createTab))
        .def("createTab", (Widget *(TabWidget::*)(int index, const std::string &)) &TabWidget::createTab, D(TabWidget, createTab, 2))
        .def("removeTab", (bool (TabWidget::*)(const std::string &)) &TabWidget::removeTab, D(TabWidget, removeTab))
        .def("removeTab", (void (TabWidget::*)(int index)) &TabWidget::removeTab, D(TabWidget, removeTab, 2))
        .def("tabLabelAt", &TabWidget::tabLabelAt, D(TabWidget, tabLabelAt))
        .def("tabIndex", &TabWidget::tabIndex, D(TabWidget, tabIndex))
        .def("tab", (Widget * (TabWidget::*)(const std::string &)) &TabWidget::tab, D(TabWidget, tab))
        .def("ensureTabVisible", &TabWidget::ensureTabVisible, D(TabWidget, ensureTabVisible));

    py::class_<ImageView, ref<ImageView>, PyImageView>(m, "ImageView", widget, D(ImageView))
        .def(py::init<Widget *, GLuint>(), D(ImageView, ImageView))
        .def("bindImage", &ImageView::bindImage, D(ImageView, bindImage))
        .def("imageShader", &ImageView::imageShader, D(ImageView, imageShader))
        .def("scaledImageSize", &ImageView::scaledImageSize, D(ImageView, scaledImageSize))
        .def("offset", &ImageView::offset, D(ImageView, offset))
        .def("setOffset", &ImageView::setOffset, D(ImageView, setOffset))
        .def("scale", &ImageView::scale, D(ImageView, scale))
        .def("fixedOffset", &ImageView::fixedOffset, D(ImageView, fixedOffset))
        .def("setFixedOffset", &ImageView::setFixedOffset, D(ImageView, setFixedOffset))
        .def("fixedScale", &ImageView::fixedScale, D(ImageView, fixedScale))
        .def("setFixedScale", &ImageView::setFixedScale, D(ImageView, setFixedScale))
        .def("zoomSensitivity", &ImageView::zoomSensitivity, D(ImageView, zoomSensitivity))
        .def("setZoomSensitivity", &ImageView::setZoomSensitivity, D(ImageView, setZoomSensitivity))
        .def("gridThreshold", &ImageView::gridThreshold, D(ImageView, gridThreshold))
        .def("setGridThreshold", &ImageView::setGridThreshold, D(ImageView, setGridThreshold))
        .def("pixelInfoThreshold", &ImageView::pixelInfoThreshold, D(ImageView, pixelInfoThreshold))
        .def("setPixelInfoThreshold", &ImageView::setPixelInfoThreshold, D(ImageView, setPixelInfoThreshold))
        .def("setPixelInfoCallback", &ImageView::setPixelInfoCallback, D(ImageView, setPixelInfoCallback))
        .def("pixelInfoCallback", &ImageView::pixelInfoCallback, D(ImageView, pixelInfoCallback))
        .def("setFontScaleFactor", &ImageView::setFontScaleFactor, D(ImageView, setFontScaleFactor))
        .def("fontScaleFactor", &ImageView::fontScaleFactor, D(ImageView, fontScaleFactor))
        .def("imageCoordinateAt", &ImageView::imageCoordinateAt, D(ImageView, imageCoordinateAt))
        .def("clampedImageCoordinateAt", &ImageView::clampedImageCoordinateAt, D(ImageView, clampedImageCoordinateAt))
        .def("positionForCoordinate", &ImageView::positionForCoordinate, D(ImageView, positionForCoordinate))
        .def("setImageCoordinateAt", &ImageView::setImageCoordinateAt, D(ImageView, setImageCoordinateAt))
        .def("center",&ImageView::center, D(ImageView, center))
        .def("fit", &ImageView::fit, D(ImageView, fit))
        .def("setScaleCentered", &ImageView::setScaleCentered, D(ImageView, setScaleCentered))
        .def("moveOffset", &ImageView::moveOffset, D(ImageView, moveOffset))
        .def("zoom", &ImageView::zoom, D(ImageView, zoom))
        .def("gridVisible", &ImageView::gridVisible, D(ImageView, gridVisible))
        .def("pixelInfoVisible", &ImageView::pixelInfoVisible, D(ImageView, pixelInfoVisible))
        .def("helpersVisible", &ImageView::helpersVisible, D(ImageView, helpersVisible));

    enum DummyEnum { };

    py::class_<FormHelper>(m, "FormHelper", D(FormHelper))
        .def(py::init<Screen *>(), D(FormHelper, FormHelper))
        .def("addWindow", &FormHelper::addWindow, py::arg("pos"),
             py::arg("title") = std::string("Untitled"),
             D(FormHelper, addWindow))
        .def("addGroup", &FormHelper::addGroup, D(FormHelper, addGroup))
        .def("addButton", &FormHelper::addButton, py::arg("label"),
             py::arg("cb"), D(FormHelper, addGroup))
        .def("addBoolVariable",
             [](FormHelper &h, const std::string &label,
                const std::function<void(bool) > &setter,
                const std::function<bool(void) > &getter, bool editable) -> CheckBox* {
                return h.addVariable(label, setter, getter, editable);
             },
             py::arg("label"), py::arg("setter"), py::arg("getter"),
             py::arg("editable") = true)
        .def("addIntVariable",
             [](FormHelper &h, const std::string &label,
                const std::function<void(int64_t) > &setter,
                const std::function<int64_t(void) > &getter, bool editable) -> Int64Box* {
                return h.addVariable(label, setter, getter, editable);
             },
             py::arg("label"), py::arg("setter"), py::arg("getter"),
             py::arg("editable") = true)
        .def("addDoubleVariable",
             [](FormHelper &h, const std::string &label,
                const std::function<void(double) > &setter,
                const std::function<double(void) > &getter, bool editable) -> FloatBox<double>* {
                return h.addVariable(label, setter, getter, editable);
             },
             py::arg("label"), py::arg("setter"), py::arg("getter"),
             py::arg("editable") = true)
        .def("addStringVariable",
             [](FormHelper &h, const std::string &label,
                const std::function<void(std::string) > &setter,
                const std::function<std::string(void) > &getter, bool editable) -> TextBox* {
                return h.addVariable(label, setter, getter, editable);
             },
             py::arg("label"), py::arg("setter"), py::arg("getter"),
             py::arg("editable") = true)
        .def("addColorVariable",
             [](FormHelper &h, const std::string &label,
                const std::function<void(Color) > &setter,
                const std::function<Color(void) > &getter, bool editable) -> ColorPicker* {
                return h.addVariable(label, setter, getter, editable);
             },
             py::arg("label"), py::arg("setter"), py::arg("getter"),
             py::arg("editable") = true)
        .def("addEnumVariable",
             [](FormHelper &h, const std::string &label,
                const std::function<void(int) > &setter,
                const std::function<int(void) > &getter, bool editable) -> ComboBox* {
                return h.addVariable(label,
                        reinterpret_cast<const std::function<void(DummyEnum)>&>(setter),
                        reinterpret_cast<const std::function<DummyEnum(void)>&>(getter),
                        editable);
             },
             py::arg("label"), py::arg("setter"), py::arg("getter"),
             py::arg("editable") = true)
        .def("addWidget", &FormHelper::addWidget, D(FormHelper, addWidget))
        .def("refresh", &FormHelper::refresh, D(FormHelper, refresh))
        .def("window", &FormHelper::window, D(FormHelper, window))
        .def("setWindow", &FormHelper::setWindow, D(FormHelper, setWindow))
        .def("fixedSize", &FormHelper::fixedSize, D(FormHelper, fixedSize))
        .def("setFixedSize", &FormHelper::setFixedSize, D(FormHelper, setFixedSize))
        .def("groupFontName", &FormHelper::groupFontName, D(FormHelper, groupFontName))
        .def("setGroupFontName", &FormHelper::setGroupFontName, D(FormHelper, setGroupFontName))
        .def("labelFontName", &FormHelper::labelFontName, D(FormHelper, labelFontName))
        .def("setLabelFontName", &FormHelper::setLabelFontName, D(FormHelper, setLabelFontName))
        .def("groupFontSize", &FormHelper::groupFontSize, D(FormHelper, groupFontSize))
        .def("setGroupFontSize", &FormHelper::setGroupFontSize, D(FormHelper, setGroupFontSize))
        .def("labelFontSize", &FormHelper::labelFontSize, D(FormHelper, labelFontSize))
        .def("setLabelFontSize", &FormHelper::setLabelFontSize, D(FormHelper, setLabelFontSize))
        .def("widgetFontSize", &FormHelper::widgetFontSize, D(FormHelper, widgetFontSize))
        .def("setWidgetFontSize", &FormHelper::setWidgetFontSize, D(FormHelper, setWidgetFontSize));

    /* GLFW constants */
    {
        #define C(name) g.attr(#name) = py::int_(GLFW_##name);
        py::module g = m.def_submodule("glfw");
        C(KEY_UNKNOWN); C(KEY_SPACE); C(KEY_APOSTROPHE); C(KEY_COMMA);
        C(KEY_MINUS); C(KEY_PERIOD); C(KEY_SLASH); C(KEY_0); C(KEY_1);
        C(KEY_2); C(KEY_3); C(KEY_4); C(KEY_5); C(KEY_6); C(KEY_7); C(KEY_8);
        C(KEY_9); C(KEY_SEMICOLON); C(KEY_EQUAL); C(KEY_A); C(KEY_B); C(KEY_C);
        C(KEY_D); C(KEY_E); C(KEY_F); C(KEY_G); C(KEY_H); C(KEY_I); C(KEY_J);
        C(KEY_K); C(KEY_L); C(KEY_M); C(KEY_N); C(KEY_O); C(KEY_P); C(KEY_Q);
        C(KEY_R); C(KEY_S); C(KEY_T); C(KEY_U); C(KEY_V); C(KEY_W); C(KEY_X);
        C(KEY_Y); C(KEY_Z); C(KEY_LEFT_BRACKET); C(KEY_BACKSLASH);
        C(KEY_RIGHT_BRACKET); C(KEY_GRAVE_ACCENT); C(KEY_WORLD_1);
        C(KEY_WORLD_2); C(KEY_ESCAPE); C(KEY_ENTER); C(KEY_TAB);
        C(KEY_BACKSPACE); C(KEY_INSERT); C(KEY_DELETE); C(KEY_RIGHT);
        C(KEY_LEFT); C(KEY_DOWN); C(KEY_UP); C(KEY_PAGE_UP); C(KEY_PAGE_DOWN);
        C(KEY_HOME); C(KEY_END); C(KEY_CAPS_LOCK); C(KEY_SCROLL_LOCK);
        C(KEY_NUM_LOCK); C(KEY_PRINT_SCREEN); C(KEY_PAUSE); C(KEY_F1);
        C(KEY_F2); C(KEY_F3); C(KEY_F4); C(KEY_F5); C(KEY_F6); C(KEY_F7);
        C(KEY_F8); C(KEY_F9); C(KEY_F10); C(KEY_F11); C(KEY_F12); C(KEY_F13);
        C(KEY_F14); C(KEY_F15); C(KEY_F16); C(KEY_F17); C(KEY_F18); C(KEY_F19);
        C(KEY_F20); C(KEY_F21); C(KEY_F22); C(KEY_F23); C(KEY_F24); C(KEY_F25);
        C(KEY_KP_0); C(KEY_KP_1); C(KEY_KP_2); C(KEY_KP_3); C(KEY_KP_4);
        C(KEY_KP_5); C(KEY_KP_6); C(KEY_KP_7); C(KEY_KP_8); C(KEY_KP_9);
        C(KEY_KP_DECIMAL); C(KEY_KP_DIVIDE); C(KEY_KP_MULTIPLY);
        C(KEY_KP_SUBTRACT); C(KEY_KP_ADD); C(KEY_KP_ENTER); C(KEY_KP_EQUAL);
        C(KEY_LEFT_SHIFT); C(KEY_LEFT_CONTROL); C(KEY_LEFT_ALT);
        C(KEY_LEFT_SUPER); C(KEY_RIGHT_SHIFT); C(KEY_RIGHT_CONTROL);
        C(KEY_RIGHT_ALT); C(KEY_RIGHT_SUPER); C(KEY_MENU); C(KEY_LAST);
        C(MOD_SHIFT); C(MOD_CONTROL); C(MOD_ALT); C(MOD_SUPER);
        C(MOUSE_BUTTON_1); C(MOUSE_BUTTON_2); C(MOUSE_BUTTON_3);
        C(MOUSE_BUTTON_4); C(MOUSE_BUTTON_5); C(MOUSE_BUTTON_6);
        C(MOUSE_BUTTON_7); C(MOUSE_BUTTON_8); C(MOUSE_BUTTON_LAST);
        C(MOUSE_BUTTON_LEFT); C(MOUSE_BUTTON_RIGHT); C(MOUSE_BUTTON_MIDDLE);
        C(RELEASE); C(PRESS); C(REPEAT);
        #undef C
    }

    /* Entypo constants */
    {
        #define C(name) g.attr("ICON_" #name) = py::int_(ENTYPO_ICON_##name);
        py::module g = m.def_submodule("entypo");
        C(PHONE); C(MOBILE); C(MOUSE); C(ADDRESS); C(MAIL); C(PAPER_PLANE);
        C(PENCIL); C(FEATHER); C(ATTACH); C(INBOX); C(REPLY); C(REPLY_ALL);
        C(FORWARD); C(USER); C(USERS); C(ADD_USER); C(VCARD); C(EXPORT);
        C(LOCATION); C(MAP); C(COMPASS); C(DIRECTION); C(HAIR_CROSS); C(SHARE);
        C(SHAREABLE); C(HEART); C(HEART_EMPTY); C(STAR); C(STAR_EMPTY);
        C(THUMBS_UP); C(THUMBS_DOWN); C(CHAT); C(COMMENT); C(QUOTE); C(HOME);
        C(POPUP); C(SEARCH); C(FLASHLIGHT); C(PRINT); C(BELL); C(LINK);
        C(FLAG); C(COG); C(TOOLS); C(TROPHY); C(TAG); C(CAMERA); C(MEGAPHONE);
        C(MOON); C(PALETTE); C(LEAF); C(NOTE); C(BEAMED_NOTE); C(NEW);
        C(GRADUATION_CAP); C(BOOK); C(NEWSPAPER); C(BAG); C(AIRPLANE);
        C(LIFEBUOY); C(EYE); C(CLOCK); C(MIC); C(CALENDAR); C(FLASH);
        C(THUNDER_CLOUD); C(DROPLET); C(CD); C(BRIEFCASE); C(AIR);
        C(HOURGLASS); C(GAUGE); C(LANGUAGE); C(NETWORK); C(KEY); C(BATTERY);
        C(BUCKET); C(MAGNET); C(DRIVE); C(CUP); C(ROCKET); C(BRUSH);
        C(SUITCASE); C(TRAFFIC_CONE); C(GLOBE); C(KEYBOARD); C(BROWSER);
        C(PUBLISH); C(PROGRESS_3); C(PROGRESS_2); C(PROGRESS_1); C(PROGRESS_0);
        C(LIGHT_DOWN); C(LIGHT_UP); C(ADJUST); C(CODE); C(MONITOR);
        C(INFINITY); C(LIGHT_BULB); C(CREDIT_CARD); C(DATABASE); C(VOICEMAIL);
        C(CLIPBOARD); C(CART); C(BOX); C(TICKET); C(RSS); C(SIGNAL);
        C(THERMOMETER); C(WATER); C(SWEDEN); C(LINE_GRAPH); C(PIE_CHART);
        C(BAR_GRAPH); C(AREA_GRAPH); C(LOCK); C(LOCK_OPEN); C(LOGOUT);
        C(LOGIN); C(CHECK); C(CROSS); C(SQUARED_MINUS); C(SQUARED_PLUS);
        C(SQUARED_CROSS); C(CIRCLED_MINUS); C(CIRCLED_PLUS); C(CIRCLED_CROSS);
        C(MINUS); C(PLUS); C(ERASE); C(BLOCK); C(INFO); C(CIRCLED_INFO);
        C(HELP); C(CIRCLED_HELP); C(WARNING); C(CYCLE); C(CW); C(CCW);
        C(SHUFFLE); C(BACK); C(LEVEL_DOWN); C(RETWEET); C(LOOP);
        C(BACK_IN_TIME); C(LEVEL_UP); C(SWITCH); C(NUMBERED_LIST);
        C(ADD_TO_LIST); C(LAYOUT); C(LIST); C(TEXT_DOC); C(TEXT_DOC_INVERTED);
        C(DOC); C(DOCS); C(LANDSCAPE_DOC); C(PICTURE); C(VIDEO); C(MUSIC);
        C(FOLDER); C(ARCHIVE); C(TRASH); C(UPLOAD); C(DOWNLOAD); C(SAVE);
        C(INSTALL); C(CLOUD); C(UPLOAD_CLOUD); C(BOOKMARK); C(BOOKMARKS);
        C(OPEN_BOOK); C(PLAY); C(PAUS); C(RECORD); C(STOP); C(FF); C(FB);
        C(TO_START); C(TO_END); C(RESIZE_FULL); C(RESIZE_SMALL); C(VOLUME);
        C(SOUND); C(MUTE); C(FLOW_CASCADE); C(FLOW_BRANCH); C(FLOW_TREE);
        C(FLOW_LINE); C(FLOW_PARALLEL); C(LEFT_BOLD); C(DOWN_BOLD); C(UP_BOLD);
        C(RIGHT_BOLD); C(LEFT); C(DOWN); C(UP); C(RIGHT); C(CIRCLED_LEFT);
        C(CIRCLED_DOWN); C(CIRCLED_UP); C(CIRCLED_RIGHT); C(TRIANGLE_LEFT);
        C(TRIANGLE_DOWN); C(TRIANGLE_UP); C(TRIANGLE_RIGHT); C(CHEVRON_LEFT);
        C(CHEVRON_DOWN); C(CHEVRON_UP); C(CHEVRON_RIGHT);
        C(CHEVRON_SMALL_LEFT); C(CHEVRON_SMALL_DOWN); C(CHEVRON_SMALL_UP);
        C(CHEVRON_SMALL_RIGHT); C(CHEVRON_THIN_LEFT); C(CHEVRON_THIN_DOWN);
        C(CHEVRON_THIN_UP); C(CHEVRON_THIN_RIGHT); C(LEFT_THIN); C(DOWN_THIN);
        C(UP_THIN); C(RIGHT_THIN); C(ARROW_COMBO); C(THREE_DOTS); C(TWO_DOTS);
        C(DOT); C(CC); C(CC_BY); C(CC_NC); C(CC_NC_EU); C(CC_NC_JP); C(CC_SA);
        C(CC_ND); C(CC_PD); C(CC_ZERO); C(CC_SHARE); C(CC_REMIX); C(DB_LOGO);
        C(DB_SHAPE); C(GITHUB); C(C_GITHUB); C(FLICKR); C(C_FLICKR); C(VIMEO);
        C(C_VIMEO); C(TWITTER); C(C_TWITTER); C(FACEBOOK); C(C_FACEBOOK);
        C(S_FACEBOOK); C(GOOGLEPLUS); C(C_GOOGLEPLUS); C(PINTEREST);
        C(C_PINTEREST); C(TUMBLR); C(C_TUMBLR); C(LINKEDIN); C(C_LINKEDIN);
        C(DRIBBBLE); C(C_DRIBBBLE); C(STUMBLEUPON); C(C_STUMBLEUPON);
        C(LASTFM); C(C_LASTFM); C(RDIO); C(C_RDIO); C(SPOTIFY); C(C_SPOTIFY);
        C(QQ); C(INSTAGRAM); C(DROPBOX); C(EVERNOTE); C(FLATTR); C(SKYPE);
        C(C_SKYPE); C(RENREN); C(SINA_WEIBO); C(PAYPAL); C(PICASA);
        C(SOUNDCLOUD); C(MIXI); C(BEHANCE); C(GOOGLE_CIRCLES); C(VK);
        C(SMASHING);
        #undef C
    }

    return m.ptr();
}

#endif
