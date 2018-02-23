#ifdef NANOGUI_PYTHON

#include "python.h"

DECLARE_WIDGET(Button);
DECLARE_WIDGET(ToolButton);
DECLARE_WIDGET(PopupButton);
DECLARE_WIDGET(ComboBox);
DECLARE_WIDGET(CheckBox);

void register_button(py::module &m) {
    py::class_<Button, FontWidget, ref<Button>, PyButton> button(m, "Button", D(Button));
    button
        .def(py::init<Widget *, const std::string &, int, const std::string &>(),
             py::arg("parent"), py::arg("caption") = std::string("Untitled"), py::arg("icon") = 0, py::arg("font") = std::string(""), D(Button, Button))
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

    py::class_<ToolButton, Button, ref<ToolButton>, PyToolButton>(m, "ToolButton", D(ToolButton))
        .def(py::init<Widget *,int, const std::string &>(),
             py::arg("parent"), py::arg("icon"), py::arg("caption") = std::string(""),
             D(ToolButton, ToolButton));

    py::class_<PopupButton, Button, ref<PopupButton>, PyPopupButton> popupBtn(m, "PopupButton", D(PopupButton));
    popupBtn
        .def(py::init<Widget *, const std::string&, int>(),
                py::arg("parent"), py::arg("caption") = std::string("Untitled"),
                py::arg("buttonIcon") = 0, D(PopupButton, PopupButton))
        .def("popup", (Popup*(PopupButton::*)(void)) &PopupButton::popup, D(PopupButton, popup))
        .def("chevronIcon", &PopupButton::chevronIcon, D(PopupButton, chevronIcon))
        .def("setChevronIcon", &PopupButton::setChevronIcon, D(PopupButton, setChevronIcon))
        .def("side", &PopupButton::side, D(PopupButton, side))
        .def("setSide", &PopupButton::setSide, D(PopupButton, setSide));

    py::class_<ComboBox, PopupButton, ref<ComboBox>, PyComboBox>(m, "ComboBox", D(ComboBox))
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

    py::class_<CheckBox, FontWidget, ref<CheckBox>, PyCheckBox>(m, "CheckBox", D(CheckBox))
        .def(py::init<Widget *, const std::string &>(), py::arg("parent"),
             py::arg("caption") = std::string("Untitled"),
             D(CheckBox, CheckBox))
        .def(py::init<Widget *, const std::string &, const std::function<void(bool)>&, const std::string &>(),
             py::arg("parent"), py::arg("caption") = std::string("Untitled"), py::arg("callback"), py::arg("font") = std::string(""),
             D(CheckBox, CheckBox))
        .def("caption", &CheckBox::caption, D(CheckBox, caption))
        .def("setCaption", &CheckBox::setCaption, D(CheckBox, setCaption))
        .def("checked", &CheckBox::checked, D(CheckBox, checked))
        .def("setChecked", &CheckBox::setChecked, D(CheckBox, setChecked))
        .def("pushed", &CheckBox::pushed, D(CheckBox, pushed))
        .def("setPushed", &CheckBox::setPushed, D(CheckBox, setPushed))
        .def("callback", &CheckBox::callback, D(CheckBox, callback))
        .def("setCallback", &CheckBox::setCallback, D(CheckBox, setCallback));
}

#endif
