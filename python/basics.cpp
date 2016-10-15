#ifdef NANOGUI_PYTHON

#include "python.h"

DECLARE_WIDGET(Label);
DECLARE_WIDGET(Popup);
DECLARE_WIDGET(MessageDialog);
DECLARE_WIDGET(VScrollPanel);
DECLARE_WIDGET(ComboBox);
DECLARE_WIDGET(ProgressBar);
DECLARE_WIDGET(Slider);

void register_basics(py::module &m) {
    py::class_<Label, Widget, ref<Label>, PyLabel>(m, "Label", D(Label))
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


    py::class_<Popup, Window, ref<Popup>, PyPopup>(m, "Popup", D(Popup))
        .def(py::init<Widget *, Window *>(), py::arg("parent"), py::arg("parentWindow"), D(Popup, Popup))
        .def("anchorPos", &Popup::anchorPos, D(Popup, anchorPos))
        .def("setAnchorPos", &Popup::setAnchorPos, D(Popup, setAnchorPos))
        .def("anchorHeight", &Popup::anchorHeight, D(Popup, anchorHeight))
        .def("setAnchorHeight", &Popup::setAnchorHeight, D(Popup, setAnchorHeight))
        .def("parentWindow", (Window*(Popup::*)(void)) &Popup::parentWindow, D(Popup, parentWindow));

    py::class_<MessageDialog, Window, ref<MessageDialog>, PyMessageDialog> mdlg(m, "MessageDialog", D(MessageDialog));
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

    py::class_<VScrollPanel, Widget, ref<VScrollPanel>, PyVScrollPanel>(m, "VScrollPanel", D(VScrollPanel))
        .def(py::init<Widget *>(), py::arg("parent"), D(VScrollPanel, VScrollPanel));

    py::class_<ComboBox, Widget, ref<ComboBox>, PyComboBox>(m, "ComboBox", D(ComboBox))
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

    py::class_<ProgressBar, Widget, ref<ProgressBar>, PyProgressBar>(m, "ProgressBar", D(ProgressBar))
        .def(py::init<Widget *>(), py::arg("parent"), D(ProgressBar, ProgressBar))
        .def("value", &ProgressBar::value, D(ProgressBar, value))
        .def("setValue", &ProgressBar::setValue, D(ProgressBar, setValue));

    py::class_<Slider, Widget, ref<Slider>, PySlider>(m, "Slider", D(Slider))
        .def(py::init<Widget *>(), py::arg("parent"), D(Slider, Slider))
        .def("value", &Slider::value, D(Slider, value))
        .def("setValue", &Slider::setValue, D(Slider, setValue))
        .def("highlightColor", &Slider::highlightColor, D(Slider, highlightColor))
        .def("setHighlightColor", &Slider::setHighlightColor, D(Slider, setHighlightColor))
        .def("range", &Slider::range, D(Slider, range))
        .def("setRange", &Slider::setRange, D(Slider, setRange))
        .def("highlightedRange", &Slider::highlightedRange, D(Slider, highlightedRange))
        .def("setHighlightedRange", &Slider::setHighlightedRange, D(Slider, setHighlightedRange))
        .def("setCallback", &Slider::setCallback, D(Slider, setCallback))
        .def("callback", &Slider::callback, D(Slider, callback))
        .def("setFinalCallback", &Slider::setFinalCallback, D(Slider, setFinalCallback))
        .def("finalCallback", &Slider::finalCallback, D(Slider, finalCallback));
}
#endif
