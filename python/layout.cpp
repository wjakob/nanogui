#ifdef NANOGUI_PYTHON

#include "python.h"

DECLARE_LAYOUT(Layout);
DECLARE_LAYOUT(GroupLayout);
DECLARE_LAYOUT(BoxLayout);
DECLARE_LAYOUT(GridLayout);
DECLARE_LAYOUT(AdvancedGridLayout);

void register_layout(py::module &m) {
    py::class_<Layout, ref<Layout>, PyLayout> layout(m, "Layout", D(Layout));
    layout
        .def("preferredSize", &Layout::preferredSize, D(Layout, preferredSize))
        .def("performLayout", &Layout::performLayout, D(Layout, performLayout));

    py::class_<BoxLayout, Layout, ref<BoxLayout>, PyBoxLayout>(m, "BoxLayout", D(BoxLayout))
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

    py::class_<GroupLayout, Layout, ref<GroupLayout>, PyGroupLayout>(m, "GroupLayout", D(GroupLayout))
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

    py::class_<GridLayout, Layout, ref<GridLayout>, PyGridLayout>(m, "GridLayout", D(GridLayout))
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

    py::class_<AdvancedGridLayout, Layout, ref<AdvancedGridLayout>, PyAdvancedGridLayout> advGridLayout(
        m, "AdvancedGridLayout", D(AdvancedGridLayout));

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
}

#endif
