#if !defined(__PYTHON_H)
#define __PYTHON_H

#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/dial.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/meter.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/colorpicker.h>
#include <nanogui/table.h>
#include <nanogui/graph.h>
#include <nanogui/ledmatrix.h>
#include <nanogui/tabwidget.h>
#include <nanogui/switchbox.h>
#include <nanogui/spinner.h>
#include <nanogui/dropdownbox.h>
#include <nanogui/editworkspace.h>
#include <nanogui/editproperties.h>
#include <nanogui/scrollbar.h>
#include <nanogui/windowmenu.h>
#include <nanogui/perfchart.h>
#include <nanogui/common.h>
#include <nanogui/listbox.h>
#include <nanogui/themebuilder.h>
#include <nanogui/tolerancebar.h>
#include <nanogui/treeview.h>
#include <nanogui/treeviewitem.h>
#include <nanogui/picflow.h>
#include <nanogui/textarea.h>
#include <nanogui/searchbox.h>
#include <nanogui/editproperties.h>
#include <nanogui/formhelper.h>

#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include <nanogui/python.h>
#include "py_doc.h"

PYBIND11_MAKE_OPAQUE(nanogui::Color)

#define D(...) DOC(nanogui, __VA_ARGS__ )

#define DECLARE_LAYOUT(Name) \
    class Py##Name : public Name { \
    public: \
        using Name::Name; \
        NANOGUI_LAYOUT_OVERLOADS(Name); \
    }

#define DECLARE_WIDGET(Name) \
    class Py##Name : public Name { \
    public: \
        using Name::Name; \
        NANOGUI_WIDGET_OVERLOADS(Name); \
    }

#define DECLARE_SCREEN(Name) \
    class Py##Name : public Name { \
    public: \
        using Name::Name; \
        NANOGUI_WIDGET_OVERLOADS(Name); \
        NANOGUI_SCREEN_OVERLOADS(Name); \
    }

namespace py = pybind11;
using namespace nanogui;

/// Make pybind aware of the ref-counted wrapper type
PYBIND11_DECLARE_HOLDER_TYPE(T, ref<T>);

#endif /* __PYTHON_H */

