#ifdef NANOGUI_PYTHON

#include "python.h"

typedef FloatBox<double> DoubleBox;
typedef IntBox<int64_t> Int64Box;

DECLARE_WIDGET(TextBox);
DECLARE_WIDGET(DoubleBox);
DECLARE_WIDGET(Int64Box);

void register_textbox(py::module &m) {
    py::class_<TextBox, FontWidget, ref<TextBox>, PyTextBox> tbox(m, "TextBox", D(TextBox));
    tbox
        .def(py::init<Widget *, const std::string &, const std::string &>(), py::arg("parent"),
            py::arg("value") = std::string("Untitled"), py::arg("font") = std::string(""), D(TextBox, TextBox))
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
        .def("placeholder", &TextBox::placeholder, D(TextBox, placeholder))
        .def("setPlaceholder", &TextBox::setPlaceholder, D(TextBox, setPlaceholder))
        .def("callback", &TextBox::callback, D(TextBox, callback))
        .def("setCallback", &TextBox::setCallback, D(TextBox, setCallback));

    py::enum_<TextBox::Alignment>(tbox, "Alignment", D(TextBox, Alignment))
        .value("Left", TextBox::Alignment::Left)
        .value("Center", TextBox::Alignment::Center)
        .value("Right", TextBox::Alignment::Right);

    py::class_<Int64Box, TextBox, ref<Int64Box>, PyInt64Box>(m, "IntBox", D(IntBox))
        .def(py::init<Widget *, int64_t>(), py::arg("parent"), py::arg("value") = (int64_t) 0, D(IntBox, IntBox))
        .def("value", &Int64Box::value, D(IntBox, value))
        .def("setValue", (void (Int64Box::*)(int64_t)) &Int64Box::setValue, D(IntBox, setValue))
        .def("setCallback", (void (Int64Box::*)(const std::function<void(int64_t)>&))
                &Int64Box::setCallback, D(IntBox, setCallback))
        .def("setValueIncrement", &Int64Box::setValueIncrement, D(IntBox, setValueIncrement))
        .def("setMinValue", &Int64Box::setMinValue, D(IntBox, setMinValue))
        .def("setMaxValue", &Int64Box::setMaxValue, D(IntBox, setMaxValue))
        .def("setMinValue", &Int64Box::setMinMaxValues, D(IntBox, setMinMaxValues));

    py::class_<DoubleBox, TextBox, ref<DoubleBox>, PyDoubleBox>(m, "FloatBox", D(FloatBox))
        .def(py::init<Widget *, double>(), py::arg("parent"), py::arg("value") = 0.0)
        .def("value", &DoubleBox::value, D(FloatBox, value))
        .def("setValue", (void (DoubleBox::*)(double)) &DoubleBox::setValue, D(FloatBox, setValue))
        .def("setCallback", (void (DoubleBox::*)(const std::function<void(double)>&))
                &DoubleBox::setCallback, D(FloatBox, setCallback))
        .def("setValueIncrement", &DoubleBox::setValueIncrement, D(FloatBox, setValueIncrement))
        .def("setMinValue", &DoubleBox::setMinValue, D(FloatBox, setMinValue))
        .def("setMaxValue", &DoubleBox::setMaxValue, D(FloatBox, setMaxValue))
        .def("setMinValue", &DoubleBox::setMinMaxValues, D(FloatBox, setMinMaxValues));
}

#endif
