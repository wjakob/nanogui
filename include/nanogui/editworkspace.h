#pragma once

#include <nanogui/widget.h>
#include <nanogui/common.h>
#include <map>
#include <set>
#include <string>

NAMESPACE_BEGIN(nanogui)

//! Adding the GUI Editor Workspace to an element allows you
/** to create, edit, load and save any elements supported
  by any loaded factories.
  When you add it without a parent (to the root element)
  it will also allow you to edit, load and save settings in
  the current skin.
*/

class NANOGUI_EXPORT EditorWorkspace : public Widget
{
public:
    RTTI_CLASS_UID("EDWS")
    RTTI_DECLARE_INFO(EditorWorkspace)

    enum EditMode
    {
        // when we are currently selecting an element
        Select=0,
        // selecting a new parent for the selected element
        SelectNewParent,

        // moving the selected element
        Move,
        // resizing the selected element
        ResizeTopLeft,
        ResizeTop,
        ResizeTopRight,
        ResizeRight,
        ResizeBottpmRight,
        ResizeBottom,
        ResizeBottomLeft,
        ResizeLeft
    };

    EditorWorkspace( Widget* parent, const std::string& id);

    using Widget::set;
    template<typename... Args>
    EditorWorkspace(Widget* parent, const Args&... args)
      : EditorWorkspace(parent, std::string("")) { set<EditorWorkspace, Args...>(args...); }

    virtual ~EditorWorkspace();

    //! called if an event happened.
    //virtual bool onEvent(const NEvent &event);

    //! draws the element and its children
    void draw(NVGcontext* ctx) override;

    bool isRunning() const { return _running; }

    void reset();

    bool isGridVisible() const;

    void cutSelectedElement();

    void setMode( EditMode mode );

    void performLayout(NVGcontext *ctx) override;
    void setWidgetEditable(intptr_t ptr, bool canEdit);
    bool isWidgetEditable(intptr_t ptr);

    void removeChild(const Widget* child) override;

    //! grid drawing...
    virtual void setGridSize(const Vector2i& gridSize);
    virtual void setUseGrid(bool useGrid);

    bool keyboardEvent(int key, int scancode, int action, int modifiers) override;
    bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;
    bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

    //! returns the first editable element under the mouse
    virtual Widget* getEditableElementFromPoint(Widget *start, const Vector2i &point);
    void setSelectedElement(Widget *elm);
    void setHoveredElement(Widget *elm);

    void selectNextSibling();
    void selectPreviousSibling();

    Widget* getSelectedElement();

    void copySelectedElementToJson();
    void pasteJsonToSelectedElement();

    std::string wtypename() const override;

    virtual void save(Serializer& out ) const;
    virtual bool load(Serializer& in );

    void addChild(int index, Widget * widget) override;

    void quit() { _running = false; }

    void bringElementToFront( Widget* elm );
    void removeElement( Widget* elm );

    void updateTree();

    void setElementName( Widget* elm, bool setText, std::string& wantName );

    void undo();
    bool isUndoEnabled() const;

    void redo();
    bool isRedoEnabled() const;

    void update();
    void preview();

    using WidgetCallback = std::function<void(Widget*)>;
    void addSelectedCallback(WidgetCallback callback) { mSelectedCallbacks.push_back(callback); }
    void setChildrenChangeCallback(std::function<void()> callback) { mChildrenChangeCallback = callback; }
    void setHoveredCallback(WidgetCallback callback) { mWidgetHoveredCallback = callback; }

    void toggleOptionsVisible();
    void activateChangeParentMode();
    void userChangeOptions();
    void setGridVisible(bool drawGrid);
    void saveSelectedElementToJson();

    void prepareCreateWidget(const std::string& wtypename);

private:
    EditMode getModeFromPos(const Vector2i &p);


    std::vector<WidgetCallback> mSelectedCallbacks;
    std::function<void(Widget*)> mWidgetHoveredCallback;
    std::function<void()> mChildrenChangeCallback;

    void _drawSelectedElement(NVGcontext* ctx);
    void _drawResizePoints(NVGcontext* ctx);
    void _drawNextWidget(NVGcontext* ctx);
    void _drawWidthRectangle(NVGcontext* ctx, Color& color, int width, const Vector4i& rectangle);
    void _drawResizePoint(NVGcontext* ctx, const Color& color, const Vector4i& rectangle);
    void _createElementsMap( Widget* start, std::map<std::string, Widget*>& mapa );
    void _sendSelectElementChangedEvent();
    void _sendHoveredElementChangedEvent();

    std::set<intptr_t> nonEditableElms;

    EditMode  _currentMode;
    EditMode  _mouseOverMode;
    Vector2i  _dragStart;
    Vector2i  _startMovePos;
    Vector4i  _selectedArea;

    Vector2i  _gridSize;
    int       _menuCommandStart;
    bool      _drawGrid, _useGrid, _running;
    Vector2i  mLastMousePos;

    Widget* mElementUnderMouse = nullptr;
    Widget* mSelectedElement = nullptr;
    Window* mOptionsWindow = nullptr;
    Widget* mNextWidget = nullptr;
    //ChangesManager* _changesManager;

    struct {
      Vector4i topleft;
      Vector4i topright;
      Vector4i top;
      Vector4i bottomleft;
      Vector4i left;
      Vector4i right;
      Vector4i bottomright;
      Vector4i bottom;
    } editArea;
};

NAMESPACE_END(nanogui)