#ifdef NANOGUI_PYTHON

#include "python.h"

typedef IntBox<int64_t> Int64Box;

void register_formhelper(py::module &m) {
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
}
#endif
