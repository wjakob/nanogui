#pragma once

#include <nanogui/widget.h>
#include <bitset>
#include <vector>

NAMESPACE_BEGIN(nanogui)

class ScrollBar;
class TextBox;

class NANOGUI_EXPORT Table : public Widget
{
public:
  RTTI_CLASS_UID(Table)
  RTTI_DECLARE_INFO(Table)

  //! modes for ordering used when a column header is clicked
  enum ColumnOrder
  {
    //! Do not use ordering
    coNone,

    //! Send a EGET_TABLE_HEADER_CHANGED message when a column header is clicked.
    coCustom,

    //! Sort it ascending by it's ascii value like: a,b,c,...
    coAscending,

    //! Sort it descending by it's ascii value like: z,x,y,...
    coDescending,

    //! Sort it ascending on first click, descending on next, etc
    coAscendingDescending,

    //! Not used as mode, only to get maximum value for this enum
    coCount
  };

  enum RowOrder
  {
    //! No element ordering
    roNone,

    //! Elements are ordered from the smallest to the largest.
    roAscending,

    //! Elements are ordered from the largest to the smallest.
    roDescending,

    //! this value is not used, it only specifies the amount of default ordering types
    //! available.
    roCount
  };

  enum DrawFlag
  {
    drawRows = 0,
    drawColumns = 1,
    drawActiveRow = 2,
    drawRowBackground = 3,
    drawBorder = 4,
    drawCount
  } ;
  //! constructor
  Table( Widget* parent,
       const std::string& id, const Vector4i& rectangle, bool clip=true,
       bool drawBack=false, bool moveOverSelect=true);

  //! destructor
  virtual ~Table();

  //! Adds a column
  //! If columnIndex is outside the current range, do push new colum at the end
  virtual void addColumn(const std::string& caption, uint32_t columnIndex=0xffffffff);

  //! remove a column from the table
  virtual void removeColumn(uint32_t columnIndex);

  //! Returns the number of columns in the table control
  virtual int getColumnCount() const;

  //! Makes a column active. This will trigger an ordering process.
  /** \param idx: The id of the column to make active.
  \return True if successful. */
  virtual bool setActiveColumn(int columnIndex, bool doOrder=false);

  //! Returns which header is currently active
  virtual int getActiveColumn() const;

  bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;

  //! Returns the ordering used by the currently active column
  virtual RowOrder getActiveColumnOrdering() const;

  //! set a column width
  virtual void setColumnWidth(uint32_t columnIndex, uint32_t width);

  //! returns column width
  virtual uint32_t getColumnWidth(uint32_t columnIndex) const;

  //! columns can be resized by drag 'n drop
  virtual void setResizableColumns(bool resizable);

  //! can columns be resized by dran 'n drop?
  virtual bool hasResizableColumns() const;

  //! This tells the table control which ordering mode should be used when
  //! a column header is clicked.
  /** \param columnIndex: The index of the column header.
  \param state: If true, a message will be sent and you can order the table data as you whish.*/
  //! \param mode: One of the modes defined in EGUI_COLUMN_ORDERING
  virtual void setColumnOrdering(uint32_t columnIndex, ColumnOrder mode);

  //! Returns which row is currently selected
  virtual int getSelected() const;

  //! set wich row is currently selected
  virtual void setSelected( int index );

  virtual ScrollBar* getVerticalScrolBar();

  virtual int getSelectedColumn() const;

  //! Returns amount of rows in the tabcontrol
  virtual int getRowCount() const;

  //! adds a row to the table
  /** \param rowIndex: zero based index of rows. The row will be
    inserted at this position. If a row already exists
    there, it will be placed after it. If the row is larger
    than the actual number of rows by more than one, it
    won't be created. Note that if you create a row that is
    not at the end, there might be performance issues*/
  virtual uint32_t addRow(uint32_t rowIndex);

  //! Remove a row from the table
  virtual void removeRow(uint32_t rowIndex);

  //! clear the table rows, but keep the columns intact
  virtual void clearRows();

  //! Swap two row positions. This is useful for a custom ordering algo.
  virtual void swapRows(uint32_t rowIndexA, uint32_t rowIndexB);

  //! This tells the table to start ordering all the rows. You
  //! need to explicitly tell the table to reorder the rows when
  //! a new row is added or the cells data is changed. This makes
  //! the system more flexible and doesn't make you pay the cost
  //! of ordering when adding a lot of rows.
  //! \param columnIndex: When set to -1 the active column is used.
  virtual void orderRows(int columnIndex=-1, RowOrder mode=roNone);


  //! Set the text of a cell
  virtual void setCellText(uint32_t rowIndex, uint32_t columnIndex, const std::string& text);

  //! Set element of a cell
  virtual void setCellElement(uint32_t rowIndex, uint32_t columnIndex, Widget* elm );

  //! Remove element from cell
  virtual void removeCellElement(uint32_t rowIndex, uint32_t columnIndex);

  //! Get element from cell
  virtual Widget* getCellElement(uint32_t rowIndex, uint32_t columnIndex) const;

  //! Set the text of a cell, and set a color of this cell.
  virtual void setCellText(uint32_t rowIndex, uint32_t columnIndex, const std::string& text, const Color& color);

  //! Set the data of a cell
  //! data will not be serialized.
  virtual void setCellDataptr(uint32_t rowIndex, uint32_t columnIndex, uintptr_t data);

  //! Set the color of a cell text
  virtual void setCellTextColor(uint32_t rowIndex, uint32_t columnIndex, const Color& color);

  //! Get the text of a cell
  virtual std::string getCellText(uint32_t rowIndex, uint32_t columnIndex ) const;

  //! Get the data of a cell
  virtual uintptr_t getCellDataptr(uint32_t rowIndex, uint32_t columnIndex ) const;

  //! clears the table, deletes all items in the table
  virtual void clear();

  //! clears the contents int table, deletes all cell elements in the table
  virtual void clearContent();

  //! called if an event happened.
  //virtual bool onEvent(const NEvent &event);

  //! draws the element and its children
  void draw(NVGcontext* ctx) override;

  //! Set flags, as defined in EGUI_TABLE_DRAW_FLAGS, which influence the layout
  void setDrawFlag( DrawFlag flag, bool enabled=true );

  //! Get the flags, as defined in EGUI_TABLE_DRAW_FLAGS, which influence the layout
  bool isFlag( DrawFlag flag ) const;

  //!
  void setItemHeight(int height);

  //!
  void beforeDraw(NVGcontext* ctx);

  bool mouseButtonEvent(const Vector2i &pp, int button, bool down, int modifiers) override;
  bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;

  //! Writes attributes of the object.
  //! Implement this to expose the attributes of your scene node animator for
  //! scripting languages, editors, debuggers or xml serialization purposes.
  //virtual void save( core::VariantArray* out ) const;

  //! Reads attributes of the object.
  //! Implement this to set the attributes of your scene node animator for
  //! scripting languages, editors, debuggers or xml deserialization purposes.
  //virtual void load( core::VariantArray* in );

  void removeChild(const Widget* child) override;
  void removeChild(int index) override;

protected:
  virtual void _refreshControls();
  virtual void _recalculateScrollBars();
  void _startEditCell(int col, int row);
  void _finishEditCell();

private:
  void _selectNew(int xpos, int ypos, bool lmb, bool onlyHover);
  bool _selectColumnHeader(int xpos, int ypos);
  bool _dragColumnStart(int xpos, int ypos);
  bool _dragColumnUpdate(int xpos);
  void _recalculateHeights();
  void _recalculateColumnsWidth();

  int _getCurrentColumn(int xpos, int ypos );
  void _recalculateCells();

  bool _clip;
  bool _moveOverSelect;
  bool _selecting;
  int  _currentResizedColumn;
  int  _resizeStart;
  bool _resizableColumns;

  int _itemHeight;
  int _overItemHeight;
  int _totalItemHeight;
  int _totalItemWidth;
  int _selectedRow, _selectedColumn;
  int _editedRow, _editedColumn;
  int _cellHeightPadding;
  int _cellWidthPadding;
  int _activeTab;
  RowOrder _currentOrdering;

  class Cell;
  class Column;
  class HidingElement;
  struct Row { std::vector<Cell*> items; };

  typedef std::vector<Column*> Columns;
  typedef std::vector<Row> Rows;
  typedef std::bitset<16> DrawFlags;

  DrawFlags _drawflags;
  Columns _columns;
  Rows _rows;
  std::string _font = "sans";

  Widget* _header;
  TextBox* _edit = nullptr;
  Widget* _itemsArea;
  ScrollBar* _verticalScrollBar;
  ScrollBar* _horizontalScrollBar;
  bool _needRefreshCellsGeometry;
  uint32_t _cellLastTimeClick;

  int _vscrollsize = 0;
  int _hscrollsize = 0;

  Cell* _getCell(int row, int col);
};

NAMESPACE_END(nanogui)

