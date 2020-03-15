/*
NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
The widget drawing code is based on the NanoVG demo application
by Mikko Mononen.
Table Widget was developed by dalerank <dalerankn8@gmail.com>

All rights reserved. Use of this source code is governed by a
BSD-style license that can be found in the LICENSE.txt file.
*/


#include <nanogui/table.h>
#include <nanogui/textbox.h>
#include <nanogui/label.h>
#include <nanogui/scrollbar.h>
#include <nanogui/screen.h>
#include <nanovg.h>

#define ARROW_PAD 15
#define DEFAULT_SCROLLBAR_SIZE 16

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Table, Widget)

const std::string TableOrderingModeNames[] =
{
  "none",
  "ascending",
  "descending",
  ""
};

const std::string TableColumnOrderingNames[] =
{
  "none",
  "custom",
  "ascend",
  "descend",
  "ascend_descend",
  ""
};

class Table::Cell : public Label
{
public:
  Cell(Widget* parent, const Vector4i& r) : Label(parent,"")
  {
    setPosition(r.x(), r.y());
    setSize(r.z() - r.x(), r.w() - r.y());

    element = nullptr;
    data = 0;
  }

  void draw(NVGcontext* ctx) override
  {
    if (inEditMode)
      Widget::draw(ctx);
    else
      Label::draw(ctx);
  }

  Widget* element;
  bool inEditMode = false;
  uintptr_t data;
};

class Table::HidingElement : public Widget
{
public:
  HidingElement( Widget* parent, const Vector4i& r )
      : Widget(parent)
  {
    setPosition(r.x(), r.y());
    setSize(r.z() - r.x(), r.w() - r.y());
  }

  bool isPointInside(const Vector2i&) const
  {
      return false;
  }
};

class Table::Column : public Label
{
public:
    Column( Widget* parent, const Vector4i& r )
        : Label( parent, "" ), orderingMode(Table::ColumnOrder::coNone)
    {
      setPosition(r.x(), r.y());
      setSize(r.z() - r.x(), r.w() - r.y());
      setTextHAlign(TextHAlign::hCenter);
    }

    Table::ColumnOrder orderingMode;

    bool isPointInside(const Vector2i&) const
    {
        return false;
    }
};

Table::Table( Widget* parent, const std::string& id,
              const Vector4i& r, bool clip,
              bool drawBack, bool moveOverSelect)
: Widget(parent),
  _clip(clip), _moveOverSelect(moveOverSelect),
  _selecting(false), _currentResizedColumn(-1), _resizeStart(0), _resizableColumns(true),
  _itemHeight(0), _overItemHeight(0), _totalItemHeight(0), _totalItemWidth(0),
  _selectedRow(-1), _selectedColumn(-1),
  _editedRow(-1), _editedColumn(-1),
  _cellHeightPadding(2), _cellWidthPadding(5), _activeTab(-1),
  _currentOrdering( RowOrder::roNone )
{
  setPosition(r.x(), r.y());
  setSize(r.z() - r.x(), r.w() - r.y());

  setDrawFlag( drawBorder );
  setDrawFlag( drawRows );
  setDrawFlag( drawColumns );
  setDrawFlag( drawActiveRow );

  _cellLastTimeClick = 0;
  _header = new HidingElement( this, Vector4i( 0, 0, width(), DEFAULT_SCROLLBAR_SIZE ) );
  //_header->setAlignment( alignUpperLeft, alignLowerRight, alignUpperLeft, alignUpperLeft );
  _header->setSubElement( true );

  Widget* iaparent = new HidingElement(this, Vector4i(0, DEFAULT_SCROLLBAR_SIZE, width(), height()));
  _itemsArea = new HidingElement(iaparent, Vector4i( 0, 0, 1, 1 ) );
  //_itemsArea->setAlignment( alignUpperLeft, alignLowerRight, alignUpperLeft, alignLowerRight );
  _itemsArea->setSubElement( true );

  _verticalScrollBar = new ScrollBar(this, ScrollBar::VerticalRight);
  //_verticalScrollBar->grab();
  //_verticalScrollBar->setNotClipped(false);
  _verticalScrollBar->setSubElement(true);
  //_verticalScrollBar->setVisibleFilledArea( false );
  //_verticalScrollBar->setAlignment( alignLowerRight, alignLowerRight, alignUpperLeft, alignLowerRight );

  _horizontalScrollBar = new ScrollBar( this, ScrollBar::HorizontalBottom );
  //_horizontalScrollBar->grab();
  //_horizontalScrollBar->setNotClipped(false);
  _horizontalScrollBar->setSubElement(true);
  //_horizontalScrollBar->setVisibleFilledArea( false );
  //_horizontalScrollBar->setAlignment( alignUpperLeft, alignLowerRight, alignLowerRight, alignLowerRight );

  _recalculateHeights();
  _refreshControls();
}

Table::~Table() {}

void Table::addColumn(const std::string& caption, uint32_t columnIndex)
{
  Column* columnHeader = new Column( _header, Vector4i( 0, 0, 1, 1 ) );
  columnHeader->setSubElement( true );
  columnHeader->setCaption( caption );
  columnHeader->setPosition(0, 0);
  float tw = nvgTextBounds(screen()->nvgContext(), 0, 0, caption.c_str(), nullptr, nullptr);
  columnHeader->setSize(tw + (_cellWidthPadding * 2) + ARROW_PAD, _header->height());
  columnHeader->setFixedSize(tw + (_cellWidthPadding * 2) + ARROW_PAD, _header->height());

  columnHeader->orderingMode = ColumnOrder::coNone;

  if ( columnIndex >= _columns.size() )
  {
    _columns.push_back( columnHeader );
    for (Row& row: _rows)
      row.items.push_back( new Cell( _itemsArea, Vector4i( 0, 0, 1, 1 ) ) );
  }
  else
  {
    _columns.insert(_columns.begin() + columnIndex, columnHeader);
    for (Row& row: _rows)
      row.items.insert(row.items.begin() + columnIndex, new Cell( _itemsArea, Vector4i(0, 0, 1, 1) ));
  }

  if (_activeTab == -1)
    _activeTab = 0;

  _recalculateColumnsWidth();
  _recalculateCells();
  _recalculateScrollBars();
}

//! remove a column from the table
void Table::removeColumn(uint32_t columnIndex)
{
  if ( columnIndex < _columns.size() )
  {
    _columns.erase( _columns.begin() + columnIndex );
    for (Row& row: _rows)
    {
      row.items[ columnIndex ]->removeLater();
      row.items.erase(row.items.begin() + columnIndex );
    }
  }

  if ( (int)columnIndex <= _activeTab )
    _activeTab = _columns.size() ? 0 : -1;

  _recalculateColumnsWidth();
}


int Table::getColumnCount() const {  return _columns.size(); }
int Table::getRowCount() const { return _rows.size(); }

bool Table::setActiveColumn(int idx, bool doOrder )
{
  if (idx < 0 || idx >= (int)_columns.size())
    return false;

  bool changed = (_activeTab != idx);

  _activeTab = idx;
  if ( _activeTab < 0 )
    return false;

  if ( doOrder )
  {
    switch ( _columns[idx]->orderingMode )
    {
    case ColumnOrder::coNone:
        _currentOrdering = RowOrder::roNone;
        break;

    case ColumnOrder::coCustom:
        _currentOrdering = RowOrder::roNone;
        break;

      case ColumnOrder::coAscending:
        _currentOrdering = RowOrder::roAscending;
        break;

      case ColumnOrder::coDescending:
        _currentOrdering = RowOrder::roDescending;
        break;

      case ColumnOrder::coAscendingDescending:
        _currentOrdering = (ColumnOrder::coAscending == _currentOrdering ? RowOrder::roDescending : RowOrder::roAscending);
        break;
      default:
        _currentOrdering = RowOrder::roNone;
    }

    orderRows( getActiveColumn(), _currentOrdering );
  }

  return true;
}

int Table::getActiveColumn() const { return _activeTab; }
Table::RowOrder Table::getActiveColumnOrdering() const {return _currentOrdering;}

void Table::setColumnWidth(uint32_t columnIndex, uint32_t width)
{
  if ( columnIndex < _columns.size() )
  {
    int tw = nvgTextBounds(screen()->nvgContext(), 0, 0, _columns[columnIndex]->caption().c_str(), nullptr, nullptr );
    const uint32_t MIN_WIDTH = tw + (_cellWidthPadding * 2);
    if ( width < MIN_WIDTH )
      width = MIN_WIDTH;

    _columns[columnIndex]->setWidth(width);
    _columns[columnIndex]->setFixedWidth(width);
  }

  _recalculateColumnsWidth();
  _recalculateCells();
  _recalculateScrollBars();
}

//! Get the width of a column
uint32_t Table::getColumnWidth(uint32_t columnIndex) const
{
  if ( columnIndex >= _columns.size() )
    return 0;

  return _columns[columnIndex]->width();
}

void Table::setResizableColumns(bool resizable) {  _resizableColumns = resizable; }
bool Table::hasResizableColumns() const {  return _resizableColumns; }

uint32_t Table::addRow(uint32_t rowIndex)
{
  if ( rowIndex > _rows.size() )
    rowIndex = _rows.size();

  Row row;

  if ( rowIndex == _rows.size() )
    _rows.push_back( row );
  else
    _rows.insert(_rows.begin() + rowIndex, row);

  _rows[rowIndex].items.resize( _columns.size() );

  for ( uint32_t i = 0 ; i < _columns.size() ; ++i )
    _rows[rowIndex].items[ i ] = nullptr;

  for ( uint32_t i = 0 ; i < _columns.size() ; ++i )
    _rows[rowIndex].items[ i ] = new Cell( _itemsArea, Vector4i( 0, 0, 1, 1 ) );

  _recalculateHeights();
  _recalculateCells();
  _recalculateScrollBars();
  return rowIndex;
}


void Table::removeRow(uint32_t rowIndex)
{
  if ( rowIndex > _rows.size() )
    return;

  for ( uint32_t colNum=0; colNum < _columns.size(); colNum++ )
    removeCellElement( rowIndex, colNum );

  _rows.erase(_rows.begin() + rowIndex );

  if ( !(_selectedRow < int(_rows.size())) )
    _selectedRow = _rows.size() - 1;

  _recalculateHeights();
  _recalculateScrollBars();
}

//! adds an list item, returns id of item
void Table::setCellText(uint32_t rowIndex, uint32_t columnIndex, const std::string& text)
{
  if ( rowIndex < _rows.size() && columnIndex < _columns.size() )
  {
    _rows[rowIndex].items[columnIndex]->setCaption( text );
  }
}

void Table::setCellText(uint32_t rowIndex, uint32_t columnIndex, const std::string& text, const Color& color)
{
  if ( rowIndex < _rows.size() && columnIndex < _columns.size() )
  {
    _rows[rowIndex].items[columnIndex]->setCaption( text );
    _rows[rowIndex].items[columnIndex]->setColor( color );
  }
}

void Table::setCellTextColor(uint32_t rowIndex, uint32_t columnIndex, const Color& color)
{
  if ( rowIndex < _rows.size() && columnIndex < _columns.size() )
    _rows[rowIndex].items[columnIndex]->setColor( color );
}

void Table::setCellDataptr(uint32_t rowIndex, uint32_t columnIndex, uintptr_t data)
{
  if ( rowIndex < _rows.size() && columnIndex < _columns.size() )
    _rows[rowIndex].items[columnIndex]->data = data;
}

std::string Table::getCellText(uint32_t rowIndex, uint32_t columnIndex ) const
{
  if ( rowIndex < _rows.size() && columnIndex < _columns.size() )
    return _rows[rowIndex].items[columnIndex]->caption();

  return "";
}

uintptr_t Table::getCellDataptr(uint32_t rowIndex, uint32_t columnIndex ) const
{
  if ( rowIndex < _rows.size() && columnIndex < _columns.size() )
  {
    return _rows[rowIndex].items[columnIndex]->data;
  }

  return 0;
}

void Table::clear()
{
  _selectedRow = -1;
  clearRows();

  for (auto& col: _columns)
    col->remove();

  _rows.clear();
  _columns.clear();

  if (_verticalScrollBar) _verticalScrollBar->setScroll(0);
  if ( _horizontalScrollBar )  _horizontalScrollBar->setScroll(0);

  _recalculateHeights();
  _recalculateColumnsWidth();
}

void Table::clearContent()
{
    for ( uint32_t rowNum=0; rowNum < _rows.size(); rowNum++ )
        for ( uint32_t colNum=0; colNum < _columns.size(); colNum++ )
            removeCellElement( rowNum, colNum );

    _recalculateCells();
}

void Table::clearRows()
{
  _selectedRow = -1;

  const auto& tableAreaChilds = _itemsArea->children();
  for (auto& w: tableAreaChilds )
    w->remove();

  _rows.clear();

  if (_verticalScrollBar) _verticalScrollBar->setScroll(0);

  _recalculateHeights();
}

int Table::getSelected() const { return _selectedRow; }

//! set wich row is currently selected
void Table::setSelected( int index )
{
  _selectedRow = -1;
  if ( index >= 0 && index < (int) _rows.size() )
    _selectedRow = index;
}

void Table::_recalculateColumnsWidth()
{
  _totalItemWidth=0;
  for (auto& col: _columns)
  {
    col->setPosition(_totalItemWidth, col->position().y());
    _totalItemWidth += col->width();
  }

  _header->setWidth(_totalItemWidth);
  _header->setFixedWidth(_totalItemWidth);
}

void Table::_recalculateHeights()
{
  _totalItemHeight = 0;
  float bounds[4] = { 0 };
  nvgTextBounds(screen()->nvgContext(), 0, 0, "A", nullptr, bounds);
  int fontH = bounds[3] - bounds[1] + (_cellHeightPadding * 2);
  _itemHeight = _overItemHeight == 0 ? fontH : _overItemHeight;

  _totalItemHeight = _itemHeight * _rows.size();    //  header is not counted, because we only want items
}


// automatic enabled/disabling and resizing of scrollbars
void Table::_recalculateScrollBars()
{
  if ( !_horizontalScrollBar || !_verticalScrollBar )
    return;

  bool wasHorizontalScrollBarVisible = _horizontalScrollBar->visible();
  bool wasVerticalScrollBarVisible = _verticalScrollBar->visible();
  _horizontalScrollBar->setVisible(false);
  _verticalScrollBar->setVisible(false);

  // CAREFUL: near identical calculations for tableRect and clientClip are also done in draw
  // area of table used for drawing without scrollbars
  Vector2i taPos = _itemsArea->position();
  Vector4i tableRect(taPos.x(), taPos.y(), _itemsArea->width(), _itemsArea->height());

  // needs horizontal scroll be visible?
  if ( _totalItemWidth > _itemsArea->width() )
  {
    tableRect.w() -= _horizontalScrollBar->height();
    _horizontalScrollBar->setVisible( true );
    _hscrollsize = std::max<int>(0, _totalItemWidth - tableRect.z());
  }

  // needs vertical scroll be visible?
  if ( _totalItemHeight > tableRect.w() )
  {
    tableRect.z() -= _verticalScrollBar->width();
    _verticalScrollBar->setVisible( true );
    _vscrollsize = std::max<int>(0, _totalItemHeight - tableRect.w() + 2 * _verticalScrollBar->width());

    // check horizontal again because we have now smaller clientClip
    if ( !_horizontalScrollBar->visible() )
    {
      if ( _totalItemWidth > tableRect.z() )
      {
        tableRect.w() -= _horizontalScrollBar->height();
        _horizontalScrollBar->setVisible(true);
        _hscrollsize = std::max<int>(0, _totalItemWidth - tableRect.z());
      }
    }
  }

  // find the correct size for the vertical scrollbar
  if ( _verticalScrollBar->visible() )
  {
    if  (!wasVerticalScrollBarVisible )
      _verticalScrollBar->setScroll(0);

    int offset = _horizontalScrollBar->visible()
            ? _horizontalScrollBar->height()
            : 0;

    _verticalScrollBar->setHeight( height()-(1 + offset) );
    _verticalScrollBar->setPosition( width() - _verticalScrollBar->width(), _verticalScrollBar->position().y() );
  }

  // find the correct size for the horizontal scrollbar
  if ( _horizontalScrollBar->visible() )
  {
    if ( !wasHorizontalScrollBarVisible )
      _horizontalScrollBar->setScroll(0);

    int offset = _verticalScrollBar->visible()
            ? _verticalScrollBar->width()
            : 0;
    _horizontalScrollBar->setWidth( width()-(1+offset) );
    _horizontalScrollBar->setPosition(_horizontalScrollBar->position().x(), height() - _horizontalScrollBar->height() );
  }

  _itemsArea->setWidth( width() - ( _verticalScrollBar->visible() ? 1 : 0 ) * _verticalScrollBar->width() );
  _itemsArea->setHeight( height() - _header->height()
                - ( _horizontalScrollBar->visible() ? 1 : 0 ) * _horizontalScrollBar->height() );

  _verticalScrollBar->bringToFront();
  _horizontalScrollBar->bringToFront();
}

void Table::_refreshControls()
{
  //updateAbsolutePosition();

  _recalculateColumnsWidth();
  _recalculateHeights();
  _recalculateScrollBars();
  _recalculateCells();
}

void Table::_recalculateCells()
{
  int yPos = 0;
  int xPos = 0;
  for (auto& row: _rows)
  {
    auto cit = _columns.begin();
    xPos = 0;
    for (int index=0; cit != _columns.end(); cit++, index++ )
    {
      row.items[index]->setPosition((*cit)->position().x(), yPos);
      row.items[index]->setSize({ (*cit)->width(), _itemHeight });
      row.items[index]->setFixedSize({ (*cit)->width(), _itemHeight });
      row.items[index]->sendToBack();
      xPos += (*cit)->width();
    }

    yPos += _itemHeight;
  }

  _itemsArea->setFixedSize({ xPos, yPos });
}

bool Table::scrollEvent(const Vector2i &p, const Vector2f &rel)
{
  int current = _totalItemHeight - _itemsArea->height();
  //float d = (rel.y() < 0 ? -1 : 1) * _itemHeight;
  _verticalScrollBar->setScroll(_verticalScrollBar->scroll() + (rel.y() < 0 ? -1 : 1)*0.05);
  return true;
}

bool Table::mouseButtonEvent(const Vector2i &pp, int button, bool down, int modifiers)
{
  if (enabled())
  {
    if (isMouseButtonLeft(button) && down)
    {
      if (_dragColumnStart(pp.x(), pp.y()))
      {
        requestFocus();
        return true;
      }

      if (_selectColumnHeader(pp.x(), pp.y()))
        return true;

      _selecting = true;
      requestFocus();
      return true;
    }
    else if (isMouseButtonLeft(button) && !down)
    {
      _currentResizedColumn = -1;
      _selecting = false;
      _selectNew(pp.x(), pp.y(), true, false);
    }
  }

  return Widget::mouseButtonEvent(pp, button, down, modifiers);
}

bool Table::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers)
{
    if (_currentResizedColumn >= 0)
    {
      if (_dragColumnUpdate(p.x()))
      {
        return true;
      }
    }
    if (_selecting || _moveOverSelect)
    {
      if (isPointInsideRect(p, absoluteRect()))
      {
        _selectNew(p.x(), p.y(), false, true);
        return true;
      }
    }

  return Widget::mouseMotionEvent(p, rel, button, modifiers);
}

void Table::setColumnOrdering(uint32_t columnIndex, ColumnOrder mode)
{
  if ( columnIndex < _columns.size() )
    _columns[columnIndex]->orderingMode = mode;
}

void Table::swapRows(uint32_t rowIndexA, uint32_t rowIndexB)
{
  if ( rowIndexA >= _rows.size() )
    return;

  if ( rowIndexB >= _rows.size() )
    return;

  Row swap = _rows[rowIndexA];
  _rows[rowIndexA] = _rows[rowIndexB];
  _rows[rowIndexB] = swap;

  if ( _selectedRow == int(rowIndexA) )
    _selectedRow = rowIndexB;
  else if ( _selectedRow == int(rowIndexB) )
    _selectedRow = rowIndexA;
}

bool Table::_dragColumnStart(int xpos, int ypos)
{
  if ( !_resizableColumns )
    return false;

  if ( ypos > ( absolutePosition().y() + _itemHeight ) )
    return false;

  const int CLICK_AREA = 12;  // to left and right of line which can be dragged
  int pos = absolutePosition().x() + 1;

  if ( _horizontalScrollBar && _horizontalScrollBar->visible() )
    pos -= (_horizontalScrollBar->scroll() * _hscrollsize);

  pos += _totalItemWidth;

  // have to search from the right as otherwise lines could no longer be resized when a column width is 0
  for ( int i = (int)_columns.size()-1; i >= 0 ; --i )
  {
    uint32_t colWidth = _columns[i]->width();

    if ( xpos >= (pos - CLICK_AREA) && xpos < ( pos + CLICK_AREA ) )
    {
      _currentResizedColumn = i;
      _resizeStart = xpos;
      return true;
    }

    pos -= colWidth;
  }

  return false;
}

bool Table::_dragColumnUpdate(int xpos)
{
  if ( !_resizableColumns || _currentResizedColumn < 0 || _currentResizedColumn >= int(_columns.size()) )
  {
    _currentResizedColumn = -1;
    return false;
  }

  int ww = int(_columns[_currentResizedColumn]->width()) + (xpos-_resizeStart);
  if ( ww < 0 )
    ww = 0;

  setColumnWidth(_currentResizedColumn, uint32_t(ww));
  _resizeStart = xpos;

  _recalculateCells();
  return false;
}

int Table::_getCurrentColumn( int xpos, int ypos )
{
  int pos = absolutePosition().x() + 1;

  if ( _horizontalScrollBar && _horizontalScrollBar->visible() )
      pos -= (_horizontalScrollBar->scroll() * _hscrollsize);

  for ( uint32_t i = 0 ; i < _columns.size() ; ++i )
  {
      uint32_t colWidth = _columns[i]->width();

      if ( xpos >= pos && xpos < ( pos + int(colWidth) ) )
          return i;

      pos += colWidth;
  }

  return -1;
}

bool Table::_selectColumnHeader(int xpos, int ypos)
{
  if ( ypos > ( absolutePosition().y() + _itemHeight ) )
    return false;

    _selectedColumn = _getCurrentColumn( xpos, ypos );
    if ( _selectedColumn >= 0 )
    {
        setActiveColumn( _selectedColumn, true );
        return true;
    }

  return false;
}

bool _nstrCmp(const std::string& s1, const std::string& s2)
{
  return strcmp(s1.c_str(), s2.c_str());
}

void Table::orderRows(int columnIndex, RowOrder mode)
{
  Row swap;

  if ( columnIndex == -1 )
    columnIndex = getActiveColumn();
  if ( columnIndex < 0 )
    return;

  if ( mode == RowOrder::roAscending )
  {
    for ( int i = 0 ; i < int(_rows.size()) - 1 ; ++i )
    {
      for ( int j = 0 ; j < int(_rows.size()) - i - 1 ; ++j )
      {
        if (_nstrCmp(_rows[j+1].items[columnIndex]->caption(), _rows[j].items[columnIndex]->caption()) )
        {
          swap = _rows[j];
          _rows[j] = _rows[j+1];
          _rows[j+1] = swap;

          if ( _selectedRow == j )
            _selectedRow = j+1;
          else if ( _selectedRow == j+1 )
            _selectedRow = j;
        }
      }
    }
  }
  else if ( mode == RowOrder::roDescending )
  {
    for ( int i = 0 ; i < int(_rows.size()) - 1 ; ++i )
    {
      for ( int j = 0 ; j < int(_rows.size()) - i - 1 ; ++j )
      {
        if ( _nstrCmp(_rows[j].items[columnIndex]->caption(), _rows[j+1].items[columnIndex]->caption()) )
        {
          swap = _rows[j];
          _rows[j] = _rows[j+1];
          _rows[j+1] = swap;

          if ( _selectedRow == j )
            _selectedRow = j+1;
          else if ( _selectedRow == j+1 )
            _selectedRow = j;
        }
      }
    }
  }
}

void Table::_selectNew( int xpos, int ypos, bool lmb, bool onlyHover)
{
  int oldSelectedRow = _selectedRow;
  int oldSelectedColumn = _selectedColumn;

  if ( ypos < ( absolutePosition().y() + _itemHeight ) )
    return;

  // find new selected item.
  if (_itemHeight!=0)
    _selectedRow = ((ypos - absolutePosition().y() - _itemHeight - 1) + (_verticalScrollBar->scroll() * _vscrollsize)) / _itemHeight;

  _selectedColumn = _getCurrentColumn( xpos, ypos );

  if (_selectedRow >= (int)_rows.size())
    _selectedRow = _rows.size() - 1;
  else if (_selectedRow<0)
    _selectedRow = 0;

  // post the news
  if ( !onlyHover )
  {
    bool selectedAgain = (_selectedRow != oldSelectedRow || _selectedColumn != oldSelectedColumn )
                                        ? false
                                        : true;

    if ( lmb && selectedAgain && ( getTimeFromStart() - _cellLastTimeClick ) < 200 )
    {
      _finishEditCell();
      _startEditCell(_selectedColumn, _selectedRow);
    }

    _cellLastTimeClick = getTimeFromStart();
  }
}

Table::Cell* Table::_getCell(int row, int col)
{
  if (row < _rows.size() && col < _columns.size())
    return _rows[row].items[col];

  return nullptr;
}

void Table::_startEditCell(int col, int row)
{
  if (_edit)
    return;

  Cell* cell = _getCell(row, col);
  if (cell)
  {
    cell->inEditMode = true;
    _edit = new TextBox(cell, getCellText(row, col));
    _edit->setSize(cell->size());
    _edit->setFixedSize(cell->size());
    _edit->setEditable(true);
    _edit->requestFocus();
    _edit->setComitCallback([cell, this](Widget* w) {
      if (TextBox* ed = TextBox::cast(w))
      {
        cell->setCaption(ed->value());
        cell->requestFocus();
        cell->inEditMode = false;
      }
      _finishEditCell();
    });
  }
}

void Table::_finishEditCell()
{
  if (!_edit)
    return;
  _edit->remove();
  _edit = nullptr;
}

void Table::beforePaint(NVGcontext* ctx)
{
  if ( _needRefreshCellsGeometry )
  {
    _recalculateCells();
    _needRefreshCellsGeometry = false;
  }
}

//! draws the element and its children
void Table::draw(NVGcontext* ctx)
{
  if ( !visible() )
    return;

  if ( _font.empty() )
    return;

  int yOffset = _verticalScrollBar->scroll() * _vscrollsize;
  int xOffset = _horizontalScrollBar->scroll() * _hscrollsize;

  _header->setPosition(-xOffset, _header->position().y());
  _itemsArea->setPosition(-xOffset, 0);

  if (_drawflags.test(drawRows))
  {
    nvgBeginPath(ctx);
    nvgStrokeColor(ctx, Color(0xc0, 0x80));

    for (uint32_t i = 0; i < _rows.size(); ++i)
    {
      Vector4i r = _rows[i].items.front()->absoluteRect();
      //r.y() = r.w() - 1;
      r.z() = position().x() + width();
      nvgRect(ctx, r.x(), r.y(), r.width(), r.height());
    }
    nvgStroke(ctx);
  }

  // draw selected row background highlighted
  if (_drawflags.test(drawRows))
  {
    nvgBeginPath(ctx);
    nvgStrokeColor(ctx, Color(0xff, 0x0, 0x0, 0x80));
    if (_selectedRow >= 0 && _selectedRow < _rows.size()
        && _selectedColumn >= 0 && _selectedColumn < _columns.size()
        && _drawflags.test(drawActiveRow))
    {
      Vector4i r(_rows[_selectedRow].items[_selectedColumn]->absoluteRect());
      nvgRect(ctx, r.x(), r.y(), r.z() - r.x(), r.w() - r.y());
    }
    nvgStroke(ctx);
  }

  if ( _drawflags.test( drawColumns ) )
  {
    nvgBeginPath(ctx);
    nvgStrokeColor(ctx, Color(0xc0, 0x80));

    for (auto& c: _columns)
    {
      Vector4i ar = c->absoluteRect();
      Vector4i r( ar.x(), ar.y(), ar.z(), ar.y() + height());
      nvgRect(ctx, r.x(), r.y(), r.z() - r.x(), r.w() - r.y());
    }

    nvgStroke(ctx);
  }

  Widget::draw(ctx);

   // draw background for whole element
  if (_drawflags.test(drawBorder))
  {
    nvgBeginPath(ctx);
    nvgRect(ctx, mPos.x(), mPos.y(), width(), height());
    nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 255));
    nvgStroke(ctx);
  }
}

void Table::setDrawFlag( DrawFlag flag, bool enabled ) { _drawflags.set( flag, enabled ); }
bool Table::isFlag( DrawFlag flag ) const {  return _drawflags.test( (int)flag ); }

//! Writes attributes of the element.
/*void Table::save( core::VariantArray* out ) const
{
}*/

//! Reads attributes of the element
/*void Table::load( core::VariantArray* in )
{
}*/

void Table::setItemHeight( int height )
{
  _overItemHeight = height;
  float bounds[4] = { 0 };
  nvgTextBounds(screen()->nvgContext(), 0, 0, "A", nullptr, bounds);
  int hh = bounds[3] - bounds[1];
  _itemHeight = _overItemHeight == 0 ? hh + (_cellHeightPadding * 2) : _overItemHeight;
}

int Table::getSelectedColumn() const { return _selectedColumn; }

void Table::removeChild(Widget* child)
{
    for ( uint32_t rowIndex = 0 ; rowIndex < _rows.size() ; ++rowIndex )
        for ( uint32_t columnIndex = 0 ; columnIndex < _columns.size() ; ++columnIndex )
        {
            Cell* cell = _rows[rowIndex].items[columnIndex];
            if ( cell && cell->element == child )
            {
                cell->element = nullptr;
                break;
            }
        }

    Widget::removeChild( child );
}

void Table::setCellElement( uint32_t rowIndex, uint32_t columnIndex, Widget* elm )
{
    if ( rowIndex < _rows.size() && columnIndex < _columns.size() )
    {
      Cell* cell = _rows[rowIndex].items[columnIndex];

      if ( elm != getCellElement( rowIndex, columnIndex ) )
      {
        removeCellElement( rowIndex, columnIndex );

        if ( elm )
        {
          cell->addChild( elm );
          cell->element = elm;
        }
      }
    }

    _verticalScrollBar->bringToFront();
    _horizontalScrollBar->bringToFront();
}

void Table::removeCellElement( uint32_t rowIndex, uint32_t columnIndex )
{
    if ( rowIndex < _rows.size() && columnIndex < _columns.size() )
    {
        Cell* cell = _rows[rowIndex].items[columnIndex];

        if ( cell->element )
            cell->element->remove();

        cell->element = nullptr;
    }
}

Widget* Table::getCellElement( uint32_t rowIndex, uint32_t columnIndex ) const
{
    if ( rowIndex < _rows.size() && columnIndex < _columns.size() )
    {
        return _rows[rowIndex].items[columnIndex]->element;
    }

    return NULL;
}

ScrollBar* Table::getVerticalScrolBar() { return _verticalScrollBar; }

NAMESPACE_END(nanogui)
