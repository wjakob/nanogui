#pragma once

#include <nanogui/widget.h>
#include <nanogui/common.h>
#include <map>
#include <string>

NAMESPACE_BEGIN(nanogui)

//! Adding the GUI Editor Workspace to an element allows you
/** to create, edit, load and save any elements supported
	by any loaded factories.
	When you add it without a parent (to the root element)
	it will also allow you to edit, load and save settings in
	the current skin.
*/

class FactoryView;

class NANOGUI_EXPORT EditorWorkspace : public Widget
{
public:
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

    EditorWorkspace( Widget* parent, std::string id );

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

  	//virtual void setMenuCommandIDStart(s32 id);

    void removeChild( Widget* child);

	  //! grid drawing...
    virtual void setGridSize(const Vector2i& gridSize);
	  virtual void setUseGrid(bool useGrid);

    bool keyboardEvent(int key, int scancode, int action, int modifiers) override;
    bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;
    bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

	  //! returns the first editable element under the mouse
    virtual Widget* getEditableElementFromPoint(Widget *start, const Vector2i &point);

	  //! selecting elements
    virtual void setSelectedElement(Widget *sel);
	  virtual void selectNextSibling();
  	virtual void selectPreviousSibling();

	  //! returns the selected element
    virtual Widget* getSelectedElement();

	  //! copies the xml of the selected element and all children to the clipboard
	  virtual void copySelectedElementToJson();

	  //! copies the xml of the selected element and all children to the clipboard
	  virtual void pasteJsonToSelectedElement();

    std::string wtypename() const override;

    virtual void save(Serializer& out ) const;
    virtual bool load(Serializer& in );

    void addChild(int index, Widget * widget) override;

    void setFactoryView( FactoryView* wnd );

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

    void setSelectedCallback(std::function<void(Widget*)> callback) { mWidgetSelectedCallback = callback; }

//slots:
    void toggleOptionsVisible();
    void activateChangeParentMode();
    void userChangeOptions();
	  void setGridVisible(bool drawGrid);
    void saveSelectedElementToJson();

private:
	
    EditMode getModeFromPos(const Vector2i &p);

    std::function<void(Widget*)> mWidgetSelectedCallback;
    
    //void _createEditorMenu();
    //void _createElementsWindow();
    void _drawSelectedElement(NVGcontext* ctx);
    void _drawResizePoints(NVGcontext* ctx);
    void _drawWidthRectangle(NVGcontext* ctx, Color& color, int width, const Vector4i& rectangle);
    void _drawResizePoint(NVGcontext* ctx, const Color& color, const Vector4i& rectangle);
    void _createElementsMap( Widget* start, std::map<std::string, Widget*>& mapa );
    EditMode	_currentMode;
	  EditMode	_mouseOverMode;
    Vector2i  _dragStart;
    Vector2i  _startMovePos;
	  Vector4i	_selectedArea;

    Vector2i	_gridSize;
    int			  _menuCommandStart;
	  bool			_drawGrid, _useGrid, _running;

    Widget* _elementUnderMouse = nullptr;
    Widget* _selectedElement = nullptr;
    Window* _previewWindow = nullptr;
    FactoryView* _factoryView = nullptr;
    Window* _optionsWindow;
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