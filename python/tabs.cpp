#ifdef NANOGUI_PYTHON

#include "python.h"

DECLARE_WIDGET(StackedWidget);
DECLARE_WIDGET(TabHeader);
DECLARE_WIDGET(TabWidget);

void register_tabs(py::module &m) {
    py::class_<StackedWidget, Widget, ref<StackedWidget>, PyStackedWidget>(m, "StackedWidget", D(StackedWidget))
        .def(py::init<Widget *>(), D(StackedWidget, StackedWidget))
        .def("selectedIndex", &StackedWidget::selectedIndex, D(StackedWidget, selectedIndex))
        .def("setSelectedIndex", &StackedWidget::setSelectedIndex, D(StackedWidget, setSelectedIndex));

    py::class_<TabHeader, Widget, ref<TabHeader>, PyTabHeader>(m, "TabHeader", D(TabHeader))
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

    py::class_<TabWidget, Widget, ref<TabWidget>, PyTabWidget>(m, "TabWidget", D(TabWidget))
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
        .def("tab", (Widget * (TabWidget::*)(int)) &TabWidget::tab, D(TabWidget, tab))
        .def("ensureTabVisible", &TabWidget::ensureTabVisible, D(TabWidget, ensureTabVisible));
}

#endif
