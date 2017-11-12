#ifdef NANOGUI_PYTHON

#include "python.h"

DECLARE_WIDGET(Widget);
DECLARE_SCREEN(Screen);
DECLARE_WIDGET(Window);

void register_widget(py::module &m) {
    py::class_<Widget, ref<Widget>, PyWidget>(m, "Widget", D(Widget))
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
        .def("__delitem__", (void(Widget::*)(int)) &Widget::removeChild, D(Widget, removeChild, 2))
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

    py::class_<Window, Widget, ref<Window>, PyWindow>(m, "Window", D(Window))
        .def(py::init<Widget *, const std::string &, const std::string &>(), py::arg("parent"),
             py::arg("title") = std::string("Untitled"), py::arg("font") = std::string("sans-bold"), D(Window, Window))
        .def("title", &Window::title, D(Window, title))
        .def("setTitle", &Window::setTitle, D(Window, setTitle))
        .def("font", &Window::font, D(Window, font))
        .def("setFont", &Window::setFont, D(Window, setFont))
        .def("modal", &Window::modal, D(Window, modal))
        .def("setModal", &Window::setModal, D(Window, setModal))
        .def("dispose", &Window::dispose, D(Window, dispose))
        .def("buttonPanel", &Window::buttonPanel, D(Window, buttonPanel))
        .def("center", &Window::center, D(Window, center));

    py::class_<Screen, Widget, ref<Screen>, PyScreen>(m, "Screen", D(Screen))
        .def(py::init<const Vector2i &, const std::string &, bool, bool, int, int, int, int, int, unsigned int, unsigned int>(),
            py::arg("size"), py::arg("caption"), py::arg("resizable") = true, py::arg("fullscreen") = false,
            py::arg("colorBits") = 8, py::arg("alphaBits") = 8, py::arg("depthBits") = 24, py::arg("stencilBits") = 8,
            py::arg("nSamples") = 0, py::arg("glMajor") = 3, py::arg("glMinor") = 3, D(Screen, Screen))
        .def("caption", &Screen::caption, D(Screen, caption))
        .def("setCaption", &Screen::setCaption, D(Screen, setCaption))
        .def("tooltipFont", &Screen::tooltipFont, D(Screen, tooltipFont))
        .def("setTooltipFont", &Screen::setTooltipFont, D(Screen, setTooltipFont))
        .def("background", &Screen::background, D(Screen, background))
        .def("setBackground", &Screen::setBackground, D(Screen, setBackground))
        .def("setVisible", &Screen::setVisible, D(Screen, setVisible))
        .def("setSize", &Screen::setSize, D(Screen, setSize))
        .def("performLayout", (void(Screen::*)(void)) &Screen::performLayout, D(Screen, performLayout))
        .def("drawAll", &Screen::drawAll, D(Screen, drawAll))
        .def("drawContents", &Screen::drawContents, D(Screen, drawContents))
        .def("resizeEvent", &Screen::resizeEvent, py::arg("size"), D(Screen, resizeEvent))
        .def("resizeCallback", &Screen::resizeCallback)
        .def("setResizeCallback", &Screen::setResizeCallback)
        .def("dropEvent", &Screen::dropEvent, D(Screen, dropEvent))
        .def("mousePos", &Screen::mousePos, D(Screen, mousePos))
        .def("pixelRatio", &Screen::pixelRatio, D(Screen, pixelRatio))
        .def("glfwWindow", &Screen::glfwWindow, D(Screen, glfwWindow),
                py::return_value_policy::reference)
        .def("nvgContext", &Screen::nvgContext, D(Screen, nvgContext),
                py::return_value_policy::reference);
}
#endif
