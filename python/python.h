#if !defined(__PYTHON_H)
#define __PYTHON_H

#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include "py_doc.h"

#define D(...) DOC(nanogui, __VA_ARGS__ )

#define WIDGET_OVERLOADS(Parent) \
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) { \
        PYBIND11_OVERLOAD(bool, Parent, mouseButtonEvent, p, button, down, modifiers); \
    } \
    bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) { \
        PYBIND11_OVERLOAD(bool, Parent, mouseMotionEvent, p, rel, button, modifiers); \
    } \
    bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) { \
        PYBIND11_OVERLOAD(bool, Parent, mouseDragEvent, p, rel, button, modifiers); \
    } \
    bool mouseEnterEvent(const Vector2i &p, bool enter) { \
        PYBIND11_OVERLOAD(bool, Parent, mouseEnterEvent, p, enter); \
    } \
    bool scrollEvent(const Vector2i &p, const Vector2f &rel) { \
        PYBIND11_OVERLOAD(bool, Parent, scrollEvent, p, rel); \
    } \
    bool focusEvent(bool focused) { \
        PYBIND11_OVERLOAD(bool, Parent, focusEvent, focused); \
    } \
    bool keyboardEvent(int key, int scancode, int action, int modifiers) { \
        PYBIND11_OVERLOAD(bool, Parent, keyboardEvent, key, scancode, action, modifiers); \
    } \
    bool keyboardCharacterEvent(unsigned int codepoint) { \
        PYBIND11_OVERLOAD(bool, Parent, keyboardCharacterEvent, codepoint); \
    } \
    Vector2i preferredSize(NVGcontext *ctx) const { \
        PYBIND11_OVERLOAD(Vector2i, Parent, preferredSize, ctx); \
    } \
    void performLayout(NVGcontext *ctx) { \
        PYBIND11_OVERLOAD(void, Parent, performLayout, ctx); \
    } \
    void draw(NVGcontext *ctx) { \
        PYBIND11_OVERLOAD(void, Parent, draw, ctx); \
    }

#define LAYOUT_OVERLOADS(Parent) \
    Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const { \
        PYBIND11_OVERLOAD(Vector2i, Parent, preferredSize, ctx, widget); \
    } \
    void performLayout(NVGcontext *ctx, Widget *widget) const { \
        PYBIND11_OVERLOAD(void, Parent, performLayout, ctx, widget); \
    }

#define SCREEN_OVERLOADS(Parent) \
    virtual void drawAll() { \
        PYBIND11_OVERLOAD(void, Parent, drawAll); \
    } \
    virtual void drawContents() { \
        PYBIND11_OVERLOAD(void, Parent, drawContents); \
    } \
    virtual bool dropEvent(const std::vector<std::string> &filenames) { \
        PYBIND11_OVERLOAD(bool, Parent, dropEvent, filenames); \
    } \
    virtual bool resizeEvent(int width, int height) { \
        PYBIND11_OVERLOAD(bool, Parent, resizeEvent, width, height); \
    }

#define DECLARE_LAYOUT(Name) \
    class Py##Name : public Name { \
    public: \
        using Name::Name; \
        LAYOUT_OVERLOADS(Name); \
    }

#define DECLARE_WIDGET(Name) \
    class Py##Name : public Name { \
    public: \
        using Name::Name; \
        WIDGET_OVERLOADS(Name); \
    }

#define DECLARE_SCREEN(Name) \
    class Py##Name : public Name { \
    public: \
        using Name::Name; \
        WIDGET_OVERLOADS(Name); \
        SCREEN_OVERLOADS(Name); \
    }

#endif /* __PYTHON_H */
