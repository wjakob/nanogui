
#include <nanogui/editworkspace.h>
#include <nanogui/opengl.h>
#include <nanogui/messagedialog.h>
#include <nanovg.h>
#include <nanogui/screen.h>

NAMESPACE_BEGIN(nanogui)

static const std::string attrEditorName = "attributeEditor";
		
EditorWorkspace::EditorWorkspace(Widget* parent, std::string id )
    : Widget(parent),
	_currentMode(EditMode::Select), _mouseOverMode(EditMode::Select),
	_gridSize(10,10), _drawGrid( true ), _useGrid(true),
	_running( true )
{
  setId(id);
	// this element is never saved.
	setSubElement(true); 
  //_changesManager = new core::ChangesManager( "temp", *this );
}

EditorWorkspace::~EditorWorkspace()
{
}

Vector2i getUpperLeftCorner(const Vector4i& r)
{
  return Vector2i(r.x(), r.y());
}

EditorWorkspace::EditMode EditorWorkspace::getModeFromPos( const Vector2i& p )
{
    try
    {   
	    if (_selectedElement)
	    {
		    Vector4i	r = _selectedElement->absoluteRect();

		    if( isPointInsideRect( p, TLRect) )
			    return EditMode::ResizeTopLeft;
		    else if( isPointInsideRect( p, TRRect) )
			    return EditMode::ResizeTopRight;
		    else if( isPointInsideRect( p, BLRect) )
			    return EditMode::ResizeBottomLeft;
		    else if(isPointInsideRect( p, BRRect) )
			    return EditMode::ResizeBottpmRight;
		    else if( isPointInsideRect( p, TopRect) )
			    return EditMode::ResizeTop;
		    else if( isPointInsideRect( p, BRect) )
			    return EditMode::ResizeBottom;
		    else if( isPointInsideRect( p, LRect) )
			    return EditMode::ResizeLeft;
		    else if( isPointInsideRect( p, RRect) )
			    return EditMode::ResizeRight;
		    else if( getEditableElementFromPoint( _selectedElement, p ) == _selectedElement )
			    return EditMode::Move;
		    else
			    return EditMode::Select;
	    }

	    return EditMode::Select;
    }
    catch( ... )
    {
        setSelectedElement(nullptr);
        return EditMode::Select;
    }
}

Widget* EditorWorkspace::getEditableElementFromPoint(Widget* start, const Vector2i &point, int index )
{
    Widget* target = 0;

	// we have to search from back to front.

    auto rev_it = start->children().rbegin();
	  int count=0;
	  while(rev_it != start->children().rend())
	  {
		  target = getEditableElementFromPoint((*rev_it),point);
		  if (target)
		  {
			  if (!target->isSubElement() && isMyChildRecursive(target) && target != this)
			  {
				  if (index == count)
					  return target;
				  else
					  count++;
			  }
			  else
				  target = 0;
		  }
      ++rev_it;
	  }

	  if (isPointInsideRect(point, start->absoluteRect()))
		  target = start;

	  return target;
}

void EditorWorkspace::setSelectedElement(Widget *sel)
{
  Widget* focus = findWidget([](Widget* w) -> bool { return w->focused(); });
	// we only give focus back to children
	if (isMyChildRecursive(focus))
		focus = nullptr;

  bool needUpdateSelectedElm = false;
	if (_selectedElement != this)
	{
		if( _selectedElement != sel)// && _editorWindow )
		{
			//_editorWindow->setSelectedElement(sel);
			_selectedElement = sel;
      needUpdateSelectedElm = true;
		}
	}
  else
  {
    _selectedElement = nullptr;
    needUpdateSelectedElm = true;
  }

	if (focus)
     focus->requestFocus();
	else
     requestFocus();

  if (mWidgetSelectedCallback && needUpdateSelectedElm)
    mWidgetSelectedCallback(sel);
}

Widget* EditorWorkspace::getSelectedElement()
{
	return _selectedElement;
}

void EditorWorkspace::selectNextSibling()
{
  Widget* p=nullptr;

	if (_selectedElement && _selectedElement->parent())
		p = _selectedElement->parent();
	else
    p = parent();

  auto it = p->children().begin();
	// find selected element
	if (_selectedElement)
		while (*it != _selectedElement)
			++it;

	if (it !=p->children().end())
		++it;
	
  // find next non sub-element
	while (it != p->children().end() && (*it)->isSubElement())
		++it;

	if (it != p->children().end())
		setSelectedElement(*it);
}

void EditorWorkspace::selectPreviousSibling()
{
  Widget* p=0;

	if (_selectedElement && _selectedElement->parent())
		p = _selectedElement->parent();
	else
    p = parent();

  auto it = p->children().begin();
	// find selected element
  if (_selectedElement)
  {
    while (*it != _selectedElement)
      ++it;
  }

	if (it != p->children().end())
	  ++it;
	// find next non sub-element
	while (it != p->children().end() && (*it)->isSubElement())
		++it;

	if (it != p->children().end())
		setSelectedElement(*it);
}

void EditorWorkspace::_createElementsMap( Widget* start, std::map< std::string, Widget* >& mapa )
{
  auto pIter = start->children().begin();
  for( ; pIter != children().end(); pIter++ )
  {
    if( Widget* nrpElm = *pIter )
    {
      if( nrpElm->id().size() > 0 )
          mapa[ nrpElm->id() ] = *pIter;
    }

    if( (*pIter)->children().size() > 0 )
      _createElementsMap( *pIter, mapa );
  }
}

void EditorWorkspace::setElementName( Widget* elm, bool setText, std::string& wantName )
{
  std::string typeName = elm->wtypename();

  std::map< std::string, Widget* > elmMap;
  _createElementsMap( this, elmMap );

  std::string checkName = wantName;
  if( elmMap.find( checkName ) != elmMap.end() || !checkName.size() )
  {
    for( int index=0; ; index++ )
    {
      checkName = typeName + std::string( "_" ) + std::to_string( index );
      if( elmMap.find( checkName ) == elmMap.end() )
          break;
    }
  }

  if (elm)
    elm->setId( checkName );

  //if( setText )
  //    elm->setValue("text", checkName);
  wantName = checkName;
}

void EditorWorkspace::bringElementToFront( Widget* elm )
{
	elm->bringToFront();
}

bool EditorWorkspace::keyboardEvent(int key, int scancode, int action, int modifiers)
{
  if (action == GLFW_RELEASE) 
  {
    switch (key)
    {
    case GLFW_KEY_DELETE:
      if (_selectedElement)
      {
        _selectedElement->remove();
        setSelectedElement(nullptr);
        _elementUnderMouse = nullptr;
        //_editorWindow->updateTree(this);

        //if (_changesManager)
        //  _changesManager->Update();
      }
      break;

    case GLFW_KEY_X:
      if (modifiers == SYSTEM_COMMAND_MOD && _selectedElement)
      {
        // cut
        //copySelectedElementXML();
        // delete element
        _selectedElement->remove();
        setSelectedElement(nullptr);
        _elementUnderMouse = nullptr;

        //if (_changesManager)
        //  _changesManager->Update();
      }
      break;

    case GLFW_KEY_N:
      if (modifiers == SYSTEM_COMMAND_MOD)
        reset();
      break;

    case GLFW_KEY_B:
      if (modifiers == SYSTEM_COMMAND_MOD)
        bringElementToFront(_selectedElement);
      break;

    case GLFW_KEY_C:
      // copy
      if (modifiers == SYSTEM_COMMAND_MOD && _selectedElement)
      {
        //copySelectedElementJson();
      }
      break;

    case GLFW_KEY_R:
      if (modifiers == SYSTEM_COMMAND_MOD)
      {
        preview();
      }
      break;

    case GLFW_KEY_P:
      if (modifiers == SYSTEM_COMMAND_MOD && _selectedElement)
        setMode(EditMode::SelectNewParent);
      break;

    case GLFW_KEY_V:
      // paste
      if (modifiers == SYSTEM_COMMAND_MOD)
      {
        //pasteJsonToSelectedElement();

        //if (_changesManager)
        //  _changesManager->Update();
      }
      break;

    case GLFW_KEY_Z:
      if (modifiers & SYSTEM_COMMAND_MOD)
      {
        //if (modifiers & SYSTEM_COMMAND_SHIFT)
        //  _changesManager->redo();
        //else
        //  _changesManager->undo();
      }
      break;

    default:
      break;
    }

    return true;
  }

  return false;
}

bool EditorWorkspace::scrollEvent(const Vector2i &p, const Vector2f &rel)
{
  if (rel.y() > 0)
    selectPreviousSibling();
  else
    selectNextSibling();

  return true;
}

bool EditorWorkspace::mouseMotionEvent(const Vector2i &pp, const Vector2i &rel, int button, int modifiers)
{
  if (_currentMode == EditMode::Select || _currentMode == EditMode::SelectNewParent)
  {
    // highlight the element that the mouse is over
    _elementUnderMouse = getEditableElementFromPoint(this, pp);
    if (_elementUnderMouse == this)
    {
      _elementUnderMouse = nullptr;
    }

    if (_currentMode == EditMode::Select)
    {
      _mouseOverMode = getModeFromPos(pp);
      if (_mouseOverMode > EditMode::Move)
        _elementUnderMouse = _selectedElement;
    }
  }
  else if (_currentMode == EditMode::Move)
  {
    // get difference
    Vector2i p = pp - _dragStart;

    // apply to top corner
    p = _startMovePos + p;
    if (_useGrid)
    {
      p -= absolutePosition();

      p.x() = (p.x() / _gridSize.x())*_gridSize.x();
      p.y() = (p.y() / _gridSize.y())*_gridSize.y();

      p += absolutePosition();
    }

    Vector2i delta = p - getUpperLeftCorner(_selectedArea);
    _selectedArea.x() += delta.x();
    _selectedArea.y() += delta.y();
    _selectedArea.z() += delta.x();
    _selectedArea.w() += delta.y();

    if (_elementUnderMouse)
    {
      Vector2i elmpos = _elementUnderMouse->position();
      _elementUnderMouse->setPosition(elmpos + delta);
    }
  }
  else if (_currentMode > EditMode::Move)
  {
    // get difference from start position
    Vector2i p = pp;

    if (_useGrid)
    {
      p -= getUpperLeftCorner(absoluteRect());

      p.x() = (p.x() / _gridSize.x())*_gridSize.x();
      p.y() = (p.y() / _gridSize.y())*_gridSize.y();

      p += getUpperLeftCorner(absoluteRect());
    }

    switch (_currentMode)
    {
    case EditMode::ResizeTop:    _selectedArea.y() = p.y(); break;
    case EditMode::ResizeBottom: _selectedArea.w() = p.y(); break;
    case EditMode::ResizeLeft:   _selectedArea.x() = p.x(); break;
    case EditMode::ResizeRight:  _selectedArea.z() = p.x(); break;
    case EditMode::ResizeTopLeft:_selectedArea.x() = p.x(); _selectedArea.y() = p.y(); break;
    case EditMode::ResizeTopRight: _selectedArea.y() = p.y(); _selectedArea.z() = p.x(); break;
    case EditMode::ResizeBottomLeft: _selectedArea.x() = p.x(); _selectedArea.w() = p.y(); break;
    case EditMode::ResizeBottpmRight: _selectedArea.z() = p.x(); _selectedArea.w() = p.y(); break;
    default:
      break;
    }

    if (_elementUnderMouse)
    {
      Vector2i parpos = _elementUnderMouse->parent()->absolutePosition();
      Vector2i newpos = Vector2i(_selectedArea.x(), _selectedArea.y()) - parpos;
      Vector2i newsize = Vector2i(_selectedArea.z() - _selectedArea.x(), _selectedArea.w() - _selectedArea.y());
      _elementUnderMouse->setPosition(newpos);
      _elementUnderMouse->setFixedSize(newsize);
      _elementUnderMouse->setSize(newsize);
    }

    return true;
  }

  return false;
}

bool EditorWorkspace::mouseButtonEvent(const Vector2i &pp, int button, bool down, int modifiers)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && down)
  {
    Vector2i p = pp;
    Widget* newSelection = findWidget(p);

    if (newSelection != this && !isMyChildRecursive(newSelection)) // redirect event
    {
      newSelection->requestFocus();
      _selectedElement = newSelection;
      return true;
    }

    if (_currentMode == EditMode::Select)
    {
      if (_selectedElement)
      {
        // start moving or dragging
        _currentMode = getModeFromPos(p);

        if (_currentMode == EditMode::Move)
          _startMovePos = _selectedElement->absolutePosition();

        _dragStart = p;
        _selectedArea = _selectedElement->absoluteRect();
      }

      if (_currentMode < EditMode::Move)
      {
        // selecting an element...
        _elementUnderMouse = getEditableElementFromPoint(this, p);

        if (_elementUnderMouse == this)
          _elementUnderMouse = 0;

        setSelectedElement(_elementUnderMouse);
      }
    }
    return true;
  }
  else if (button == GLFW_MOUSE_BUTTON_RIGHT && down)
  {
    if (_currentMode == EditMode::SelectNewParent || _currentMode >= EditMode::Move)
    {
      // cancel dragging
      _currentMode = EditMode::Select;
    }
    return true;
  }
  else if (button == GLFW_MOUSE_BUTTON_LEFT && !down)
  {
    if (_currentMode == EditMode::SelectNewParent)
    {
      if (_selectedElement)
      {
        _elementUnderMouse = getEditableElementFromPoint(this, pp);
        if (_elementUnderMouse != _selectedElement)
        {
          _elementUnderMouse->addChild(_selectedElement);
          _selectedElement->setPosition(0, 0);

          setSelectedElement(nullptr);
        }

        //if (_changesManager)
        //  _changesManager->Update();
      }
      _currentMode = EditMode::Select;
    }
    else if (_currentMode >= EditMode::Move)
    {
      Widget *sel = _selectedElement;
      // unselect
      setSelectedElement(0);

      // move
      Vector2i p(0, 0);
      if (sel->parent())
        p = sel->parent()->absolutePosition();

      // select
      setSelectedElement(sel);
      _currentMode = EditMode::Select;

      //if (_changesManager)
      //  _changesManager->Update();
    }
    return true;
  } 

  return false;
}

//! called if an event happened.
/*bool EditorWorkspace::onEvent( const NEvent &e )
{
	switch(e.EventType)
	{
    /*case NRP_DRAGDROP_EVENT:
        if( e.DragDropEvent.EventType == NRP_DROP_ELEMENT && e.DragDropEvent.Element == _factoryView )
        {
            Point mousePos = e.MouseEvent.getPosition();
            Widget* parentElm = getEditableElementFromPoint(this, mousePos );
            
            s32 fIndex = _factoryView->GetFactoryIndex();
            s32 elmIndex = _factoryView->GetElementIndex();
            if(  fIndex >= 0 && elmIndex >= 0 )
            {
                AbstractWidgetsFactory* factory = WidgetsFactoriesManager::instance().getFactory( fIndex );
                String name = factory->getWidgetTypeName( elmIndex );

                // add it
                Widget *newElement = factory->createWidget( name, parentElm );
                if( newElement )
                {
                    Point p = mousePos - parentElm->getAbsoluteRect().UpperLeftCorner;
					newElement->setGeometry( RectI( p, core::NSizeU(100,100) ) );
                    
                    String checkName;
                    setElementName( newElement, true, checkName );
                    setSelectedElement(0);
                    setSelectedElement( newElement );    

                    if( _changesManager )
                        _changesManager->Update();
                }
            }
            
            getEnvironment()->setDragObject( NULL, Texture(), core::RectI( 0, 0, 0, 0 ) );
            _editorWindow->updateTree( this );
            return true;
        }
    break;
	// even if we didn't absorb the event,
	// we never pass events back to the GUI we're editing!
	return false;
}*/

void EditorWorkspace::preview()
{
    Vector2i scrSize = screen()->size();
    Widget* elm = children().size() > 0 ? ( children().front() ) : nullptr;
    if( !elm )
    {
      new MessageDialog(this, MessageDialog::Type::Information, "Warning", "Have no element to preview");
    }
    else
    {
		 /* _factoryView->EnabledEditorMode( false );
          _previewWindow = new Window( this, RectI( Point( 0, 0 ), scrSize ), L"Preview mode", -1 );

		  io::FilePath saveFileName( L"temp/forPreview.ui" );
          getEnvironment()->saveUI( saveFileName, elm );

          getEnvironment()->loadUI( saveFileName, _previewWindow );
          _previewWindow->setModal(); 
          _previewWindow->setNotClipped( true );
		  _previewWindow->setWindowFlag( Window::draggable, false );
		  _factoryView->EnabledEditorMode( true ); */
    }
}

//! draws the element and its children
void EditorWorkspace::draw(NVGcontext* ctx)
{
  nvgSave(ctx);

  nvgBeginPath(ctx);
  nvgRect(ctx, mPos.x(), mPos.y(), width(), height());
  nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 255));
  nvgStroke(ctx);

	if( _drawGrid )
	{
		// draw the grid
		int cy = _gridSize.x();
    nvgBeginPath(ctx);
    nvgStrokeColor(ctx, nvgRGBA(0xE0, 0xE0, 0xE0, 0x20));

		while (cy < height())
		{
      nvgMoveTo(ctx, mPos.x(), mPos.y() + cy);
      nvgLineTo(ctx, mPos.x() + width(), mPos.y() + cy);
      cy += _gridSize.y();
		}

    int cx = _gridSize.x();
    while (cx < width())
    {
      nvgMoveTo(ctx, mPos.x() + cx, mPos.y());
      nvgLineTo(ctx, mPos.x() + cx, mPos.y() + height());
      cx += _gridSize.x();
    }
    nvgStroke(ctx);
  }

  nvgRestore(ctx);

  Widget::draw(ctx);

  _drawSelectedElement(ctx);
  _drawResizePoints(ctx);
}

void EditorWorkspace::addChild(int index, Widget * widget)
{
  Widget::addChild(index, widget);
}

void EditorWorkspace::_drawWidthRectangle(NVGcontext* ctx, Color& color, int ww, const Vector4i& r)
{
  nvgBeginPath(ctx);
  nvgStrokeColor(ctx, color);
  nvgRect(ctx, r.x(), r.y(), r.z() - r.x(), r.w() - r.y());
  nvgStroke(ctx);
}

Vector2i getCenterRect(const Vector4i& r)
{
  return Vector2i((r.z() + r.x()) / 2, (r.w() + r.y()) / 2);
}

void EditorWorkspace::_drawResizePoint(NVGcontext* ctx, const Color& color, const Vector4i& tr)
{
  nvgBeginPath(ctx);
  nvgFillColor(ctx, color);
  Vector2i c = getCenterRect(tr);
  float r = (tr.z() - tr.x()) / 2;
  nvgCircle(ctx, c.x(), c.y(), r );
  //nvgRect(ctx, tr.x(), tr.y(), tr.z() - tr.x(), tr.w() - tr.y());
  nvgFill(ctx);

  nvgBeginPath(ctx);
  nvgStrokeColor(ctx, Color(0, 0, 0, 255));
  nvgCircle(ctx, c.x(), c.y(), r );
  //nvgRect(ctx, tr.x(), tr.y(), tr.z() - tr.x(), tr.w() - tr.y());
  nvgStroke(ctx);
}

void EditorWorkspace::_drawResizePoints(NVGcontext* ctx)
{
    if( _selectedElement )
    {
        // draw handles for moving
        EditMode m = _currentMode;
        Vector4i r = _selectedArea;
        if (m < EditMode::Move)
        {
            m = _mouseOverMode;
            r = _selectedElement->absoluteRect();
        }

        int dfHalf = 3;
        Vector2i c = getCenterRect(r);

        TLRect =  Vector4i(r.x() - dfHalf, r.y() - dfHalf, r.x() + dfHalf, r.y() + dfHalf);
        TRRect = Vector4i(r.z() - dfHalf, r.y() - dfHalf, r.z() + dfHalf, r.y() + dfHalf);
        TopRect = Vector4i(c.x() - dfHalf, r.y() - dfHalf, c.x() + dfHalf, r.y() + dfHalf);
        BLRect = Vector4i(r.x() - dfHalf, r.w() - dfHalf, r.x() + dfHalf, r.w() + dfHalf);
        LRect = Vector4i(r.x() - dfHalf, c.y() - dfHalf, r.x() + dfHalf, c.y() + dfHalf);
        RRect = Vector4i(r.z() - dfHalf, c.y() - dfHalf, r.z() + dfHalf, c.y() + dfHalf);
        BRRect = Vector4i(r.z() - dfHalf, r.w() - dfHalf, r.z() + dfHalf, r.w() + dfHalf);
        BRect = Vector4i(c.x() - dfHalf, r.w() - dfHalf, c.x() + dfHalf, r.w() + dfHalf);

        // top left
        Color bg(180,0, 0, 255);
        Color select(180, 255, 0, 255);
        bool mySide = (m == EditMode::ResizeTop || m == EditMode::ResizeLeft || m == EditMode::ResizeTopLeft);
        _drawResizePoint(ctx, mySide ? select : bg, TLRect );

        mySide = (m == EditMode::ResizeTop || m == EditMode::ResizeRight || m == EditMode::ResizeTopRight);
        _drawResizePoint(ctx, mySide ? select : bg, TRRect);

        mySide = (m == EditMode::ResizeTop);
        _drawResizePoint( ctx, mySide ? select : bg, TopRect);

        mySide = (m == EditMode::ResizeLeft || m == EditMode::ResizeBottomLeft || m == EditMode::ResizeBottom);
        _drawResizePoint( ctx, mySide ? select : bg, BLRect);

        mySide = (m == EditMode::ResizeLeft);
        _drawResizePoint( ctx, mySide ? select : bg, LRect);

        mySide = (m == EditMode::ResizeRight);
        _drawResizePoint( ctx, mySide ? select : bg, RRect);

        mySide = (m ==  EditMode::ResizeRight || m == EditMode::ResizeBottpmRight || m == EditMode::ResizeBottom);
        _drawResizePoint( ctx, mySide ? select : bg, BRRect );

        mySide = (m == EditMode::ResizeBottom);
        _drawResizePoint( ctx, mySide ? select : bg, BRect);
    }
}

void EditorWorkspace::_drawSelectedElement(NVGcontext* ctx)
{
  if (_elementUnderMouse &&
      _elementUnderMouse != _selectedElement &&
      _elementUnderMouse != parent() )
  {
    Vector4i r = _elementUnderMouse->absoluteRect();
    Color color(100,0,0,255);
    _drawWidthRectangle( ctx, color, 2, r );
  }

  if (_selectedElement)
  {
    if (_currentMode == EditMode::Select)
    {
      Color color(100, 0, 0, 255);
      _drawWidthRectangle(ctx, color, 2, _selectedElement->absoluteRect());
    }
    else if (_currentMode == EditMode::SelectNewParent)
    {
      float value = std::fmod((float)glfwGetTime(), 1.0f);
      Vector4i tr = _selectedElement->absoluteRect();
      Color color(0, 0, 255, value > 0.5 ? 32 : 64);
      nvgBeginPath(ctx);
      nvgFillColor(ctx, color);
      nvgRect(ctx, tr.x(), tr.y(), tr.z() - tr.x(), tr.w() - tr.y());
      nvgFill(ctx);
    }
  }

  if (_currentMode >= EditMode::Move)
  {
    const Vector4i& tr = _selectedArea;
    Color color(255,0,0,64);
    nvgBeginPath(ctx);
    nvgFillColor(ctx, color);
    nvgRect(ctx, tr.x(), tr.y(), tr.z() - tr.x(), tr.w() - tr.y());
    nvgFill(ctx);
  }
}

void EditorWorkspace::setGridVisible( bool visible )
{
	_drawGrid = visible;
}

void EditorWorkspace::setGridSize(const Vector2i& gridSize)
{
	_gridSize = gridSize;
	if (_gridSize.x() < 2)
		_gridSize.x() = 2;
	if (_gridSize.y()< 2)
		_gridSize.y() = 2;
}

void EditorWorkspace::setUseGrid(bool useGrid)
{
	_useGrid = useGrid;
}

void EditorWorkspace::copySelectedElementToJson()
{
	// create memory write file
	//io::ArrayMappedFile memWrite( L"#Clipboard#" );
	// save gui to mem file
	//getEnvironment()->saveUI( &memWrite, _selectedElement );

	// copy to clipboard- wide chars not supported yet :(
	//String text( memWrite.getData() );
	//getEnvironment()->getOSOperator()->copyToClipboard( text );
}

void EditorWorkspace::pasteJsonToSelectedElement()
{
	// get clipboard data
	//String XMLText = getEnvironment()->getOSOperator()->getTextFromClipboard();

	//io::ArrayMappedFile memFile("#Clipboard#");

	// write clipboard data
	//memFile.write( XMLText.toAscii().pointer(), XMLText.size() );

	// rewind file
	//memFile.seek(0, false);

	// read xml
	//getEnvironment()->loadUI( &memFile, _selectedElement);

	// reset focus
	//getEnvironment()->setFocus(this);
}

void EditorWorkspace::save(Serializer& out) const
{
	//out->AddBool( L"DrawGrid", drawGrid_);
	//out->AddBool( L"UseGrid", UseGrid);
	//out->AddPosition2d( L"GridSize", Point(GridSize.Width, GridSize.Height));
	//out->AddInt( L"MenuCommandStart", MenuCommandStart);
}

bool EditorWorkspace::load(Serializer& in )
{
	//setGridVisible(in->getAttributeAsBool(L"DrawGrid"));
	//setUseGrid(in->getAttributeAsBool(L"UseGrid"));

	//Point tmpp = in->getAttributeAsPosition2d(L"GridSize");
	//core::NSizeU tmpd(tmpp.X, tmpp.Y);
	//setGridSize(tmpd);
	//setMenuCommandIDStart(in->getAttributeAsInt( L"MenuCommandStart"));
  return true;
}

std::string EditorWorkspace::wtypename() const
{
    return "EditorWorkspace";
}

void EditorWorkspace::setFactoryView( FactoryView* wnd )
{   
    _factoryView = wnd;
}

void EditorWorkspace::reset()
{
    setSelectedElement(nullptr);
    _elementUnderMouse = 0;

    while( !children().empty() )
        removeChild( children().front() );
}

void EditorWorkspace::cutSelectedElement()
{
    copySelectedElementToJson();
    
    removeElement( _selectedElement );
}

void EditorWorkspace::removeElement( Widget* elm )
{
    Widget* saveElm = elm;
    setSelectedElement(nullptr);
    _elementUnderMouse = nullptr;
    saveElm->remove();
}

void EditorWorkspace::setMode( EditMode mode )
{
    _currentMode = mode;
}

void EditorWorkspace::updateTree()
{
    //_editorWindow->updateTree( this );
}

void EditorWorkspace::undo()
{
    //if( _changesManager )
      //  _changesManager->Undo();
}

void EditorWorkspace::redo()
{
    //if( _changesManager )
      //  _changesManager->Redo();
}

bool EditorWorkspace::isUndoEnabled() const
{
    //return _changesManager ? _changesManager->IsUndoEnabled() : false;
  return false;
}

bool EditorWorkspace::isRedoEnabled() const
{
    //return _changesManager ? _changesManager->IsRedoEnabled() : false;
  return false;
}

void EditorWorkspace::update()
{
    //if( _changesManager )
      //  _changesManager->Update();
}

void EditorWorkspace::removeChild( Widget* child )
{
    setSelectedElement( nullptr );
    _elementUnderMouse = nullptr;
 
    Widget::removeChild( child );
}

void EditorWorkspace::activateChangeParentMode()
{
    setMode( EditMode::SelectNewParent );
}

void EditorWorkspace::toggleOptionsVisible()
{
    /*if( _optionsWindow )
    {
        _optionsWindow->deleteLater();
        _optionsWindow = NULL;
        return;
    }

    _optionsWindow = new Window( NULL, RectI(0, 0, 0, 0), L"Workspace options", -1 );
    _optionsWindow->setRelativeRectProportional( RectF(0.3f, 0.25f, 0.6f, 0.75f) );
	  _optionsWindow->getButton( Window::buttonClose )->setVisible( false );

	  AttributeEditor* editor = (AttributeEditor*)WidgetsFactoriesManager::instance().createWidget( attrEditorName, _optionsWindow);
    editor->setID( attrEditorName.getHash() );
    editor->setRelativeRectProportional( RectF(0.0f, 0.1f, 1.0f, 1.0f));
    editor->setAlignment(alignUpperLeft, alignLowerRight, alignUpperLeft, alignLowerRight);

    save( editor->getAttribs() );
    editor->refreshAttribs();

    CONNECT( editor, onAttributeChanged(), this, EditWorkspace::userChangeOptions );
    */
}

void EditorWorkspace::saveSelectedElementToJson()
{
    Widget* elm = getSelectedElement();

    //getEnvironment()->saveUI( io::FilePath( "temp/testXml.ui" ), elm );
}

void EditorWorkspace::userChangeOptions()
{
    /*if( _optionsWindow )
    {
        if( AttributeEditor* editor = safety_cast<AttributeEditor*>( _optionsWindow->findChild( attrEditorName.getHash() ) ) )
            load( editor->getAttribs() );
    }*/
}

bool EditorWorkspace::isGridVisible() const
{
	return _drawGrid;
}

NAMESPACE_END(nanogui)
