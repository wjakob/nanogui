/*
  This file contains docstrings for the Python bindings.
  Do not edit! These were automatically extracted by mkdoc.py
 */

#define __EXPAND(x)                                      x
#define __COUNT(_1, _2, _3, _4, _5, _6, _7, COUNT, ...)  COUNT
#define __VA_SIZE(...)                                   __EXPAND(__COUNT(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1))
#define __CAT1(a, b)                                     a ## b
#define __CAT2(a, b)                                     __CAT1(a, b)
#define __DOC1(n1)                                       __doc_##n1
#define __DOC2(n1, n2)                                   __doc_##n1##_##n2
#define __DOC3(n1, n2, n3)                               __doc_##n1##_##n2##_##n3
#define __DOC4(n1, n2, n3, n4)                           __doc_##n1##_##n2##_##n3##_##n4
#define __DOC5(n1, n2, n3, n4, n5)                       __doc_##n1##_##n2##_##n3##_##n4##_##n5
#define __DOC6(n1, n2, n3, n4, n5, n6)                   __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6
#define __DOC7(n1, n2, n3, n4, n5, n6, n7)               __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6##_##n7
#define DOC(...)                                         __EXPAND(__EXPAND(__CAT2(__DOC, __VA_SIZE(__VA_ARGS__)))(__VA_ARGS__))

#if defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


static const char *__doc_nanogui_AdvancedGridLayout =
R"doc(Advanced Grid layout.

The is a fancier grid layout with support for items that span multiple
rows or columns, and per-widget alignment flags. Each row and column
additionally stores a stretch factor that controls how additional
space is redistributed. The downside of this flexibility is that a
layout anchor data structure must be provided for each widget.

An example:

```
using AdvancedGridLayout::Anchor;
Label *label = new Label(window, "A label");
// Add a centered label at grid position (1, 5), which spans two horizontal cells
layout->setAnchor(label, Anchor(1, 5, 2, 1, Alignment::Middle, Alignment::Middle));

```

The grid is initialized with user-specified column and row size
vectors (which can be expanded later on if desired). If a size value
of zero is specified for a column or row, the size is set to the
maximum preferred size of any widgets contained in the same row or
column. Any remaining space is redistributed according to the row and
column stretch factors.

The high level usage somewhat resembles the classic HIG layout:

- https://web.archive.org/web/20070813221705/http://www.autel.cz/dmi/t
utorial.html - https://github.com/jaapgeurts/higlayout)doc";

static const char *__doc_nanogui_AdvancedGridLayout_AdvancedGridLayout =
R"doc(Creates an AdvancedGridLayout with specified columns, rows, and
margin.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor = R"doc(Helper struct to coordinate anchor points for the layout.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_Anchor = R"doc(Creates a ``0`` Anchor.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_Anchor_2 = R"doc(Create an Anchor at position ``(x, y)`` with specified Alignment.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_Anchor_3 =
R"doc(Create an Anchor at position ``(x, y)`` of size ``(w, h)`` with
specified alignments.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_align = R"doc(The ``(x, y)`` Alignment.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_operator_int = R"doc(Allows for printing out Anchor position, size, and alignment.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_pos = R"doc(The ``(x, y)`` position.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_size = R"doc(The ``(x, y)`` size.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_anchor = R"doc(Retrieve the anchor data structure for a given widget)doc";

static const char *__doc_nanogui_AdvancedGridLayout_appendCol = R"doc(Append a column of the given size (and stretch factor))doc";

static const char *__doc_nanogui_AdvancedGridLayout_appendRow = R"doc(Append a row of the given size (and stretch factor))doc";

static const char *__doc_nanogui_AdvancedGridLayout_colCount = R"doc(Return the number of cols)doc";

static const char *__doc_nanogui_AdvancedGridLayout_computeLayout = R"doc(Computes the layout)doc";

static const char *__doc_nanogui_AdvancedGridLayout_mAnchor = R"doc(The mapping of widgets to their specified anchor points.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_mColStretch = R"doc(The stretch for each column of this AdvancedGridLayout.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_mCols = R"doc(The columns of this AdvancedGridLayout.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_mMargin = R"doc(The margin around this AdvancedGridLayout.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_mRowStretch = R"doc(The stretch for each row of this AdvancedGridLayout.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_mRows = R"doc(The rows of this AdvancedGridLayout.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_margin = R"doc(The margin of this AdvancedGridLayout.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_performLayout = R"doc(See Layout::performLayout.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_preferredSize = R"doc(See Layout::preferredSize.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_rowCount = R"doc(Return the number of rows)doc";

static const char *__doc_nanogui_AdvancedGridLayout_setAnchor = R"doc(Specify the anchor data structure for a given widget)doc";

static const char *__doc_nanogui_AdvancedGridLayout_setColStretch = R"doc(Set the stretch factor of a given column)doc";

static const char *__doc_nanogui_AdvancedGridLayout_setMargin = R"doc(Sets the margin of this AdvancedGridLayout.)doc";

static const char *__doc_nanogui_AdvancedGridLayout_setRowStretch = R"doc(Set the stretch factor of a given row)doc";

static const char *__doc_nanogui_Alignment = R"doc(The different kinds of alignments a layout can perform.)doc";

static const char *__doc_nanogui_Alignment_Fill = R"doc(Fill according to preferred sizes.)doc";

static const char *__doc_nanogui_Alignment_Maximum = R"doc(Take as much space as is allowed.)doc";

static const char *__doc_nanogui_Alignment_Middle = R"doc(Center align.)doc";

static const char *__doc_nanogui_Alignment_Minimum = R"doc(Take only as much space as is required.)doc";

static const char *__doc_nanogui_TextAlignment = R"doc(The different kinds of alignments a layout can perform.)doc";

static const char *__doc_nanogui_TextAlignment_Auto = R"doc(Fill according to preferred sizes.)doc";

static const char *__doc_nanogui_TextAlignment_Left = R"doc(Take as much space as is allowed.)doc";

static const char *__doc_nanogui_TextAlignment_Center = R"doc(Center align.)doc";

static const char *__doc_nanogui_TextAlignment_Right = R"doc(Take only as much space as is required.)doc";


static const char *__doc_nanogui_Arcball =
R"doc(Arcball helper class to interactively rotate objects on-screen.

The Arcball class enables fluid interaction by representing rotations
using a quaternion, and is setup to be used in conjunction with the
existing mouse callbacks defined in nanogui::Widget. The Arcball
operates by maintaining an "active" state which is typically
controlled using a mouse button click / release. A click pressed would
call Arcball::button with ``down = true``, and a click released with
``down = false``. The high level mechanics are:

1. The Arcball is made active by calling Arcball::button with a
specified click location, and ``down = true``. 2. As the user holds
the mouse button down and drags, calls to Arcball::motion are issued.
Internally, the Arcball keeps track of how far the rotation is from
the start click. During the active state, mQuat is not updated, call
Arcball::matrix to get the current rotation for use in drawing
updates. Receiving the rotation as a matrix will usually be more
convenient for traditional pipelines, however you can also acquire the
active rotation using Arcball::activeState. 3. The user releases the
mouse button, and a call to Arcball::button with ``down = false``. The
Arcball is no longer active, and its internal mQuat is updated.

A very simple nanogui::Screen derived class to illustrate usage:

```
class ArcballScreen : public nanogui::Screen {
public:
    // Creating a 400x400 window
    ArcballScreen() : nanogui::Screen({400, 400}, "ArcballDemo") {
        mArcball.setSize(mSize);// Note 1
    }
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override {
        // In this example, we are using the left mouse button
        // to control the arcball motion
        if (button == GLFW_MOUSE_BUTTON_1) {
            mArcball.button(p, down);// Note 2
            return true;
        }
        return false;
    }
    virtual bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override {
        if (button == GLFW_MOUSE_BUTTON_1) {
            mArcball.motion(p);// Note 2
            return true;
        }
        return false;
    }
    virtual void drawContents() override {
        // Option 1: acquire a 4x4 homogeneous rotation matrix
        Matrix4f rotation = mArcball.matrix();
        // Option 2: acquire an equivalent quaternion
        Quaternionf rotation = mArcball.activeState();
        // ... do some drawing with the current rotation ...
    }
protected:
    nanogui::Arcball mArcball;
};
**Note 1**
 The user is responsible for setting the size with
 :func:`Arcball::setSize <nanogui::Arcball::setSize>`, this does **not**
 need to be the same as the Screen dimensions (e.g., you are using the
 Arcball to control a specific ``glViewport``).
**Note 2**
 Be aware that the input vector ``p`` to
 :func:`Widget::mouseButtonEvent <nanogui::Widget::mouseButtonEvent>`
 and :func:`Widget::mouseMotionEvent <nanogui::Widget::mouseMotionEvent>`
 are in the coordinates of the Screen dimensions (top left is ``(0, 0)``,
 bottom right is ``(width, height)``).  If you are using the Arcball to
 control a subregion of the Screen, you will want to transform the input
 ``p`` before calling :func:`Arcball::button <nanogui::Arcball::button>`
 or :func:`Arcball::motion <nanogui::Arcball::motion>`.  For example, if
 controlling the right half of the screen, you might create
 ``Vector2i adjusted_click(p.x() - (mSize.x() / 2), p.y())``, and then
 call ``mArcball.motion(adjusted_click)``.

```)doc";

static const char *__doc_nanogui_Arcball_Arcball =
R"doc(The default constructor.

```
Note:
Make sure to call :func:`Arcball::setSize <nanogui::Arcball::setSize>`
after construction.

```

Parameter ``speedFactor``:
    The speed at which the Arcball rotates (default: ``2.0``). See
    also mSpeedFactor.)doc";

static const char *__doc_nanogui_Arcball_Arcball_2 =
R"doc(Constructs an Arcball based off of the specified rotation.

```
Note:
Make sure to call :func:`Arcball::setSize <nanogui::Arcball::setSize>`
after construction.

```)doc";

static const char *__doc_nanogui_Arcball_active = R"doc(Returns whether or not this Arcball is currently active.)doc";

static const char *__doc_nanogui_Arcball_activeState = R"doc(Returns the current rotation *including* the active motion.)doc";

static const char *__doc_nanogui_Arcball_button =
R"doc(Signals a state change from active to non-active, or vice-versa.

Parameter ``pos``:
    The click location, should be in the same coordinate system as
    specified by mSize.

Parameter ``pressed``:
    When ``True``, this Arcball becomes active. When ``False``, this
    Arcball becomes non-active, and its internal mQuat is updated with
    the final rotation.)doc";

static const char *__doc_nanogui_Arcball_interrupt =
R"doc(Interrupts the current Arcball motion by calling Arcball::button with
``(0, 0)`` and ``False``.

Use this method to "close" the state of the Arcball when a mouse
release event is not available. You would use this method if you need
to stop the Arcball from updating its internal rotation, but the event
stopping the rotation does **not** come from a mouse release. For
example, you have a callback that created a nanogui::MessageDialog
which will now be in focus.)doc";

static const char *__doc_nanogui_Arcball_mActive = R"doc(Whether or not this Arcball is currently active.)doc";

static const char *__doc_nanogui_Arcball_mIncr =
R"doc(When active, tracks the overall update to the state. Identity when
non-active.)doc";

static const char *__doc_nanogui_Arcball_mLastPos =
R"doc(The last click position (which triggered the Arcball to be active /
non-active).)doc";

static const char *__doc_nanogui_Arcball_mQuat =
R"doc(The current stable state. When this Arcball is active, represents the
state of this Arcball when Arcball::button was called with ``down =
true``.)doc";

static const char *__doc_nanogui_Arcball_mSize = R"doc(The size of this Arcball.)doc";

static const char *__doc_nanogui_Arcball_mSpeedFactor =
R"doc(The speed at which this Arcball rotates. Smaller values mean it
rotates more slowly, higher values mean it rotates more quickly.)doc";

static const char *__doc_nanogui_Arcball_matrix =
R"doc(Returns the current rotation *including* the active motion, suitable
for use with typical homogeneous matrix transformations. The upper
left 3x3 block is the rotation matrix, with 0-0-0-1 as the right-most
column / bottom row.)doc";

static const char *__doc_nanogui_Arcball_motion =
R"doc(When active, updates mIncr corresponding to the specified position.

Parameter ``pos``:
    Where the mouse has been dragged to.)doc";

static const char *__doc_nanogui_Arcball_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_new = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Arcball_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_Arcball_setSize =
R"doc(Sets the size of this Arcball.

The size of the Arcball and the positions being provided in
Arcball::button and Arcball::motion are directly related.)doc";

static const char *__doc_nanogui_Arcball_setSpeedFactor = R"doc(Sets the speed at which this Arcball rotates. See also mSpeedFactor.)doc";

static const char *__doc_nanogui_Arcball_setState =
R"doc(Sets the rotation of this Arcball. The Arcball will be marked as
**not** active.)doc";

static const char *__doc_nanogui_Arcball_size = R"doc(Returns the current size of this Arcball.)doc";

static const char *__doc_nanogui_Arcball_speedFactor = R"doc(Returns the current speed at which this Arcball rotates.)doc";

static const char *__doc_nanogui_Arcball_state =
R"doc(The internal rotation of the Arcball.

Call Arcball::matrix for drawing loops, this method will not return
any updates while mActive is ``True``.)doc";

static const char *__doc_nanogui_Arcball_state_2 = R"doc(``const`` version of Arcball::state.)doc";

static const char *__doc_nanogui_BoxLayout =
R"doc(Simple horizontal/vertical box layout

This widget stacks up a bunch of widgets horizontally or vertically.
It adds margins around the entire container and a custom spacing
between adjacent widgets.)doc";

static const char *__doc_nanogui_BoxLayout_BoxLayout =
R"doc(Construct a box layout which packs widgets in the given
``Orientation``

Parameter ``orientation``:
    The Orientation this BoxLayout expands along

Parameter ``alignment``:
    Widget alignment perpendicular to the chosen orientation

Parameter ``margin``:
    Margin around the layout container

Parameter ``spacing``:
    Extra spacing placed between widgets)doc";

static const char *__doc_nanogui_BoxLayout_alignment = R"doc(The Alignment of this BoxLayout.)doc";

static const char *__doc_nanogui_BoxLayout_mAlignment = R"doc(The Alignment of this BoxLayout.)doc";

static const char *__doc_nanogui_BoxLayout_mMargin = R"doc(The margin of this BoxLayout.)doc";

static const char *__doc_nanogui_BoxLayout_mOrientation = R"doc(The Orientation of this BoxLayout.)doc";

static const char *__doc_nanogui_BoxLayout_mSpacing = R"doc(The spacing between widgets of this BoxLayout.)doc";

static const char *__doc_nanogui_BoxLayout_margin = R"doc(The margin of this BoxLayout.)doc";

static const char *__doc_nanogui_BoxLayout_orientation = R"doc(The Orientation this BoxLayout is using.)doc";

static const char *__doc_nanogui_BoxLayout_performLayout = R"doc(See Layout::performLayout.)doc";

static const char *__doc_nanogui_BoxLayout_preferredSize = R"doc(See Layout::preferredSize.)doc";

static const char *__doc_nanogui_BoxLayout_setAlignment = R"doc(Sets the Alignment of this BoxLayout.)doc";

static const char *__doc_nanogui_BoxLayout_setMargin = R"doc(Sets the margin of this BoxLayout.)doc";

static const char *__doc_nanogui_BoxLayout_setOrientation = R"doc(Sets the Orientation of this BoxLayout.)doc";

static const char *__doc_nanogui_BoxLayout_setSpacing = R"doc(Sets the spacing of this BoxLayout.)doc";

static const char *__doc_nanogui_BoxLayout_spacing = R"doc(The spacing this BoxLayout is using to pad in between widgets.)doc";

static const char *__doc_nanogui_Button = R"doc([Normal/Toggle/Radio/Popup] Button widget.)doc";

static const char *__doc_nanogui_Button_Button =
R"doc(Creates a button attached to the specified parent.

Parameter ``parent``:
    The nanogui::Widget this Button will be attached to.

Parameter ``caption``:
    The name of the button (default ``"Untitled"``).

Parameter ``icon``:
    The icon to display with this Button. See nanogui::Button::mIcon.)doc";

static const char *__doc_nanogui_Button_Flags = R"doc(Flags to specify the button behavior (can be combined with binary OR))doc";

static const char *__doc_nanogui_Button_Flags_NormalButton = R"doc(A normal Button.)doc";

static const char *__doc_nanogui_Button_Flags_PopupButton = R"doc(A popup Button.)doc";

static const char *__doc_nanogui_Button_Flags_RadioButton = R"doc(A radio Button.)doc";

static const char *__doc_nanogui_Button_Flags_ToggleButton = R"doc(A toggle Button.)doc";

static const char *__doc_nanogui_Button_IconPosition = R"doc(The available icon positions.)doc";

static const char *__doc_nanogui_Button_IconPosition_Left = R"doc(Button icon on the far left.)doc";

static const char *__doc_nanogui_Button_IconPosition_LeftCentered = R"doc(Button icon on the left, centered (depends on caption text length).)doc";

static const char *__doc_nanogui_Button_IconPosition_Right = R"doc(Button icon on the far right.)doc";

static const char *__doc_nanogui_Button_IconPosition_RightCentered = R"doc(Button icon on the right, centered (depends on caption text length).)doc";

static const char *__doc_nanogui_Button_backgroundColor = R"doc(Returns the background color of this Button.)doc";

static const char *__doc_nanogui_Button_buttonGroup = R"doc(The current button group (for radio buttons).)doc";

static const char *__doc_nanogui_Button_callback = R"doc(The current callback to execute (for any type of button).)doc";

static const char *__doc_nanogui_Button_caption = R"doc(Returns the caption of this Button.)doc";

static const char *__doc_nanogui_Button_changeCallback = R"doc(The current callback to execute (for toggle buttons).)doc";

static const char *__doc_nanogui_Button_draw = R"doc(Responsible for drawing the Button.)doc";

static const char *__doc_nanogui_Button_flags =
R"doc(The current flags of this Button (see nanogui::Button::Flags for
options).)doc";

static const char *__doc_nanogui_Button_icon = R"doc(Returns the icon of this Button. See nanogui::Button::mIcon.)doc";

static const char *__doc_nanogui_Button_iconPosition = R"doc(The position of the icon for this Button.)doc";

static const char *__doc_nanogui_Button_load = R"doc(Sets the state of this Button provided the given Serializer.)doc";

static const char *__doc_nanogui_Button_mBackgroundColor = R"doc(The background color of this Button.)doc";

static const char *__doc_nanogui_Button_mButtonGroup = R"doc(The button group for radio buttons.)doc";

static const char *__doc_nanogui_Button_mCallback = R"doc(The callback issued for all types of buttons.)doc";

static const char *__doc_nanogui_Button_mCaption = R"doc(The caption of this Button.)doc";

static const char *__doc_nanogui_Button_mChangeCallback = R"doc(The callback issued for toggle buttons.)doc";

static const char *__doc_nanogui_Button_mFlags =
R"doc(The current flags of this button (see nanogui::Button::Flags for
options).)doc";

static const char *__doc_nanogui_Button_mIcon =
R"doc(The icon of this Button (``0`` means no icon).

```
The icon to display with this Button.  If not ``0``, may either be a
picture icon, or one of the icons enumerated in
:ref:`file_nanogui_entypo.h`.  The kind of icon (image or Entypo)
is determined by the functions :func:`nanogui::nvgIsImageIcon` and its
reciprocal counterpart :func:`nanogui::nvgIsFontIcon`.

```)doc";

static const char *__doc_nanogui_Button_mIconPosition = R"doc(The position to draw the icon at.)doc";

static const char *__doc_nanogui_Button_mPushed = R"doc(Whether or not this Button is currently pushed.)doc";

static const char *__doc_nanogui_Button_mTextColor = R"doc(The color of the caption text of this Button.)doc";

static const char *__doc_nanogui_Button_mouseButtonEvent =
R"doc(The callback that is called when any type of mouse button event is
issued to this Button.)doc";

static const char *__doc_nanogui_Button_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Button_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Button_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Button_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Button_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Button_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Button_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Button_operator_new = R"doc()doc";

static const char *__doc_nanogui_Button_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Button_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Button_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Button_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_Button_preferredSize = R"doc(The preferred size of this Button.)doc";

static const char *__doc_nanogui_Button_pushed = R"doc(Whether or not this Button is currently pushed.)doc";

static const char *__doc_nanogui_Button_save = R"doc(Saves the state of this Button provided the given Serializer.)doc";

static const char *__doc_nanogui_Button_setBackgroundColor = R"doc(Sets the background color of this Button.)doc";

static const char *__doc_nanogui_Button_setButtonGroup = R"doc(Set the button group (for radio buttons).)doc";

static const char *__doc_nanogui_Button_setCallback = R"doc(Set the push callback (for any type of button).)doc";

static const char *__doc_nanogui_Button_setCaption = R"doc(Sets the caption of this Button.)doc";

static const char *__doc_nanogui_Button_setChangeCallback = R"doc(Set the change callback (for toggle buttons).)doc";

static const char *__doc_nanogui_Button_setFlags =
R"doc(Sets the flags of this Button (see nanogui::Button::Flags for
options).)doc";

static const char *__doc_nanogui_Button_setIcon = R"doc(Sets the icon of this Button. See nanogui::Button::mIcon.)doc";

static const char *__doc_nanogui_Button_setIconPosition = R"doc(Sets the position of the icon for this Button.)doc";

static const char *__doc_nanogui_Button_setPushed = R"doc(Sets whether or not this Button is currently pushed.)doc";

static const char *__doc_nanogui_Button_setTextColor = R"doc(Sets the text color of the caption of this Button.)doc";

static const char *__doc_nanogui_Button_textColor = R"doc(Returns the text color of the caption of this Button.)doc";

static const char *__doc_nanogui_CheckBox =
R"doc(Two-state check box widget.

Remark:
    This class overrides nanogui::Widget::mIconExtraScale to be
    ``1.2f``, which affects all subclasses of this Widget. Subclasses
    must explicitly set a different value if needed (e.g., in their
    constructor).)doc";

static const char *__doc_nanogui_CheckBox_CheckBox =
R"doc(Adds a CheckBox to the specified ``parent``.

Parameter ``parent``:
    The Widget to add this CheckBox to.

Parameter ``caption``:
    The caption text of the CheckBox (default ``"Untitled"``).

Parameter ``callback``:
    If provided, the callback to execute when the CheckBox is checked
    or unchecked. Default parameter function does nothing. See
    nanogui::CheckBox::mPushed for the difference between "pushed" and
    "checked".)doc";

static const char *__doc_nanogui_CheckBox_callback = R"doc(Returns the current callback of this CheckBox.)doc";

static const char *__doc_nanogui_CheckBox_caption = R"doc(The caption of this CheckBox.)doc";

static const char *__doc_nanogui_CheckBox_checked = R"doc(Whether or not this CheckBox is currently checked.)doc";

static const char *__doc_nanogui_CheckBox_draw = R"doc(Draws this CheckBox.)doc";

static const char *__doc_nanogui_CheckBox_load = R"doc(Loads the state of the specified Serializer to this CheckBox.)doc";

static const char *__doc_nanogui_CheckBox_mCallback = R"doc(The function to execute when nanogui::CheckBox::mChecked is changed.)doc";

static const char *__doc_nanogui_CheckBox_mCaption = R"doc(The caption text of this CheckBox.)doc";

static const char *__doc_nanogui_CheckBox_mChecked = R"doc(Whether or not this CheckBox is currently checked or unchecked.)doc";

static const char *__doc_nanogui_CheckBox_mPushed =
R"doc(Internal tracking variable to distinguish between mouse click and
release. nanogui::CheckBox::mCallback is only called upon release. See
nanogui::CheckBox::mouseButtonEvent for specific conditions.)doc";

static const char *__doc_nanogui_CheckBox_mouseButtonEvent =
R"doc(The mouse button callback will return ``True`` when all three
conditions are met:

1. This CheckBox is "enabled" (see nanogui::Widget::mEnabled). 2.
``p`` is inside this CheckBox. 3. ``button`` is
``GLFW_MOUSE_BUTTON_1`` (left mouse click).

Since a mouse button event is issued for both when the mouse is
pressed, as well as released, this function sets
nanogui::CheckBox::mPushed to ``True`` when parameter ``down ==
true``. When the second event (``down == false``) is fired,
nanogui::CheckBox::mChecked is inverted and
nanogui::CheckBox::mCallback is called.

That is, the callback provided is only called when the mouse button is
released, **and** the click location remains within the CheckBox
boundaries. If the user clicks on the CheckBox and releases away from
the bounds of the CheckBox, nanogui::CheckBox::mPushed is simply set
back to ``False``.)doc";

static const char *__doc_nanogui_CheckBox_operator_delete = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_new = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_CheckBox_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_CheckBox_preferredSize = R"doc(The preferred size of this CheckBox.)doc";

static const char *__doc_nanogui_CheckBox_pushed =
R"doc(Whether or not this CheckBox is currently pushed. See
nanogui::CheckBox::mPushed.)doc";

static const char *__doc_nanogui_CheckBox_save = R"doc(Saves this CheckBox to the specified Serializer.)doc";

static const char *__doc_nanogui_CheckBox_setCallback =
R"doc(Sets the callback to be executed when this CheckBox is checked /
unchecked.)doc";

static const char *__doc_nanogui_CheckBox_setCaption = R"doc(Sets the caption of this CheckBox.)doc";

static const char *__doc_nanogui_CheckBox_setChecked = R"doc(Sets whether or not this CheckBox is currently checked.)doc";

static const char *__doc_nanogui_CheckBox_setPushed =
R"doc(Sets whether or not this CheckBox is currently pushed. See
nanogui::CheckBox::mPushed.)doc";

static const char *__doc_nanogui_Color =
R"doc(Stores an RGBA floating point color value.

This class simply wraps around an ``Eigen::Vector4f``, providing some
convenient methods and terminology for thinking of it as a color. The
data operates in the same way as ``Eigen::Vector4f``, and the
following values are identical:

```
+---------+-------------+-----------------------+-------------+
| Channel | Array Index | Eigen::Vector4f Value | Color Value |
+=========+=============+=======================+=============+
| Red     | ``0``       | x()                   | r()         |
+---------+-------------+-----------------------+-------------+
| Green   | ``1``       | y()                   | g()         |
+---------+-------------+-----------------------+-------------+
| Blue    | ``2``       | z()                   | b()         |
+---------+-------------+-----------------------+-------------+
| Alpha   | ``3``       | w()                   | w()         |
+---------+-------------+-----------------------+-------------+
Note:
The method for the alpha component is **always** ``w()``.

```

You can and should still use the various convenience methods such as
``any()``, ``all()``, ``head<index>()``, etc provided by Eigen.)doc";

static const char *__doc_nanogui_ColorPicker =
R"doc(Push button with a popup to tweak a color value. This widget was
contributed by Christian Schueller.)doc";

static const char *__doc_nanogui_ColorPicker_ColorPicker =
R"doc(Attaches a ColorPicker to the specified parent.

Parameter ``parent``:
    The Widget to add this ColorPicker to.

Parameter ``color``:
    The color initially selected by this ColorPicker (default: Red).)doc";

static const char *__doc_nanogui_ColorPicker_callback = R"doc(The callback executed when the ColorWheel changes.)doc";

static const char *__doc_nanogui_ColorPicker_color = R"doc(Get the current Color selected for this ColorPicker.)doc";

static const char *__doc_nanogui_ColorPicker_finalCallback =
R"doc(The callback to execute when a new Color is selected on the ColorWheel
**and** the user clicks the nanogui::ColorPicker::mPickButton or
nanogui::ColorPicker::mResetButton.)doc";

static const char *__doc_nanogui_ColorPicker_mCallback = R"doc(The "fast" callback executed when the ColorWheel has changed.)doc";

static const char *__doc_nanogui_ColorPicker_mColorWheel =
R"doc(The ColorWheel for this ColorPicker (the actual widget allowing
selection).)doc";

static const char *__doc_nanogui_ColorPicker_mFinalCallback =
R"doc(The callback to execute when a new Color is selected on the ColorWheel
**and** the user clicks the nanogui::ColorPicker::mPickButton or
nanogui::ColorPicker::mResetButton.)doc";

static const char *__doc_nanogui_ColorPicker_mPickButton =
R"doc(The Button used to signal that the current value on the ColorWheel is
the desired color to be chosen. The default value for the caption of
this Button is ``"Pick"``. You can change it using
nanogui::ColorPicker::setPickButtonCaption if you need.

The color of this Button will not affect nanogui::ColorPicker::color
until the user has actively selected by clicking this pick button.
Similarly, the nanogui::ColorPicker::mCallback function is only called
when a user selects a new Color using by clicking this Button.)doc";

static const char *__doc_nanogui_ColorPicker_mResetButton =
R"doc(Remains the Color of the active color selection, until the user picks
a new Color on the ColorWheel **and** selects the
nanogui::ColorPicker::mPickButton. The default value for the caption
of this Button is ``"Reset"``. You can change it using
nanogui::ColorPicker::setResetButtonCaption if you need.)doc";

static const char *__doc_nanogui_ColorPicker_operator_delete = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_new = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_pickButtonCaption = R"doc(The current caption of the nanogui::ColorPicker::mPickButton.)doc";

static const char *__doc_nanogui_ColorPicker_resetButtonCaption = R"doc(The current caption of the nanogui::ColorPicker::mResetButton.)doc";

static const char *__doc_nanogui_ColorPicker_setCallback =
R"doc(Sets the callback is executed as the ColorWheel itself is changed. Set
this callback if you need to receive updates for the ColorWheel
changing before the user clicks nanogui::ColorPicker::mPickButton or
nanogui::ColorPicker::mPickButton.)doc";

static const char *__doc_nanogui_ColorPicker_setColor = R"doc(Set the current Color selected for this ColorPicker.)doc";

static const char *__doc_nanogui_ColorPicker_setFinalCallback =
R"doc(The callback to execute when a new Color is selected on the ColorWheel
**and** the user clicks the nanogui::ColorPicker::mPickButton or
nanogui::ColorPicker::mResetButton.)doc";

static const char *__doc_nanogui_ColorPicker_setPickButtonCaption = R"doc(Sets the current caption of the nanogui::ColorPicker::mPickButton.)doc";

static const char *__doc_nanogui_ColorPicker_setResetButtonCaption = R"doc(Sets the current caption of the nanogui::ColorPicker::mResetButton.)doc";

static const char *__doc_nanogui_ColorWheel =
R"doc(Fancy analog widget to select a color value. This widget was
contributed by Dmitriy Morozov.)doc";

static const char *__doc_nanogui_ColorWheel_ColorWheel =
R"doc(Adds a ColorWheel to the specified parent.

Parameter ``parent``:
    The Widget to add this ColorWheel to.

Parameter ``color``:
    The initial color of the ColorWheel (default: Red).)doc";

static const char *__doc_nanogui_ColorWheel_Region = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_Region_Both = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_Region_InnerTriangle = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_Region_None = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_Region_OuterCircle = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_adjustPosition = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_callback = R"doc(The callback to execute when a user changes the ColorWheel value.)doc";

static const char *__doc_nanogui_ColorWheel_color = R"doc(The current Color this ColorWheel has selected.)doc";

static const char *__doc_nanogui_ColorWheel_draw = R"doc(Draws the ColorWheel.)doc";

static const char *__doc_nanogui_ColorWheel_hue2rgb = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_load = R"doc(Sets the state of this ColorWheel using the specified Serializer.)doc";

static const char *__doc_nanogui_ColorWheel_mBlack =
R"doc(The implicit Saturation component of the HSV color model. See
implementation nanogui::ColorWheel::color for its usage. Valid values
are in the range ``[0, 1]``.)doc";

static const char *__doc_nanogui_ColorWheel_mCallback = R"doc(The current callback to execute when the color value has changed.)doc";

static const char *__doc_nanogui_ColorWheel_mDragRegion = R"doc(The current region the mouse is interacting with.)doc";

static const char *__doc_nanogui_ColorWheel_mHue = R"doc(The current Hue in the HSV color model.)doc";

static const char *__doc_nanogui_ColorWheel_mWhite =
R"doc(The implicit Value component of the HSV color model. See
implementation nanogui::ColorWheel::color for its usage. Valid values
are in the range ``[0, 1]``.)doc";

static const char *__doc_nanogui_ColorWheel_mouseButtonEvent = R"doc(Handles mouse button click events for the ColorWheel.)doc";

static const char *__doc_nanogui_ColorWheel_mouseDragEvent = R"doc(Handles mouse drag events for the ColorWheel.)doc";

static const char *__doc_nanogui_ColorWheel_operator_delete = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_new = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_preferredSize = R"doc(The preferred size of this ColorWheel.)doc";

static const char *__doc_nanogui_ColorWheel_save =
R"doc(Saves the current state of this ColorWheel to the specified
Serializer.)doc";

static const char *__doc_nanogui_ColorWheel_setCallback = R"doc(Sets the callback to execute when a user changes the ColorWheel value.)doc";

static const char *__doc_nanogui_ColorWheel_setColor = R"doc(Sets the current Color this ColorWheel has selected.)doc";

static const char *__doc_nanogui_Color_Color = R"doc(Default constructor: represents black (``r, g, b, a = 0``))doc";

static const char *__doc_nanogui_Color_Color_10 =
R"doc(Explicit constructor: creates the Color ``(r, g, b, a)``.

Parameter ``r``:
    The red component of the color.

Parameter ``g``:
    The green component of the color.

Parameter ``b``:
    The blue component of the color.

Parameter ``a``:
    The alpha component of the color.)doc";

static const char *__doc_nanogui_Color_Color_11 =
R"doc(Explicit constructor: creates the Color ``(r, g, b, a) / 255.0``.
Values are casted to floats before division.

Parameter ``r``:
    The red component of the color, will be divided by ``255.0``.

Parameter ``g``:
    The green component of the color, will be divided by ``255.0``.

Parameter ``b``:
    The blue component of the color, will be divided by ``255.0``.

Parameter ``a``:
    The alpha component of the color, will be divided by ``255.0``.)doc";

static const char *__doc_nanogui_Color_Color_12 =
R"doc(Construct a color vector from MatrixBase (needed to play nice with
Eigen))doc";

static const char *__doc_nanogui_Color_Color_2 =
R"doc(Makes an exact copy of the data represented by the input parameter.

Parameter ``color``:
    The four dimensional float vector being copied.)doc";

static const char *__doc_nanogui_Color_Color_3 =
R"doc(Copies (x, y, z) from the input vector, and uses the value specified
by the ``alpha`` parameter for this Color object's alpha component.

Parameter ``color``:
    The three dimensional float vector being copied.

Parameter ``alpha``:
    The value to set this object's alpha component to.)doc";

static const char *__doc_nanogui_Color_Color_4 =
R"doc(Copies (x, y, z) from the input vector, casted as floats first and
then divided by ``255.0``, and uses the value specified by the
``alpha`` parameter, casted to a float and divided by ``255.0`` as
well, for this Color object's alpha component.

Parameter ``color``:
    The three dimensional integer vector being copied, will be divided
    by ``255.0``.

Parameter ``alpha``:
    The value to set this object's alpha component to, will be divided
    by ``255.0``.)doc";

static const char *__doc_nanogui_Color_Color_5 =
R"doc(Copies (x, y, z) from the input vector, and sets the alpha of this
color to be ``1.0``.

Parameter ``color``:
    The three dimensional float vector being copied.)doc";

static const char *__doc_nanogui_Color_Color_6 =
R"doc(Copies (x, y, z) from the input vector, casting to floats and dividing
by ``255.0``. The alpha of this color will be set to ``1.0``.

Parameter ``color``:
    The three dimensional integer vector being copied, will be divided
    by ``255.0``.)doc";

static const char *__doc_nanogui_Color_Color_7 =
R"doc(Copies (x, y, z, w) from the input vector, casting to floats and
dividing by ``255.0``.

Parameter ``color``:
    The three dimensional integer vector being copied, will be divided
    by ``255.0``.)doc";

static const char *__doc_nanogui_Color_Color_8 =
R"doc(Creates the Color ``(intensity, intensity, intensity, alpha)``.

Parameter ``intensity``:
    The value to be used for red, green, and blue.

Parameter ``alpha``:
    The alpha component of the color.)doc";

static const char *__doc_nanogui_Color_Color_9 =
R"doc(Creates the Color ``(intensity, intensity, intensity, alpha) /
255.0``. Values are casted to floats before division.

Parameter ``intensity``:
    The value to be used for red, green, and blue, will be divided by
    ``255.0``.

Parameter ``alpha``:
    The alpha component of the color, will be divided by ``255.0``.)doc";

static const char *__doc_nanogui_Color_b = R"doc(Return a reference to the blue channel)doc";

static const char *__doc_nanogui_Color_b_2 = R"doc(Return a reference to the blue channel (const version))doc";

static const char *__doc_nanogui_Color_contrastingColor =
R"doc(Computes the luminance as ``l = 0.299r + 0.587g + 0.144b + 0.0a``. If
the luminance is less than 0.5, white is returned. If the luminance is
greater than or equal to 0.5, black is returned. Both returns will
have an alpha component of 1.0.)doc";

static const char *__doc_nanogui_Color_g = R"doc(Return a reference to the green channel)doc";

static const char *__doc_nanogui_Color_g_2 = R"doc(Return a reference to the green channel (const version))doc";

static const char *__doc_nanogui_Color_operator_assign = R"doc(Assign a color vector from MatrixBase (needed to play nice with Eigen))doc";

static const char *__doc_nanogui_Color_operator_const_NVGcolor = R"doc(Allows for conversion between this Color and NanoVG's representation.)doc";

static const char *__doc_nanogui_Color_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Color_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Color_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Color_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Color_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Color_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Color_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Color_operator_new = R"doc()doc";

static const char *__doc_nanogui_Color_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Color_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Color_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Color_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_Color_r = R"doc(Return a reference to the red channel)doc";

static const char *__doc_nanogui_Color_r_2 = R"doc(Return a reference to the red channel (const version))doc";

static const char *__doc_nanogui_ComboBox = R"doc(Simple combo box widget based on a popup button.)doc";

static const char *__doc_nanogui_ComboBox_ComboBox = R"doc(Create an empty combo box)doc";

static const char *__doc_nanogui_ComboBox_ComboBox_2 = R"doc(Create a new combo box with the given items)doc";

static const char *__doc_nanogui_ComboBox_ComboBox_3 =
R"doc(Create a new combo box with the given items, providing both short and
long descriptive labels for each item)doc";

static const char *__doc_nanogui_ComboBox_callback = R"doc(The callback to execute for this ComboBox.)doc";

static const char *__doc_nanogui_ComboBox_items = R"doc(The items associated with this ComboBox.)doc";

static const char *__doc_nanogui_ComboBox_itemsShort = R"doc(The short descriptions associated with this ComboBox.)doc";

static const char *__doc_nanogui_ComboBox_load = R"doc(Sets the state of this ComboBox from the specified Serializer.)doc";

static const char *__doc_nanogui_ComboBox_mCallback = R"doc(The callback for this ComboBox.)doc";

static const char *__doc_nanogui_ComboBox_mItems = R"doc(The items associated with this ComboBox.)doc";

static const char *__doc_nanogui_ComboBox_mItemsShort = R"doc(The short descriptions of items associated with this ComboBox.)doc";

static const char *__doc_nanogui_ComboBox_mSelectedIndex = R"doc(The current index this ComboBox has selected.)doc";

static const char *__doc_nanogui_ComboBox_operator_delete = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_new = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_save = R"doc(Saves the state of this ComboBox to the specified Serializer.)doc";

static const char *__doc_nanogui_ComboBox_scrollEvent = R"doc(Handles mouse scrolling events for this ComboBox.)doc";

static const char *__doc_nanogui_ComboBox_selectedIndex = R"doc(The current index this ComboBox has selected.)doc";

static const char *__doc_nanogui_ComboBox_setCallback = R"doc(Sets the callback to execute for this ComboBox.)doc";

static const char *__doc_nanogui_ComboBox_setItems =
R"doc(Sets the items for this ComboBox, providing both short and long
descriptive lables for each item.)doc";

static const char *__doc_nanogui_ComboBox_setItems_2 = R"doc(Sets the items for this ComboBox.)doc";

static const char *__doc_nanogui_ComboBox_setSelectedIndex = R"doc(Sets the current index this ComboBox has selected.)doc";

static const char *__doc_nanogui_Cursor =
R"doc(Cursor shapes available to use in GLFW. Shape of actual cursor
determined by Operating System.)doc";

static const char *__doc_nanogui_Cursor_Arrow = R"doc(The arrow cursor.)doc";

static const char *__doc_nanogui_Cursor_Crosshair = R"doc(The crosshair cursor.)doc";

static const char *__doc_nanogui_Cursor_CursorCount =
R"doc(Not a cursor --- should always be last: enables a loop over the cursor
types.)doc";

static const char *__doc_nanogui_Cursor_HResize = R"doc(The horizontal resize cursor.)doc";

static const char *__doc_nanogui_Cursor_Hand = R"doc(The hand cursor.)doc";

static const char *__doc_nanogui_Cursor_IBeam = R"doc(The I-beam cursor.)doc";

static const char *__doc_nanogui_Cursor_VResize = R"doc(The vertical resize cursor.)doc";

static const char *__doc_nanogui_FloatBox =
R"doc(A specialization of TextBox representing floating point values.

Template parameters should be float types, e.g. ``float``, ``double``,
``float64_t``, etc.)doc";

static const char *__doc_nanogui_FloatBox_FloatBox = R"doc()doc";

static const char *__doc_nanogui_FloatBox_mMaxValue = R"doc()doc";

static const char *__doc_nanogui_FloatBox_mMinValue = R"doc()doc";

static const char *__doc_nanogui_FloatBox_mMouseDownValue = R"doc()doc";

static const char *__doc_nanogui_FloatBox_mNumberFormat = R"doc()doc";

static const char *__doc_nanogui_FloatBox_mValueIncrement = R"doc()doc";

static const char *__doc_nanogui_FloatBox_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_FloatBox_mouseDragEvent = R"doc()doc";

static const char *__doc_nanogui_FloatBox_numberFormat = R"doc()doc";

static const char *__doc_nanogui_FloatBox_numberFormat_2 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_delete = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_new = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_FloatBox_scrollEvent = R"doc()doc";

static const char *__doc_nanogui_FloatBox_setCallback = R"doc()doc";

static const char *__doc_nanogui_FloatBox_setMaxValue = R"doc()doc";

static const char *__doc_nanogui_FloatBox_setMinMaxValues = R"doc()doc";

static const char *__doc_nanogui_FloatBox_setMinValue = R"doc()doc";

static const char *__doc_nanogui_FloatBox_setValue = R"doc()doc";

static const char *__doc_nanogui_FloatBox_setValueIncrement = R"doc()doc";

static const char *__doc_nanogui_FloatBox_value = R"doc()doc";

static const char *__doc_nanogui_FormHelper =
R"doc(Convenience class to create simple AntTweakBar-style layouts that
expose variables of various types using NanoGUI widgets

**Example**:

```
// [ ... initialize NanoGUI, construct screen ... ]
FormHelper* h = new FormHelper(screen);
// Add a new windows widget
h->addWindow(Eigen::Vector2i(10,10),"Menu");
// Start a new group
h->addGroup("Group 1");
// Expose an integer variable by reference
h->addVariable("integer variable", aInt);
// Expose a float variable via setter/getter functions
h->addVariable(
  [&](float value) { aFloat = value; },
  [&]() { return *aFloat; },
  "float variable");
// add a new button
h->addButton("Button", [&]() { std::cout << "Button pressed" << std::endl; });

```)doc";

static const char *__doc_nanogui_FormHelper_FormHelper = R"doc(Create a helper class to construct NanoGUI widgets on the given screen)doc";

static const char *__doc_nanogui_FormHelper_addButton = R"doc(Add a button with a custom callback)doc";

static const char *__doc_nanogui_FormHelper_addGroup = R"doc(Add a new group that may contain several sub-widgets)doc";

static const char *__doc_nanogui_FormHelper_addVariable = R"doc(Add a new data widget controlled using custom getter/setter functions)doc";

static const char *__doc_nanogui_FormHelper_addVariable_2 = R"doc(Add a new data widget that exposes a raw variable in memory)doc";

static const char *__doc_nanogui_FormHelper_addWidget = R"doc(Add an arbitrary (optionally labeled) widget to the layout)doc";

static const char *__doc_nanogui_FormHelper_addWindow = R"doc(Add a new top-level window)doc";

static const char *__doc_nanogui_FormHelper_fixedSize = R"doc(The current fixed size being used for newly added widgets.)doc";

static const char *__doc_nanogui_FormHelper_groupFontName = R"doc(The font name being used for group headers.)doc";

static const char *__doc_nanogui_FormHelper_groupFontSize = R"doc(The size of the font being used for group headers.)doc";

static const char *__doc_nanogui_FormHelper_labelFontName = R"doc(The font name being used for labels.)doc";

static const char *__doc_nanogui_FormHelper_labelFontSize = R"doc(The size of the font being used for labels.)doc";

static const char *__doc_nanogui_FormHelper_mFixedSize = R"doc(The fixed size for newly added widgets.)doc";

static const char *__doc_nanogui_FormHelper_mGroupFontName = R"doc(The group header font name.)doc";

static const char *__doc_nanogui_FormHelper_mGroupFontSize = R"doc(The font size for group headers.)doc";

static const char *__doc_nanogui_FormHelper_mLabelFontName = R"doc(The label font name.)doc";

static const char *__doc_nanogui_FormHelper_mLabelFontSize = R"doc(The font size for labels.)doc";

static const char *__doc_nanogui_FormHelper_mLayout =
R"doc(A reference to the nanogui::AdvancedGridLayout this FormHelper is
using.)doc";

static const char *__doc_nanogui_FormHelper_mPostGroupSpacing = R"doc(The spacing used **after** each group.)doc";

static const char *__doc_nanogui_FormHelper_mPreGroupSpacing = R"doc(The spacing used **before** new groups.)doc";

static const char *__doc_nanogui_FormHelper_mRefreshCallbacks = R"doc(The callbacks associated with all widgets this FormHelper is managing.)doc";

static const char *__doc_nanogui_FormHelper_mScreen = R"doc(A reference to the nanogui::Screen this FormHelper is assisting.)doc";

static const char *__doc_nanogui_FormHelper_mVariableSpacing = R"doc(The spacing between all other widgets.)doc";

static const char *__doc_nanogui_FormHelper_mWidgetFontSize = R"doc(The font size for non-group / non-label widgets.)doc";

static const char *__doc_nanogui_FormHelper_mWindow = R"doc(A reference to the nanogui::Window this FormHelper is controlling.)doc";

static const char *__doc_nanogui_FormHelper_operator_delete = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_new = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_FormHelper_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_FormHelper_refresh = R"doc(Cause all widgets to re-synchronize with the underlying variable state)doc";

static const char *__doc_nanogui_FormHelper_setFixedSize = R"doc(Specify a fixed size for newly added widgets.)doc";

static const char *__doc_nanogui_FormHelper_setGroupFontName = R"doc(Sets the font name to be used for group headers.)doc";

static const char *__doc_nanogui_FormHelper_setGroupFontSize = R"doc(Sets the size of the font being used for group headers.)doc";

static const char *__doc_nanogui_FormHelper_setLabelFontName = R"doc(Sets the font name being used for labels.)doc";

static const char *__doc_nanogui_FormHelper_setLabelFontSize = R"doc(Sets the size of the font being used for labels.)doc";

static const char *__doc_nanogui_FormHelper_setWidgetFontSize =
R"doc(Sets the size of the font being used for non-group / non-label
widgets.)doc";

static const char *__doc_nanogui_FormHelper_setWindow = R"doc(Set the active Window instance.)doc";

static const char *__doc_nanogui_FormHelper_widgetFontSize = R"doc(The size of the font being used for non-group / non-label widgets.)doc";

static const char *__doc_nanogui_FormHelper_window = R"doc(Access the currently active Window instance)doc";

static const char *__doc_nanogui_GLCanvas =
R"doc(Canvas widget for rendering OpenGL content. This widget was
contributed by Jan Winkler.

Canvas widget that can be used to display arbitrary OpenGL content.
This is useful to display and manipulate 3D objects as part of an
interactive application. The implementation uses scissoring to ensure
that rendered objects don't spill into neighboring widgets.

```
**Usage**
 Override :func:`nanogui::GLCanvas::drawGL` in subclasses to provide
 custom drawing code.  See :ref:`nanogui_example_4`.

```)doc";

static const char *__doc_nanogui_GLCanvas_GLCanvas =
R"doc(Creates a GLCanvas attached to the specified parent.

Parameter ``parent``:
    The Widget to attach this GLCanvas to.)doc";

static const char *__doc_nanogui_GLCanvas_backgroundColor = R"doc(Returns the background color.)doc";

static const char *__doc_nanogui_GLCanvas_draw = R"doc(Draw the canvas.)doc";

static const char *__doc_nanogui_GLCanvas_drawBorder = R"doc(Return whether the widget border gets drawn or not.)doc";

static const char *__doc_nanogui_GLCanvas_drawGL = R"doc(Draw the GL scene. Override this method to draw the actual GL content.)doc";

static const char *__doc_nanogui_GLCanvas_drawWidgetBorder = R"doc(Internal helper function for drawing the widget border)doc";

static const char *__doc_nanogui_GLCanvas_load = R"doc(Set the state of this GLCanvas from the specified Serializer.)doc";

static const char *__doc_nanogui_GLCanvas_mBackgroundColor = R"doc(The background color (what is used with ``glClearColor``).)doc";

static const char *__doc_nanogui_GLCanvas_mDrawBorder = R"doc(Whether to draw the widget border or not.)doc";

static const char *__doc_nanogui_GLCanvas_operator_delete = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_new = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_GLCanvas_save = R"doc(Save the state of this GLCanvas to the specified Serializer.)doc";

static const char *__doc_nanogui_GLCanvas_setBackgroundColor = R"doc(Sets the background color.)doc";

static const char *__doc_nanogui_GLCanvas_setDrawBorder = R"doc(Set whether to draw the widget border or not.)doc";

static const char *__doc_nanogui_GLFramebuffer = R"doc(Helper class for creating framebuffer objects.)doc";

static const char *__doc_nanogui_GLFramebuffer_GLFramebuffer = R"doc(Default constructor: unusable until you call the ``init()`` method)doc";

static const char *__doc_nanogui_GLFramebuffer_bind = R"doc(Bind the framebuffer object)doc";

static const char *__doc_nanogui_GLFramebuffer_blit = R"doc(Blit the framebuffer object onto the screen)doc";

static const char *__doc_nanogui_GLFramebuffer_downloadTGA =
R"doc(Quick and dirty method to write a TGA (32bpp RGBA) file of the
framebuffer contents for debugging)doc";

static const char *__doc_nanogui_GLFramebuffer_free = R"doc(Release all associated resources)doc";

static const char *__doc_nanogui_GLFramebuffer_init =
R"doc(Create a new framebuffer with the specified size and number of MSAA
samples)doc";

static const char *__doc_nanogui_GLFramebuffer_mColor = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_mDepth = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_mFramebuffer = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_mSamples = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_mSize = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_delete = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_new = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_ready = R"doc(Return whether or not the framebuffer object has been initialized)doc";

static const char *__doc_nanogui_GLFramebuffer_release = R"doc(Release/unbind the framebuffer object)doc";

static const char *__doc_nanogui_GLFramebuffer_samples = R"doc(Return the number of MSAA samples)doc";

static const char *__doc_nanogui_GLShader =
R"doc(Helper class for compiling and linking OpenGL shaders and uploading
associated vertex and index buffers from Eigen matrices.)doc";

static const char *__doc_nanogui_GLShader_Buffer =
R"doc(A wrapper struct for maintaining various aspects of items being
managed by OpenGL. Buffers are created when GLShader::uploadAttrib is
called.)doc";

static const char *__doc_nanogui_GLShader_Buffer_compSize = R"doc(The size (in bytes) of an individual element in this buffer.)doc";

static const char *__doc_nanogui_GLShader_Buffer_dim = R"doc(The dimension of this buffer (typically the row width).)doc";

static const char *__doc_nanogui_GLShader_Buffer_glType = R"doc(The OpenGL type of this buffer.)doc";

static const char *__doc_nanogui_GLShader_Buffer_id = R"doc(The identifier used with OpenGL.)doc";

static const char *__doc_nanogui_GLShader_Buffer_size = R"doc(The total number of elements represented by this buffer.)doc";

static const char *__doc_nanogui_GLShader_Buffer_version = R"doc(The current version if this buffer.)doc";

static const char *__doc_nanogui_GLShader_GLShader = R"doc(Create an unitialized OpenGL shader)doc";

static const char *__doc_nanogui_GLShader_attrib =
R"doc(Return the handle of a named shader attribute (-1 if it does not
exist))doc";

static const char *__doc_nanogui_GLShader_attribBuffer =
R"doc((Advanced) Returns a reference to the specified GLShader::Buffer.

```
Danger:
Extreme caution must be exercised when using this method.  The user is
discouraged from explicitly storing the reference returned, as it can
change, become deprecated, or no longer reside in
:member:`mBufferObjects <nanogui::GLShader::mBufferObjects>`.
There are generally very few use cases that justify using this method
directly.  For example, if you need the version of a buffer, call
:func:`attribVersion <nanogui::GLShader::attribVersion>`.  If you want
to share data between :class:`GLShader <nanogui::GLShader>` objects,
call :func:`shareAttrib <nanogui::GLShader::shareAttrib>`.
One example use case for this method is sharing data between different
GPU pipelines such as CUDA or OpenCL.  When sharing data, you
typically need to map pointers between the API's.  The returned
buffer's :member:`Buffer::id <nanogui::GLShader::Buffer::id>` is the
``GLuint`` you will want to map to the other API.
In short, only use this method if you absolutely need to.

```

Parameter ``name``:
    The name of the desired attribute.

Returns:
    A reference to the current buffer associated with ``name``. Should
    not be explicitly stored.

Throws:
    std::runtime_error If ``name`` is not found.)doc";

static const char *__doc_nanogui_GLShader_attribVersion = R"doc(Return the version number of a given attribute)doc";

static const char *__doc_nanogui_GLShader_bind =
R"doc(Select this shader for subsequent draw calls. Simply executes
``glUseProgram`` with mProgramShader, and ``glBindVertexArray`` with
mVertexArrayObject.)doc";

static const char *__doc_nanogui_GLShader_bufferSize = R"doc(Return the size of all registered buffers in bytes)doc";

static const char *__doc_nanogui_GLShader_define =
R"doc(Set a preprocessor definition. Custom preprocessor definitions must be
added **before** initializing the shader (e.g., via initFromFiles).
See also: mDefinitions.)doc";

static const char *__doc_nanogui_GLShader_downloadAttrib = R"doc(Download a vertex buffer object into an Eigen matrix)doc";

static const char *__doc_nanogui_GLShader_downloadAttrib_2 = R"doc()doc";

static const char *__doc_nanogui_GLShader_drawArray = R"doc(Draw a sequence of primitives)doc";

static const char *__doc_nanogui_GLShader_drawIndexed = R"doc(Draw a sequence of primitives using a previously uploaded index buffer)doc";

static const char *__doc_nanogui_GLShader_free = R"doc(Release underlying OpenGL objects)doc";

static const char *__doc_nanogui_GLShader_freeAttrib = R"doc(Completely free an existing attribute buffer)doc";

static const char *__doc_nanogui_GLShader_hasAttrib = R"doc(Check if an attribute was registered a given name)doc";

static const char *__doc_nanogui_GLShader_init =
R"doc(Initialize the shader using the specified source strings.

Parameter ``name``:
    The name this shader will be registered as.

Parameter ``vertex_str``:
    The source of the vertex shader as a string.

Parameter ``fragment_str``:
    The source of the fragment shader as a string.

Parameter ``geometry_str``:
    The source of the geometry shader as a string. The default value
    is the empty string, which indicates no geometry shader will be
    used.)doc";

static const char *__doc_nanogui_GLShader_initFromFiles =
R"doc(Initialize the shader using the specified files on disk.

Parameter ``name``:
    The name this shader will be registered as.

Parameter ``vertex_fname``:
    The path to the file containing the source of the fragment shader.

Parameter ``fragment_fname``:
    The path to the file containing the source of the vertex shader.

Parameter ``geometry_fname``:
    The path to the file containing the source of the geometry shader.
    The default value is the empty string, which indicates no geometry
    shader will be used.)doc";

static const char *__doc_nanogui_GLShader_invalidateAttribs = R"doc(Invalidate the version numbers associated with attribute data)doc";

static const char *__doc_nanogui_GLShader_mBufferObjects =
R"doc(The map of string names to buffer objects representing the various
attributes that have been uploaded using uploadAttrib.)doc";

static const char *__doc_nanogui_GLShader_mDefinitions =
R"doc(```
The map of preprocessor names to values (if any have been created).  If
a definition was added seeking to create ``#define WIDTH 256``, the key
would be ``"WIDTH"`` and the value would be ``"256"``.  These definitions
will be included automatically in the string that gets compiled for the
vertex, geometry, and fragment shader code.

```)doc";

static const char *__doc_nanogui_GLShader_mFragmentShader =
R"doc(The fragment shader of this GLShader (as returned by
``glCreateShader``).)doc";

static const char *__doc_nanogui_GLShader_mGeometryShader =
R"doc(The geometry shader (if requested) of this GLShader (as returned by
``glCreateShader``).)doc";

static const char *__doc_nanogui_GLShader_mName = R"doc(The registered name of this GLShader.)doc";

static const char *__doc_nanogui_GLShader_mProgramShader = R"doc(The OpenGL program (as returned by ``glCreateProgram``).)doc";

static const char *__doc_nanogui_GLShader_mVertexArrayObject =
R"doc(The vertex array associated with this GLShader (as returned by
``glGenVertexArrays``).)doc";

static const char *__doc_nanogui_GLShader_mVertexShader =
R"doc(The vertex shader of this GLShader (as returned by
``glCreateShader``).)doc";

static const char *__doc_nanogui_GLShader_name = R"doc(Return the name of the shader)doc";

static const char *__doc_nanogui_GLShader_resetAttribVersion = R"doc(Reset the version number of a given attribute)doc";

static const char *__doc_nanogui_GLShader_setUniform = R"doc(Initialize a uniform parameter with a 4x4 matrix (float))doc";

static const char *__doc_nanogui_GLShader_setUniform_10 = R"doc(Initialize a uniform parameter with a 3D vector (int))doc";

static const char *__doc_nanogui_GLShader_setUniform_11 = R"doc(Initialize a uniform parameter with a 3D vector (float))doc";

static const char *__doc_nanogui_GLShader_setUniform_12 = R"doc(Initialize a uniform parameter with a 4D vector (int))doc";

static const char *__doc_nanogui_GLShader_setUniform_13 = R"doc(Initialize a uniform parameter with a 4D vector (float))doc";

static const char *__doc_nanogui_GLShader_setUniform_14 = R"doc(Initialize a uniform buffer with a uniform buffer object)doc";

static const char *__doc_nanogui_GLShader_setUniform_2 = R"doc(Initialize a uniform parameter with a 3x3 affine transform (float))doc";

static const char *__doc_nanogui_GLShader_setUniform_3 = R"doc(Initialize a uniform parameter with a 3x3 matrix (float))doc";

static const char *__doc_nanogui_GLShader_setUniform_4 = R"doc(Initialize a uniform parameter with a 2x2 affine transform (float))doc";

static const char *__doc_nanogui_GLShader_setUniform_5 = R"doc(Initialize a uniform parameter with a boolean value)doc";

static const char *__doc_nanogui_GLShader_setUniform_6 = R"doc(Initialize a uniform parameter with an integer value)doc";

static const char *__doc_nanogui_GLShader_setUniform_7 = R"doc(Initialize a uniform parameter with a floating point value)doc";

static const char *__doc_nanogui_GLShader_setUniform_8 = R"doc(Initialize a uniform parameter with a 2D vector (int))doc";

static const char *__doc_nanogui_GLShader_setUniform_9 = R"doc(Initialize a uniform parameter with a 2D vector (float))doc";

static const char *__doc_nanogui_GLShader_shareAttrib =
R"doc(Create a symbolic link to an attribute of another GLShader. This
avoids duplicating unnecessary data)doc";

static const char *__doc_nanogui_GLShader_uniform = R"doc(Return the handle of a uniform attribute (-1 if it does not exist))doc";

static const char *__doc_nanogui_GLShader_uploadAttrib =
R"doc(Upload an Eigen matrix as a vertex buffer object (refreshing it as
needed))doc";

static const char *__doc_nanogui_GLShader_uploadAttrib_2 = R"doc()doc";

static const char *__doc_nanogui_GLShader_uploadIndices = R"doc(Upload an index buffer)doc";

static const char *__doc_nanogui_GLUniformBuffer = R"doc(Helper class for creating OpenGL Uniform Buffer objects.)doc";

static const char *__doc_nanogui_GLUniformBuffer_GLUniformBuffer = R"doc(Default constructor: unusable until you call the ``init()`` method)doc";

static const char *__doc_nanogui_GLUniformBuffer_bind = R"doc(Bind the uniform buffer to a specific binding point)doc";

static const char *__doc_nanogui_GLUniformBuffer_free = R"doc(Release underlying OpenGL object)doc";

static const char *__doc_nanogui_GLUniformBuffer_getBindingPoint = R"doc(Return the binding point of this uniform buffer)doc";

static const char *__doc_nanogui_GLUniformBuffer_init = R"doc(Create a new uniform buffer)doc";

static const char *__doc_nanogui_GLUniformBuffer_mBindingPoint = R"doc()doc";

static const char *__doc_nanogui_GLUniformBuffer_mID = R"doc()doc";

static const char *__doc_nanogui_GLUniformBuffer_release = R"doc(Release/unbind the uniform buffer)doc";

static const char *__doc_nanogui_GLUniformBuffer_update = R"doc(Update content on the GPU using data)doc";

static const char *__doc_nanogui_Graph = R"doc(Simple graph widget for showing a function plot.)doc";

static const char *__doc_nanogui_Graph_Graph = R"doc()doc";

static const char *__doc_nanogui_Graph_backgroundColor = R"doc()doc";

static const char *__doc_nanogui_Graph_caption = R"doc()doc";

static const char *__doc_nanogui_Graph_draw = R"doc()doc";

static const char *__doc_nanogui_Graph_footer = R"doc()doc";

static const char *__doc_nanogui_Graph_foregroundColor = R"doc()doc";

static const char *__doc_nanogui_Graph_header = R"doc()doc";

static const char *__doc_nanogui_Graph_load = R"doc()doc";

static const char *__doc_nanogui_Graph_mBackgroundColor = R"doc()doc";

static const char *__doc_nanogui_Graph_mCaption = R"doc()doc";

static const char *__doc_nanogui_Graph_mFooter = R"doc()doc";

static const char *__doc_nanogui_Graph_mForegroundColor = R"doc()doc";

static const char *__doc_nanogui_Graph_mHeader = R"doc()doc";

static const char *__doc_nanogui_Graph_mTextColor = R"doc()doc";

static const char *__doc_nanogui_Graph_mValues = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_new = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Graph_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_Graph_preferredSize = R"doc()doc";

static const char *__doc_nanogui_Graph_save = R"doc()doc";

static const char *__doc_nanogui_Graph_setBackgroundColor = R"doc()doc";

static const char *__doc_nanogui_Graph_setCaption = R"doc()doc";

static const char *__doc_nanogui_Graph_setFooter = R"doc()doc";

static const char *__doc_nanogui_Graph_setForegroundColor = R"doc()doc";

static const char *__doc_nanogui_Graph_setHeader = R"doc()doc";

static const char *__doc_nanogui_Graph_setTextColor = R"doc()doc";

static const char *__doc_nanogui_Graph_setValues = R"doc()doc";

static const char *__doc_nanogui_Graph_textColor = R"doc()doc";

static const char *__doc_nanogui_Graph_values = R"doc()doc";

static const char *__doc_nanogui_Graph_values_2 = R"doc()doc";

static const char *__doc_nanogui_GridLayout =
R"doc(Grid layout.

Widgets are arranged in a grid that has a fixed grid resolution
``resolution`` along one of the axes. The layout orientation indicates
the fixed dimension; widgets are also appended on this axis. The
spacing between items can be specified per axis. The
horizontal/vertical alignment can be specified per row and column.)doc";

static const char *__doc_nanogui_GridLayout_GridLayout =
R"doc(Create a 2-column grid layout by default.

Parameter ``orientation``:
    The fixed dimension of this GridLayout.

Parameter ``resolution``:
    The number of rows or columns in the grid (depending on the
    Orientation).

Parameter ``alignment``:
    How widgets should be aligned within each grid cell.

Parameter ``margin``:
    The amount of spacing to add around the border of the grid.

Parameter ``spacing``:
    The amount of spacing between widgets added to the grid.)doc";

static const char *__doc_nanogui_GridLayout_alignment =
R"doc(The Alignment of the specified axis (row or column number, depending
on the Orientation) at the specified index of that row or column.)doc";

static const char *__doc_nanogui_GridLayout_computeLayout = R"doc(Compute the maximum row and column sizes)doc";

static const char *__doc_nanogui_GridLayout_mAlignment = R"doc(The actual Alignment being used.)doc";

static const char *__doc_nanogui_GridLayout_mDefaultAlignment = R"doc(The default Alignment for this GridLayout.)doc";

static const char *__doc_nanogui_GridLayout_mMargin = R"doc(The margin around this GridLayout.)doc";

static const char *__doc_nanogui_GridLayout_mOrientation = R"doc(The Orientation defining this GridLayout.)doc";

static const char *__doc_nanogui_GridLayout_mResolution =
R"doc(The number of rows or columns before starting a new one, depending on
the Orientation.)doc";

static const char *__doc_nanogui_GridLayout_mSpacing = R"doc(The spacing used for each dimension.)doc";

static const char *__doc_nanogui_GridLayout_margin = R"doc(The margin around this GridLayout.)doc";

static const char *__doc_nanogui_GridLayout_operator_delete = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_new = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_orientation = R"doc(The Orientation of this GridLayout.)doc";

static const char *__doc_nanogui_GridLayout_performLayout = R"doc(See Layout::performLayout.)doc";

static const char *__doc_nanogui_GridLayout_preferredSize = R"doc(See Layout::preferredSize.)doc";

static const char *__doc_nanogui_GridLayout_resolution =
R"doc(The number of rows or columns (depending on the Orientation) of this
GridLayout.)doc";

static const char *__doc_nanogui_GridLayout_setColAlignment = R"doc(Sets the Alignment of the columns.)doc";

static const char *__doc_nanogui_GridLayout_setColAlignment_2 = R"doc(Use this to set variable Alignment for columns.)doc";

static const char *__doc_nanogui_GridLayout_setMargin = R"doc(Sets the margin of this GridLayout.)doc";

static const char *__doc_nanogui_GridLayout_setOrientation = R"doc(Sets the Orientation of this GridLayout.)doc";

static const char *__doc_nanogui_GridLayout_setResolution =
R"doc(Sets the number of rows or columns (depending on the Orientation) of
this GridLayout.)doc";

static const char *__doc_nanogui_GridLayout_setRowAlignment = R"doc(Sets the Alignment of the rows.)doc";

static const char *__doc_nanogui_GridLayout_setRowAlignment_2 = R"doc(Use this to set variable Alignment for rows.)doc";

static const char *__doc_nanogui_GridLayout_setSpacing = R"doc(Sets the spacing for a specific axis.)doc";

static const char *__doc_nanogui_GridLayout_setSpacing_2 = R"doc(Sets the spacing for all axes.)doc";

static const char *__doc_nanogui_GridLayout_spacing =
R"doc(The spacing at the specified axis (row or column number, depending on
the Orientation).)doc";

static const char *__doc_nanogui_GroupLayout =
R"doc(Special layout for widgets grouped by labels.

This widget resembles a box layout in that it arranges a set of
widgets vertically. All widgets are indented on the horizontal axis
except for Label widgets, which are not indented.

This creates a pleasing layout where a number of widgets are grouped
under some high-level heading.)doc";

static const char *__doc_nanogui_GroupLayout_GroupLayout =
R"doc(Creates a GroupLayout.

Parameter ``margin``:
    The margin around the widgets added.

Parameter ``spacing``:
    The spacing between widgets added.

Parameter ``groupSpacing``:
    The spacing between groups (groups are defined by each Label
    added).

Parameter ``groupIndent``:
    The amount to indent widgets in a group (underneath a Label).)doc";

static const char *__doc_nanogui_GroupLayout_groupIndent =
R"doc(The indent of widgets in a group (underneath a Label) of this
GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_groupSpacing = R"doc(The spacing between groups of this GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_mGroupIndent =
R"doc(The indent amount of a group under its defining Label of this
GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_mGroupSpacing = R"doc(The spacing between groups of this GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_mMargin = R"doc(The margin of this GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_mSpacing = R"doc(The spacing between widgets of this GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_margin = R"doc(The margin of this GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_performLayout = R"doc(See Layout::performLayout.)doc";

static const char *__doc_nanogui_GroupLayout_preferredSize = R"doc(See Layout::preferredSize.)doc";

static const char *__doc_nanogui_GroupLayout_setGroupIndent =
R"doc(Sets the indent of widgets in a group (underneath a Label) of this
GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_setGroupSpacing = R"doc(Sets the spacing between groups of this GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_setMargin = R"doc(Sets the margin of this GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_setSpacing = R"doc(Sets the spacing between widgets of this GroupLayout.)doc";

static const char *__doc_nanogui_GroupLayout_spacing = R"doc(The spacing between widgets of this GroupLayout.)doc";

static const char *__doc_nanogui_ImagePanel = R"doc(Image panel widget which shows a number of square-shaped icons.)doc";

static const char *__doc_nanogui_ImagePanel_ImagePanel = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_callback = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_draw = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_gridSize = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_images = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_indexForPosition = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_mCallback = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_mImages = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_mMargin = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_mMouseIndex = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_mSpacing = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_mThumbSize = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_mouseMotionEvent = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_delete = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_new = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_preferredSize = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_setCallback = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_setImages = R"doc()doc";

static const char *__doc_nanogui_ImageView = R"doc(Widget used to display images.)doc";

static const char *__doc_nanogui_ImageView_ImageView = R"doc()doc";

static const char *__doc_nanogui_ImageView_bindImage = R"doc()doc";

static const char *__doc_nanogui_ImageView_center = R"doc(Centers the image without affecting the scaling factor.)doc";

static const char *__doc_nanogui_ImageView_clampedImageCoordinateAt =
R"doc(Calculates the image coordinates of the given pixel position on the
widget. If the position provided corresponds to a coordinate outside
the range of the image, the coordinates are clamped to edges of the
image.)doc";

static const char *__doc_nanogui_ImageView_draw = R"doc()doc";

static const char *__doc_nanogui_ImageView_drawHelpers = R"doc()doc";

static const char *__doc_nanogui_ImageView_drawImageBorder = R"doc()doc";

static const char *__doc_nanogui_ImageView_drawPixelGrid = R"doc()doc";

static const char *__doc_nanogui_ImageView_drawPixelInfo = R"doc()doc";

static const char *__doc_nanogui_ImageView_drawWidgetBorder = R"doc()doc";

static const char *__doc_nanogui_ImageView_fit = R"doc(Centers and scales the image so that it fits inside the widgets.)doc";

static const char *__doc_nanogui_ImageView_fixedOffset = R"doc()doc";

static const char *__doc_nanogui_ImageView_fixedScale = R"doc()doc";

static const char *__doc_nanogui_ImageView_fontScaleFactor = R"doc()doc";

static const char *__doc_nanogui_ImageView_gridThreshold = R"doc()doc";

static const char *__doc_nanogui_ImageView_gridVisible = R"doc(Function indicating whether the grid is currently visible.)doc";

static const char *__doc_nanogui_ImageView_helpersVisible = R"doc(Function indicating whether any of the overlays are visible.)doc";

static const char *__doc_nanogui_ImageView_imageCoordinateAt =
R"doc(Calculates the image coordinates of the given pixel position on the
widget.)doc";

static const char *__doc_nanogui_ImageView_imageShader = R"doc()doc";

static const char *__doc_nanogui_ImageView_imageSize = R"doc()doc";

static const char *__doc_nanogui_ImageView_imageSizeF = R"doc()doc";

static const char *__doc_nanogui_ImageView_keyboardCharacterEvent = R"doc()doc";

static const char *__doc_nanogui_ImageView_keyboardEvent = R"doc()doc";

static const char *__doc_nanogui_ImageView_mFixedOffset = R"doc()doc";

static const char *__doc_nanogui_ImageView_mFixedScale = R"doc()doc";

static const char *__doc_nanogui_ImageView_mFontScaleFactor = R"doc()doc";

static const char *__doc_nanogui_ImageView_mGridThreshold = R"doc()doc";

static const char *__doc_nanogui_ImageView_mImageID = R"doc()doc";

static const char *__doc_nanogui_ImageView_mImageSize = R"doc()doc";

static const char *__doc_nanogui_ImageView_mOffset = R"doc()doc";

static const char *__doc_nanogui_ImageView_mPixelInfoCallback = R"doc()doc";

static const char *__doc_nanogui_ImageView_mPixelInfoThreshold = R"doc()doc";

static const char *__doc_nanogui_ImageView_mScale = R"doc()doc";

static const char *__doc_nanogui_ImageView_mShader = R"doc()doc";

static const char *__doc_nanogui_ImageView_mZoomSensitivity = R"doc()doc";

static const char *__doc_nanogui_ImageView_mouseDragEvent = R"doc()doc";

static const char *__doc_nanogui_ImageView_moveOffset = R"doc(Moves the offset by the specified amount. Does bound checking.)doc";

static const char *__doc_nanogui_ImageView_offset = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_delete = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_new = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_ImageView_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_ImageView_performLayout = R"doc()doc";

static const char *__doc_nanogui_ImageView_pixelInfoCallback = R"doc()doc";

static const char *__doc_nanogui_ImageView_pixelInfoThreshold = R"doc()doc";

static const char *__doc_nanogui_ImageView_pixelInfoVisible =
R"doc(Function indicating whether the pixel information is currently
visible.)doc";

static const char *__doc_nanogui_ImageView_positionF = R"doc()doc";

static const char *__doc_nanogui_ImageView_positionForCoordinate =
R"doc(Calculates the position inside the widget for the given image
coordinate.)doc";

static const char *__doc_nanogui_ImageView_preferredSize = R"doc()doc";

static const char *__doc_nanogui_ImageView_scale = R"doc()doc";

static const char *__doc_nanogui_ImageView_scaledImageSize = R"doc()doc";

static const char *__doc_nanogui_ImageView_scaledImageSizeF = R"doc()doc";

static const char *__doc_nanogui_ImageView_scrollEvent = R"doc()doc";

static const char *__doc_nanogui_ImageView_setFixedOffset = R"doc()doc";

static const char *__doc_nanogui_ImageView_setFixedScale = R"doc()doc";

static const char *__doc_nanogui_ImageView_setFontScaleFactor = R"doc()doc";

static const char *__doc_nanogui_ImageView_setGridThreshold = R"doc()doc";

static const char *__doc_nanogui_ImageView_setImageCoordinateAt =
R"doc(Modifies the internal state of the image viewer widget so that the
pixel at the provided position on the widget has the specified image
coordinate. Also clamps the values of offset to the sides of the
widget.)doc";

static const char *__doc_nanogui_ImageView_setOffset = R"doc()doc";

static const char *__doc_nanogui_ImageView_setPixelInfoCallback = R"doc()doc";

static const char *__doc_nanogui_ImageView_setPixelInfoThreshold = R"doc()doc";

static const char *__doc_nanogui_ImageView_setScale = R"doc()doc";

static const char *__doc_nanogui_ImageView_setScaleCentered = R"doc(Set the scale while keeping the image centered)doc";

static const char *__doc_nanogui_ImageView_setZoomSensitivity = R"doc()doc";

static const char *__doc_nanogui_ImageView_sizeF = R"doc()doc";

static const char *__doc_nanogui_ImageView_updateImageParameters = R"doc()doc";

static const char *__doc_nanogui_ImageView_writePixelInfo = R"doc()doc";

static const char *__doc_nanogui_ImageView_zoom =
R"doc(Changes the scale factor by the provided amount modified by the zoom
sensitivity member variable. The scaling occurs such that the image
coordinate under the focused position remains in the same position
before and after the scaling.)doc";

static const char *__doc_nanogui_ImageView_zoomSensitivity = R"doc()doc";

static const char *__doc_nanogui_IntBox =
R"doc(A specialization of TextBox for representing integral values.

Template parameters should be integral types, e.g. ``int``, ``long``,
``uint32_t``, etc.)doc";

static const char *__doc_nanogui_IntBox_IntBox = R"doc()doc";

static const char *__doc_nanogui_IntBox_mMaxValue = R"doc()doc";

static const char *__doc_nanogui_IntBox_mMinValue = R"doc()doc";

static const char *__doc_nanogui_IntBox_mMouseDownValue = R"doc()doc";

static const char *__doc_nanogui_IntBox_mValueIncrement = R"doc()doc";

static const char *__doc_nanogui_IntBox_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_IntBox_mouseDragEvent = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_delete = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_new = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_IntBox_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_IntBox_scrollEvent = R"doc()doc";

static const char *__doc_nanogui_IntBox_setCallback = R"doc()doc";

static const char *__doc_nanogui_IntBox_setMaxValue = R"doc()doc";

static const char *__doc_nanogui_IntBox_setMinMaxValues = R"doc()doc";

static const char *__doc_nanogui_IntBox_setMinValue = R"doc()doc";

static const char *__doc_nanogui_IntBox_setValue = R"doc()doc";

static const char *__doc_nanogui_IntBox_setValueIncrement = R"doc()doc";

static const char *__doc_nanogui_IntBox_value = R"doc()doc";

static const char *__doc_nanogui_Label =
R"doc(Text label widget.

The font and color can be customized. When Widget::setFixedWidth() is
used, the text is wrapped when it surpasses the specified width.)doc";

static const char *__doc_nanogui_Label_Label = R"doc()doc";

static const char *__doc_nanogui_Label_caption = R"doc(Get the label's text caption)doc";

static const char *__doc_nanogui_Label_color = R"doc(Get the label color)doc";

static const char *__doc_nanogui_Label_draw = R"doc(Draw the label)doc";

static const char *__doc_nanogui_Label_font = R"doc(Get the currently active font)doc";

static const char *__doc_nanogui_Label_load = R"doc()doc";

static const char *__doc_nanogui_Label_mCaption = R"doc()doc";

static const char *__doc_nanogui_Label_mColor = R"doc()doc";

static const char *__doc_nanogui_Label_mFont = R"doc()doc";

static const char *__doc_nanogui_Label_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Label_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Label_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Label_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Label_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Label_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Label_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Label_operator_new = R"doc()doc";

static const char *__doc_nanogui_Label_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Label_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Label_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Label_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_Label_preferredSize = R"doc(Compute the size needed to fully display the label)doc";

static const char *__doc_nanogui_Label_save = R"doc()doc";

static const char *__doc_nanogui_Label_setCaption = R"doc(Set the label's text caption)doc";

static const char *__doc_nanogui_Label_setColor = R"doc(Set the label color)doc";

static const char *__doc_nanogui_Label_setFont =
R"doc(Set the currently active font (2 are available by default: 'sans' and
'sans-bold'))doc";

static const char *__doc_nanogui_Label_setTheme = R"doc(Set the Theme used to draw this widget)doc";

static const char *__doc_nanogui_Layout = R"doc(Basic interface of a layout engine.)doc";

static const char *__doc_nanogui_Layout_performLayout =
R"doc(Performs any and all resizing applicable.

Parameter ``ctx``:
    The ``NanoVG`` context being used for drawing.

Parameter ``widget``:
    The Widget this layout is controlling sizing for.)doc";

static const char *__doc_nanogui_Layout_preferredSize =
R"doc(The preferred size for this layout.

Parameter ``ctx``:
    The ``NanoVG`` context being used for drawing.

Parameter ``widget``:
    The Widget this layout's preferred size is considering.

Returns:
    The preferred size, accounting for things such as spacing, padding
    for icons, etc.)doc";

static const char *__doc_nanogui_MessageDialog = R"doc(Simple "OK" or "Yes/No"-style modal dialogs.)doc";

static const char *__doc_nanogui_MessageDialog_MessageDialog = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_Type = R"doc(Classification of the type of message this MessageDialog represents.)doc";

static const char *__doc_nanogui_MessageDialog_Type_Information = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_Type_Question = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_Type_Warning = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_callback = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_mCallback = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_mMessageLabel = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_messageLabel = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_messageLabel_2 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_delete = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_new = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_setCallback = R"doc()doc";

static const char *__doc_nanogui_Object = R"doc(Reference counted object base class.)doc";

static const char *__doc_nanogui_Object_Object = R"doc(Default constructor)doc";

static const char *__doc_nanogui_Object_Object_2 = R"doc(Copy constructor)doc";

static const char *__doc_nanogui_Object_decRef =
R"doc(Decrease the reference count of the object and possibly deallocate it.

The object will automatically be deallocated once the reference count
reaches zero.)doc";

static const char *__doc_nanogui_Object_getRefCount = R"doc(Return the current reference count)doc";

static const char *__doc_nanogui_Object_incRef = R"doc(Increase the object's reference count by one)doc";

static const char *__doc_nanogui_Object_m_refCount = R"doc()doc";

static const char *__doc_nanogui_Orientation = R"doc(The direction of data flow for a layout.)doc";

static const char *__doc_nanogui_Orientation_Horizontal = R"doc(Layout expands on horizontal axis.)doc";

static const char *__doc_nanogui_Orientation_Vertical = R"doc(Layout expands on vertical axis.)doc";

static const char *__doc_nanogui_Popup =
R"doc(Popup window for combo boxes, popup buttons, nested dialogs etc.

Usually the Popup instance is constructed by another widget (e.g.
PopupButton) and does not need to be created by hand.)doc";

static const char *__doc_nanogui_PopupButton =
R"doc(Button which launches a popup widget.

Remark:
    This class overrides nanogui::Widget::mIconExtraScale to be
    ``0.8f``, which affects all subclasses of this Widget. Subclasses
    must explicitly set a different value if needed (e.g., in their
    constructor).)doc";

static const char *__doc_nanogui_PopupButton_PopupButton = R"doc()doc";

static const char *__doc_nanogui_PopupButton_chevronIcon = R"doc()doc";

static const char *__doc_nanogui_PopupButton_draw = R"doc()doc";

static const char *__doc_nanogui_PopupButton_load = R"doc()doc";

static const char *__doc_nanogui_PopupButton_mChevronIcon = R"doc()doc";

static const char *__doc_nanogui_PopupButton_mPopup = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_delete = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_new = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_performLayout = R"doc()doc";

static const char *__doc_nanogui_PopupButton_popup = R"doc()doc";

static const char *__doc_nanogui_PopupButton_popup_2 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_preferredSize = R"doc()doc";

static const char *__doc_nanogui_PopupButton_save = R"doc()doc";

static const char *__doc_nanogui_PopupButton_setChevronIcon = R"doc()doc";

static const char *__doc_nanogui_PopupButton_setSide = R"doc()doc";

static const char *__doc_nanogui_PopupButton_side = R"doc()doc";

static const char *__doc_nanogui_Popup_Popup =
R"doc(Create a new popup parented to a screen (first argument) and a parent
window)doc";

static const char *__doc_nanogui_Popup_Side = R"doc()doc";

static const char *__doc_nanogui_Popup_Side_Left = R"doc()doc";

static const char *__doc_nanogui_Popup_Side_Right = R"doc()doc";

static const char *__doc_nanogui_Popup_anchorHeight =
R"doc(Return the anchor height; this determines the vertical shift relative
to the anchor position)doc";

static const char *__doc_nanogui_Popup_anchorPos =
R"doc(Set the anchor position in the parent window; the placement of the
popup is relative to it)doc";

static const char *__doc_nanogui_Popup_draw = R"doc(Draw the popup window)doc";

static const char *__doc_nanogui_Popup_load = R"doc()doc";

static const char *__doc_nanogui_Popup_mAnchorHeight = R"doc()doc";

static const char *__doc_nanogui_Popup_mAnchorPos = R"doc()doc";

static const char *__doc_nanogui_Popup_mParentWindow = R"doc()doc";

static const char *__doc_nanogui_Popup_mSide = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_new = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Popup_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_Popup_parentWindow = R"doc(Return the parent window of the popup)doc";

static const char *__doc_nanogui_Popup_parentWindow_2 = R"doc(Return the parent window of the popup)doc";

static const char *__doc_nanogui_Popup_performLayout =
R"doc(Invoke the associated layout generator to properly place child
widgets, if any)doc";

static const char *__doc_nanogui_Popup_refreshRelativePlacement = R"doc(Internal helper function to maintain nested window position values)doc";

static const char *__doc_nanogui_Popup_save = R"doc()doc";

static const char *__doc_nanogui_Popup_setAnchorHeight =
R"doc(Set the anchor height; this determines the vertical shift relative to
the anchor position)doc";

static const char *__doc_nanogui_Popup_setAnchorPos =
R"doc(Return the anchor position in the parent window; the placement of the
popup is relative to it)doc";

static const char *__doc_nanogui_Popup_setSide = R"doc(Set the side of the parent window at which popup will appear)doc";

static const char *__doc_nanogui_Popup_side = R"doc(Return the side of the parent window at which popup will appear)doc";

static const char *__doc_nanogui_ProgressBar = R"doc(Standard widget for visualizing progress.)doc";

static const char *__doc_nanogui_ProgressBar_ProgressBar = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_draw = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_load = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_mValue = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_delete = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_new = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_preferredSize = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_save = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_setValue = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_value = R"doc()doc";

static const char *__doc_nanogui_Screen =
R"doc(Represents a display surface (i.e. a full-screen or windowed GLFW
window) and forms the root element of a hierarchy of nanogui widgets.)doc";

static const char *__doc_nanogui_Screen_Screen =
R"doc(Create a new Screen instance

Parameter ``size``:
    Size in pixels at 96 dpi (on high-DPI screens, the actual
    resolution in terms of hardware pixels may be larger by an integer
    factor)

Parameter ``caption``:
    Window title (in UTF-8 encoding)

Parameter ``resizable``:
    If creating a window, should it be resizable?

Parameter ``fullscreen``:
    Specifies whether to create a windowed or full-screen view

Parameter ``colorBits``:
    Number of bits per pixel dedicated to the R/G/B color components

Parameter ``alphaBits``:
    Number of bits per pixel dedicated to the alpha channel

Parameter ``depthBits``:
    Number of bits per pixel dedicated to the Z-buffer

Parameter ``stencilBits``:
    Number of bits per pixel dedicated to the stencil buffer
    (recommended to set this to 8. NanoVG can draw higher-quality
    strokes using a stencil buffer)

Parameter ``nSamples``:
    Number of MSAA samples (set to 0 to disable)

Parameter ``glMajor``:
    The requested OpenGL Major version number. Default is 3, if
    changed the value must correspond to a forward compatible core
    profile (for portability reasons). For example, set this to 4 and
    glMinor to 1 for a forward compatible core OpenGL 4.1 profile.
    Requesting an invalid profile will result in no context (and
    therefore no GUI) being created.

Parameter ``glMinor``:
    The requested OpenGL Minor version number. Default is 3, if
    changed the value must correspond to a forward compatible core
    profile (for portability reasons). For example, set this to 1 and
    glMajor to 4 for a forward compatible core OpenGL 4.1 profile.
    Requesting an invalid profile will result in no context (and
    therefore no GUI) being created.)doc";

static const char *__doc_nanogui_Screen_Screen_2 =
R"doc(Default constructor

Performs no initialization at all. Use this if the application is
responsible for setting up GLFW, OpenGL, etc.

In this case, override Screen and call initalize() with a pointer to
an existing ``GLFWwindow`` instance

You will also be responsible in this case to deliver GLFW callbacks to
the appropriate callback event handlers below)doc";

static const char *__doc_nanogui_Screen_background = R"doc(Return the screen's background color)doc";

static const char *__doc_nanogui_Screen_caption = R"doc(Get the window title bar caption)doc";

static const char *__doc_nanogui_Screen_centerWindow = R"doc()doc";

static const char *__doc_nanogui_Screen_charCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_cursorPosCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_disposeWindow = R"doc()doc";

static const char *__doc_nanogui_Screen_drawAll = R"doc(Draw the Screen contents)doc";

static const char *__doc_nanogui_Screen_drawContents = R"doc(Draw the window contents --- put your OpenGL draw calls here)doc";

static const char *__doc_nanogui_Screen_drawWidgets = R"doc()doc";

static const char *__doc_nanogui_Screen_dropCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_dropEvent = R"doc(Handle a file drop event)doc";

static const char *__doc_nanogui_Screen_glfwWindow = R"doc(Return a pointer to the underlying GLFW window data structure)doc";

static const char *__doc_nanogui_Screen_initialize = R"doc(Initialize the Screen)doc";

static const char *__doc_nanogui_Screen_keyCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_keyboardCharacterEvent = R"doc(Text input event handler: codepoint is native endian UTF-32 format)doc";

static const char *__doc_nanogui_Screen_keyboardEvent = R"doc(Default keyboard event handler)doc";

static const char *__doc_nanogui_Screen_mBackground = R"doc()doc";

static const char *__doc_nanogui_Screen_mCaption = R"doc()doc";

static const char *__doc_nanogui_Screen_mCursor = R"doc()doc";

static const char *__doc_nanogui_Screen_mCursors = R"doc()doc";

static const char *__doc_nanogui_Screen_mDragActive = R"doc()doc";

static const char *__doc_nanogui_Screen_mDragWidget = R"doc()doc";

static const char *__doc_nanogui_Screen_mFBSize = R"doc()doc";

static const char *__doc_nanogui_Screen_mFocusPath = R"doc()doc";

static const char *__doc_nanogui_Screen_mFullscreen = R"doc()doc";

static const char *__doc_nanogui_Screen_mGLFWWindow = R"doc()doc";

static const char *__doc_nanogui_Screen_mLastInteraction = R"doc()doc";

static const char *__doc_nanogui_Screen_mModifiers = R"doc()doc";

static const char *__doc_nanogui_Screen_mMousePos = R"doc()doc";

static const char *__doc_nanogui_Screen_mMouseState = R"doc()doc";

static const char *__doc_nanogui_Screen_mNVGContext = R"doc()doc";

static const char *__doc_nanogui_Screen_mPixelRatio = R"doc()doc";

static const char *__doc_nanogui_Screen_mProcessEvents = R"doc()doc";

static const char *__doc_nanogui_Screen_mResizeCallback = R"doc()doc";

static const char *__doc_nanogui_Screen_mShutdownGLFWOnDestruct = R"doc()doc";

static const char *__doc_nanogui_Screen_mouseButtonCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_mousePos = R"doc(Return the last observed mouse position value)doc";

static const char *__doc_nanogui_Screen_moveWindowToFront = R"doc()doc";

static const char *__doc_nanogui_Screen_nvgContext = R"doc(Return a pointer to the underlying nanoVG draw context)doc";

static const char *__doc_nanogui_Screen_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_new = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Screen_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_Screen_performLayout = R"doc(Compute the layout of all widgets)doc";

static const char *__doc_nanogui_Screen_pixelRatio =
R"doc(Return the ratio between pixel and device coordinates (e.g. >= 2 on
Mac Retina displays))doc";

static const char *__doc_nanogui_Screen_resizeCallback = R"doc(Set the resize callback)doc";

static const char *__doc_nanogui_Screen_resizeCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_resizeEvent = R"doc(Window resize event handler)doc";

static const char *__doc_nanogui_Screen_scrollCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_setBackground = R"doc(Set the screen's background color)doc";

static const char *__doc_nanogui_Screen_setCaption = R"doc(Set the window title bar caption)doc";

static const char *__doc_nanogui_Screen_setResizeCallback = R"doc()doc";

static const char *__doc_nanogui_Screen_setShutdownGLFWOnDestruct = R"doc()doc";

static const char *__doc_nanogui_Screen_setSize = R"doc(Set window size)doc";

static const char *__doc_nanogui_Screen_setVisible = R"doc(Set the top-level window visibility (no effect on full-screen windows))doc";

static const char *__doc_nanogui_Screen_shutdownGLFWOnDestruct = R"doc()doc";

static const char *__doc_nanogui_Screen_updateFocus = R"doc()doc";

static const char *__doc_nanogui_Slider = R"doc(Fractional slider widget with mouse control.)doc";

static const char *__doc_nanogui_Slider_Slider = R"doc()doc";

static const char *__doc_nanogui_Slider_callback = R"doc()doc";

static const char *__doc_nanogui_Slider_draw = R"doc()doc";

static const char *__doc_nanogui_Slider_finalCallback = R"doc()doc";

static const char *__doc_nanogui_Slider_highlightColor = R"doc()doc";

static const char *__doc_nanogui_Slider_highlightedRange = R"doc()doc";

static const char *__doc_nanogui_Slider_load = R"doc()doc";

static const char *__doc_nanogui_Slider_mCallback = R"doc()doc";

static const char *__doc_nanogui_Slider_mFinalCallback = R"doc()doc";

static const char *__doc_nanogui_Slider_mHighlightColor = R"doc()doc";

static const char *__doc_nanogui_Slider_mHighlightedRange = R"doc()doc";

static const char *__doc_nanogui_Slider_mRange = R"doc()doc";

static const char *__doc_nanogui_Slider_mValue = R"doc()doc";

static const char *__doc_nanogui_Slider_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_Slider_mouseDragEvent = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_new = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Slider_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_Slider_preferredSize = R"doc()doc";

static const char *__doc_nanogui_Slider_range = R"doc()doc";

static const char *__doc_nanogui_Slider_save = R"doc()doc";

static const char *__doc_nanogui_Slider_setCallback = R"doc()doc";

static const char *__doc_nanogui_Slider_setFinalCallback = R"doc()doc";

static const char *__doc_nanogui_Slider_setHighlightColor = R"doc()doc";

static const char *__doc_nanogui_Slider_setHighlightedRange = R"doc()doc";

static const char *__doc_nanogui_Slider_setRange = R"doc()doc";

static const char *__doc_nanogui_Slider_setValue = R"doc()doc";

static const char *__doc_nanogui_Slider_value = R"doc()doc";

static const char *__doc_nanogui_StackedWidget = R"doc(A stack widget.)doc";

static const char *__doc_nanogui_StackedWidget_StackedWidget = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_addChild = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_mSelectedIndex = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_delete = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_new = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_performLayout = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_preferredSize = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_selectedIndex = R"doc()doc";

static const char *__doc_nanogui_StackedWidget_setSelectedIndex = R"doc()doc";

static const char *__doc_nanogui_TabHeader = R"doc(A Tab navigable widget.)doc";

static const char *__doc_nanogui_TabHeader_ClickLocation = R"doc(The location in which the Widget will be facing.)doc";

static const char *__doc_nanogui_TabHeader_ClickLocation_LeftControls = R"doc()doc";

static const char *__doc_nanogui_TabHeader_ClickLocation_RightControls = R"doc()doc";

static const char *__doc_nanogui_TabHeader_ClickLocation_TabButtons = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton = R"doc(Implementation class of the actual tab buttons.)doc";

static const char *__doc_nanogui_TabHeader_TabButton_StringView = R"doc(Helper struct to represent the TabButton.)doc";

static const char *__doc_nanogui_TabHeader_TabButton_StringView_first = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_StringView_last = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_TabButton = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_calculateVisibleString = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_drawActiveBorderAt = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_drawAtPosition = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_drawInactiveBorderAt = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_label = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_mHeader = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_mLabel = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_mSize = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_mVisibleText = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_mVisibleWidth = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_preferredSize = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_setLabel = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_setSize = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabButton_size = R"doc()doc";

static const char *__doc_nanogui_TabHeader_TabHeader = R"doc()doc";

static const char *__doc_nanogui_TabHeader_activeButtonArea =
R"doc(Returns a pair of Vectors describing the top left (pair.first) and the
bottom right (pair.second) positions of the rectangle containing the
active tab button. Returns two zero vectors if the active button is
not visible.)doc";

static const char *__doc_nanogui_TabHeader_activeIterator = R"doc()doc";

static const char *__doc_nanogui_TabHeader_activeIterator_2 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_activeTab = R"doc()doc";

static const char *__doc_nanogui_TabHeader_addTab = R"doc(Inserts a tab at the end of the tabs collection.)doc";

static const char *__doc_nanogui_TabHeader_addTab_2 = R"doc(Inserts a tab into the tabs collection at the specified index.)doc";

static const char *__doc_nanogui_TabHeader_calculateVisibleEnd = R"doc(Given the beginning of the visible tabs, calculate the end.)doc";

static const char *__doc_nanogui_TabHeader_callback = R"doc()doc";

static const char *__doc_nanogui_TabHeader_draw = R"doc()doc";

static const char *__doc_nanogui_TabHeader_drawControls = R"doc()doc";

static const char *__doc_nanogui_TabHeader_ensureTabVisible =
R"doc(Recalculate the visible range of tabs so that the tab with the
specified index is visible. The tab with the specified index will
either be the first or last visible one depending on the position
relative to the old visible range.)doc";

static const char *__doc_nanogui_TabHeader_font = R"doc()doc";

static const char *__doc_nanogui_TabHeader_isTabVisible = R"doc()doc";

static const char *__doc_nanogui_TabHeader_locateClick = R"doc()doc";

static const char *__doc_nanogui_TabHeader_mActiveTab = R"doc()doc";

static const char *__doc_nanogui_TabHeader_mCallback = R"doc()doc";

static const char *__doc_nanogui_TabHeader_mFont = R"doc()doc";

static const char *__doc_nanogui_TabHeader_mOverflowing = R"doc()doc";

static const char *__doc_nanogui_TabHeader_mTabButtons = R"doc()doc";

static const char *__doc_nanogui_TabHeader_mVisibleEnd = R"doc()doc";

static const char *__doc_nanogui_TabHeader_mVisibleStart = R"doc()doc";

static const char *__doc_nanogui_TabHeader_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_TabHeader_onArrowLeft = R"doc()doc";

static const char *__doc_nanogui_TabHeader_onArrowRight = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_delete = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_new = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_overflowing = R"doc()doc";

static const char *__doc_nanogui_TabHeader_performLayout = R"doc()doc";

static const char *__doc_nanogui_TabHeader_preferredSize = R"doc()doc";

static const char *__doc_nanogui_TabHeader_removeTab =
R"doc(Removes the tab with the specified label and returns the index of the
label. Returns -1 if there was no such tab)doc";

static const char *__doc_nanogui_TabHeader_removeTab_2 = R"doc(Removes the tab with the specified index.)doc";

static const char *__doc_nanogui_TabHeader_setActiveTab = R"doc()doc";

static const char *__doc_nanogui_TabHeader_setCallback =
R"doc(Sets the callable objects which is invoked when a tab button is
pressed. The argument provided to the callback is the index of the
tab.)doc";

static const char *__doc_nanogui_TabHeader_setFont = R"doc()doc";

static const char *__doc_nanogui_TabHeader_tabCount = R"doc()doc";

static const char *__doc_nanogui_TabHeader_tabIndex =
R"doc(Retrieves the index of a specific tab label. Returns the number of
tabs (tabsCount) if there is no such tab.)doc";

static const char *__doc_nanogui_TabHeader_tabIterator = R"doc()doc";

static const char *__doc_nanogui_TabHeader_tabIterator_2 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_tabLabelAt = R"doc(Retrieves the label of the tab at a specific index.)doc";

static const char *__doc_nanogui_TabHeader_visibleBegin = R"doc()doc";

static const char *__doc_nanogui_TabHeader_visibleBegin_2 = R"doc()doc";

static const char *__doc_nanogui_TabHeader_visibleButtonArea =
R"doc(Returns a pair of Vectors describing the top left (pair.first) and the
bottom right (pair.second) positions of the rectangle containing the
visible tab buttons.)doc";

static const char *__doc_nanogui_TabHeader_visibleEnd = R"doc()doc";

static const char *__doc_nanogui_TabHeader_visibleEnd_2 = R"doc()doc";

static const char *__doc_nanogui_TabWidget =
R"doc(A wrapper around the widgets TabHeader and StackedWidget which hooks
the two classes together.)doc";

static const char *__doc_nanogui_TabWidget_TabWidget = R"doc()doc";

static const char *__doc_nanogui_TabWidget_activeTab = R"doc()doc";

static const char *__doc_nanogui_TabWidget_addTab =
R"doc(Inserts a tab at the end of the tabs collection and associates it with
the provided widget.)doc";

static const char *__doc_nanogui_TabWidget_addTab_2 =
R"doc(Inserts a tab into the tabs collection at the specified index and
associates it with the provided widget.)doc";

static const char *__doc_nanogui_TabWidget_callback = R"doc()doc";

static const char *__doc_nanogui_TabWidget_createTab =
R"doc(Creates a new tab with the specified name and returns a pointer to the
layer.)doc";

static const char *__doc_nanogui_TabWidget_createTab_2 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_draw = R"doc()doc";

static const char *__doc_nanogui_TabWidget_ensureTabVisible =
R"doc(This function can be invoked to ensure that the tab with the provided
index the is visible, i.e to track the given tab. Forwards to the tab
header widget. This function should be used whenever the client wishes
to make the tab header follow a newly added tab, as the content of the
new tab is made visible but the tab header does not track it by
default.)doc";

static const char *__doc_nanogui_TabWidget_mCallback = R"doc()doc";

static const char *__doc_nanogui_TabWidget_mContent = R"doc()doc";

static const char *__doc_nanogui_TabWidget_mHeader = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_delete = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_new = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_TabWidget_performLayout = R"doc()doc";

static const char *__doc_nanogui_TabWidget_preferredSize = R"doc()doc";

static const char *__doc_nanogui_TabWidget_removeTab =
R"doc(Removes the tab with the specified label and returns the index of the
label. Returns whether the removal was successful.)doc";

static const char *__doc_nanogui_TabWidget_removeTab_2 = R"doc(Removes the tab with the specified index.)doc";

static const char *__doc_nanogui_TabWidget_setActiveTab = R"doc()doc";

static const char *__doc_nanogui_TabWidget_setCallback =
R"doc(Sets the callable objects which is invoked when a tab is changed. The
argument provided to the callback is the index of the new active tab.)doc";

static const char *__doc_nanogui_TabWidget_tab = R"doc()doc";

static const char *__doc_nanogui_TabWidget_tabCount = R"doc()doc";

static const char *__doc_nanogui_TabWidget_tabIndex =
R"doc(Retrieves the index of a specific tab using a widget pointer. Returns
-1 if there is no such tab.)doc";

static const char *__doc_nanogui_TabWidget_tabLabelAt = R"doc(Retrieves the label of the tab at a specific index.)doc";

static const char *__doc_nanogui_TabWidget_tabLabelIndex =
R"doc(Retrieves the index of a specific tab using its tab label. Returns -1
if there is no such tab.)doc";

static const char *__doc_nanogui_TabWidget_tab_2 = R"doc()doc";

static const char *__doc_nanogui_TextBox =
R"doc(Fancy text box with builtin regular expression-based validation.

Remark:
    This class overrides nanogui::Widget::mIconExtraScale to be
    ``0.8f``, which affects all subclasses of this Widget. Subclasses
    must explicitly set a different value if needed (e.g., in their
    constructor).)doc";

static const char *__doc_nanogui_TextBox_Alignment = R"doc(How to align the text in the text box.)doc";

static const char *__doc_nanogui_TextBox_Alignment_Center = R"doc()doc";

static const char *__doc_nanogui_TextBox_Alignment_Left = R"doc()doc";

static const char *__doc_nanogui_TextBox_Alignment_Right = R"doc()doc";

static const char *__doc_nanogui_TextBox_SpinArea = R"doc(The location (if any) for the spin area.)doc";

static const char *__doc_nanogui_TextBox_SpinArea_Bottom = R"doc()doc";

static const char *__doc_nanogui_TextBox_SpinArea_None = R"doc()doc";

static const char *__doc_nanogui_TextBox_SpinArea_Top = R"doc()doc";

static const char *__doc_nanogui_TextBox_TextBox = R"doc()doc";

static const char *__doc_nanogui_TextBox_alignment = R"doc()doc";

static const char *__doc_nanogui_TextBox_callback = R"doc(The callback to execute when the value of this TextBox has changed.)doc";

static const char *__doc_nanogui_TextBox_checkFormat = R"doc()doc";

static const char *__doc_nanogui_TextBox_copySelection = R"doc()doc";

static const char *__doc_nanogui_TextBox_cursorIndex2Position = R"doc()doc";

static const char *__doc_nanogui_TextBox_defaultValue = R"doc()doc";

static const char *__doc_nanogui_TextBox_deleteSelection = R"doc()doc";

static const char *__doc_nanogui_TextBox_draw = R"doc()doc";

static const char *__doc_nanogui_TextBox_editable = R"doc()doc";

static const char *__doc_nanogui_TextBox_focusEvent = R"doc()doc";

static const char *__doc_nanogui_TextBox_format = R"doc(Return the underlying regular expression specifying valid formats)doc";

static const char *__doc_nanogui_TextBox_keyboardCharacterEvent = R"doc()doc";

static const char *__doc_nanogui_TextBox_keyboardEvent = R"doc()doc";

static const char *__doc_nanogui_TextBox_load = R"doc()doc";

static const char *__doc_nanogui_TextBox_mAlignment = R"doc()doc";

static const char *__doc_nanogui_TextBox_mCallback = R"doc()doc";

static const char *__doc_nanogui_TextBox_mCommitted = R"doc()doc";

static const char *__doc_nanogui_TextBox_mCursorPos = R"doc()doc";

static const char *__doc_nanogui_TextBox_mDefaultValue = R"doc()doc";

static const char *__doc_nanogui_TextBox_mEditable = R"doc()doc";

static const char *__doc_nanogui_TextBox_mFormat = R"doc()doc";

static const char *__doc_nanogui_TextBox_mLastClick = R"doc()doc";

static const char *__doc_nanogui_TextBox_mMouseDownModifier = R"doc()doc";

static const char *__doc_nanogui_TextBox_mMouseDownPos = R"doc()doc";

static const char *__doc_nanogui_TextBox_mMouseDragPos = R"doc()doc";

static const char *__doc_nanogui_TextBox_mMousePos = R"doc()doc";

static const char *__doc_nanogui_TextBox_mPlaceholder = R"doc()doc";

static const char *__doc_nanogui_TextBox_mSelectionPos = R"doc()doc";

static const char *__doc_nanogui_TextBox_mSpinnable = R"doc()doc";

static const char *__doc_nanogui_TextBox_mTextOffset = R"doc()doc";

static const char *__doc_nanogui_TextBox_mUnits = R"doc()doc";

static const char *__doc_nanogui_TextBox_mUnitsImage = R"doc()doc";

static const char *__doc_nanogui_TextBox_mValidFormat = R"doc()doc";

static const char *__doc_nanogui_TextBox_mValue = R"doc()doc";

static const char *__doc_nanogui_TextBox_mValueTemp = R"doc()doc";

static const char *__doc_nanogui_TextBox_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_TextBox_mouseDragEvent = R"doc()doc";

static const char *__doc_nanogui_TextBox_mouseMotionEvent = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_delete = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_new = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_TextBox_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_TextBox_pasteFromClipboard = R"doc()doc";

static const char *__doc_nanogui_TextBox_placeholder =
R"doc(Return the placeholder text to be displayed while the text box is
empty.)doc";

static const char *__doc_nanogui_TextBox_position2CursorIndex = R"doc()doc";

static const char *__doc_nanogui_TextBox_preferredSize = R"doc()doc";

static const char *__doc_nanogui_TextBox_save = R"doc()doc";

static const char *__doc_nanogui_TextBox_setAlignment = R"doc()doc";

static const char *__doc_nanogui_TextBox_setCallback =
R"doc(Sets the callback to execute when the value of this TextBox has
changed.)doc";

static const char *__doc_nanogui_TextBox_setDefaultValue = R"doc()doc";

static const char *__doc_nanogui_TextBox_setEditable = R"doc()doc";

static const char *__doc_nanogui_TextBox_setFormat = R"doc(Specify a regular expression specifying valid formats)doc";

static const char *__doc_nanogui_TextBox_setPlaceholder =
R"doc(Specify a placeholder text to be displayed while the text box is
empty.)doc";

static const char *__doc_nanogui_TextBox_setSpinnable = R"doc()doc";

static const char *__doc_nanogui_TextBox_setTheme = R"doc(Set the Theme used to draw this widget)doc";

static const char *__doc_nanogui_TextBox_setUnits = R"doc()doc";

static const char *__doc_nanogui_TextBox_setUnitsImage = R"doc()doc";

static const char *__doc_nanogui_TextBox_setValue = R"doc()doc";

static const char *__doc_nanogui_TextBox_spinArea = R"doc()doc";

static const char *__doc_nanogui_TextBox_spinnable = R"doc()doc";

static const char *__doc_nanogui_TextBox_units = R"doc()doc";

static const char *__doc_nanogui_TextBox_unitsImage = R"doc()doc";

static const char *__doc_nanogui_TextBox_updateCursor = R"doc()doc";

static const char *__doc_nanogui_TextBox_value = R"doc()doc";

static const char *__doc_nanogui_Theme = R"doc(Storage class for basic theme-related properties.)doc";

static const char *__doc_nanogui_Theme_Theme = R"doc()doc";

static const char *__doc_nanogui_Theme_mBorderDark =
R"doc(The dark border color (default: intensity=``29``, alpha=``255``; see
nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mBorderLight =
R"doc(The light border color (default: intensity=``92``, alpha=``255``; see
nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mBorderMedium =
R"doc(The medium border color (default: intensity=``35``, alpha=``255``; see
nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mButtonCornerRadius =
R"doc(Rounding radius for Button (and derived types) widgets (default:
``2``).)doc";

static const char *__doc_nanogui_Theme_mButtonFontSize = R"doc(The font size for buttons (default: ``20``).)doc";

static const char *__doc_nanogui_Theme_mButtonGradientBotFocused =
R"doc(The bottom gradient color for buttons in focus (default:
intensity=``48``, alpha=``255``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mButtonGradientBotPushed =
R"doc(The bottom gradient color for buttons currently pushed (default:
intensity=``29``, alpha=``255``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mButtonGradientBotUnfocused =
R"doc(The bottom gradient color for buttons not in focus (default:
intensity=``58``, alpha=``255``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mButtonGradientTopFocused =
R"doc(The top gradient color for buttons in focus (default:
intensity=``64``, alpha=``255``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mButtonGradientTopPushed =
R"doc(The top gradient color for buttons currently pushed (default:
intensity=``41``, alpha=``255``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mButtonGradientTopUnfocused =
R"doc(The top gradient color for buttons not in focus (default:
intensity=``74``, alpha=``255``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mCheckBoxIcon = R"doc(Icon to use for CheckBox widgets (default: ``ENTYPO_ICON_CHECK``).)doc";

static const char *__doc_nanogui_Theme_mDisabledTextColor =
R"doc(The disable dtext color (default: intensity=``255``, alpha=``80``; see
nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mDropShadow =
R"doc(The color of the drop shadow drawn behind widgets (default:
intensity=``0``, alpha=``128``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mFontBold =
R"doc(The bold font face (default: ``"sans-bold"`` from
``resources/roboto_regular.ttf``).)doc";

static const char *__doc_nanogui_Theme_mFontIcons =
R"doc(The icon font face (default: ``"icons"`` from
``resources/entypo.ttf``).)doc";

static const char *__doc_nanogui_Theme_mFontNormal =
R"doc(The standard font face (default: ``"sans"`` from
``resources/roboto_regular.ttf``).)doc";

static const char *__doc_nanogui_Theme_mIconColor = R"doc(The icon color (default: nanogui::Theme::mTextColor).)doc";

static const char *__doc_nanogui_Theme_mIconScale =
R"doc(The amount of scaling that is applied to each icon to fit the size of
NanoGUI widgets. The default value is ``0.77f``, setting to e.g.
higher than ``1.0f`` is generally discouraged.)doc";

static const char *__doc_nanogui_Theme_mMessageAltButtonIcon =
R"doc(Icon to use on MessageDialog alt button (default:
``ENTYPO_ICON_CIRCLE_WITH_CROSS``).)doc";

static const char *__doc_nanogui_Theme_mMessageInformationIcon =
R"doc(Icon to use for informational MessageDialog widgets (default:
``ENTYPO_ICON_INFO_WITH_CIRCLE``).)doc";

static const char *__doc_nanogui_Theme_mMessagePrimaryButtonIcon =
R"doc(Icon to use on MessageDialog primary button (default:
``ENTYPO_ICON_CHECK``).)doc";

static const char *__doc_nanogui_Theme_mMessageQuestionIcon =
R"doc(Icon to use for interrogative MessageDialog widgets (default:
``ENTYPO_ICON_HELP_WITH_CIRCLE``).)doc";

static const char *__doc_nanogui_Theme_mMessageWarningIcon =
R"doc(Icon to use for warning MessageDialog widgets (default:
``ENTYPO_ICON_WARNING``).)doc";

static const char *__doc_nanogui_Theme_mPopupChevronLeftIcon =
R"doc(Icon to use for PopupButton widgets opening to the left (default:
``ENTYPO_ICON_CHEVRON_LEFT``).)doc";

static const char *__doc_nanogui_Theme_mPopupChevronRightIcon =
R"doc(Icon to use for PopupButton widgets opening to the right (default:
``ENTYPO_ICON_CHEVRON_RIGHT``).)doc";

static const char *__doc_nanogui_Theme_mStandardFontSize =
R"doc(The font size for all widgets other than buttons and textboxes
(default: `` 16``).)doc";

static const char *__doc_nanogui_Theme_mTabBorderWidth = R"doc(The border width for TabHeader widgets (default: ``0.75f``).)doc";

static const char *__doc_nanogui_Theme_mTabButtonHorizontalPadding =
R"doc(The amount of horizontal padding for a TabHeader widget (default:
``10``).)doc";

static const char *__doc_nanogui_Theme_mTabButtonVerticalPadding =
R"doc(The amount of vertical padding for a TabHeader widget (default:
``2``).)doc";

static const char *__doc_nanogui_Theme_mTabControlWidth =
R"doc(Used to help specify what lies "in bound" for a TabHeader widget
(default: ``20``).)doc";

static const char *__doc_nanogui_Theme_mTabHeaderLeftIcon =
R"doc(Icon to indicate hidden tabs to the left on a TabHeader (default:
``ENTYPO_ICON_ARROW_BOLD_LEFT``).)doc";

static const char *__doc_nanogui_Theme_mTabHeaderRightIcon =
R"doc(Icon to indicate hidden tabs to the right on a TabHeader (default:
``ENTYPO_ICON_ARROW_BOLD_RIGHT``).)doc";

static const char *__doc_nanogui_Theme_mTabInnerMargin = R"doc(The inner margin on a TabHeader widget (default: ``5``).)doc";

static const char *__doc_nanogui_Theme_mTabMaxButtonWidth = R"doc(The maximum size for buttons on a TabHeader widget (default: ``160``).)doc";

static const char *__doc_nanogui_Theme_mTabMinButtonWidth = R"doc(The minimum size for buttons on a TabHeader widget (default: ``20``).)doc";

static const char *__doc_nanogui_Theme_mTextBoxDownIcon =
R"doc(Icon to use when a TextBox has a down toggle (e.g. IntBox) (default:
``ENTYPO_ICON_CHEVRON_DOWN``).)doc";

static const char *__doc_nanogui_Theme_mTextBoxFontSize = R"doc(The font size for text boxes (default: ``20``).)doc";

static const char *__doc_nanogui_Theme_mTextBoxUpIcon =
R"doc(Icon to use when a TextBox has an up toggle (e.g. IntBox) (default:
``ENTYPO_ICON_CHEVRON_UP``).)doc";

static const char *__doc_nanogui_Theme_mTextColor =
R"doc(The text color (default: intensity=``255``, alpha=``160``; see
nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mTextColorShadow =
R"doc(The text shadow color (default: intensity=``0``, alpha=``160``; see
nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mTransparent =
R"doc(The transparency color (default: intensity=``0``, alpha=``0``; see
nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mWindowCornerRadius = R"doc(Rounding radius for Window widget corners (default: ``2``).)doc";

static const char *__doc_nanogui_Theme_mWindowDropShadowSize =
R"doc(Size of drop shadow rendered behind the Window widgets (default:
``10``).)doc";

static const char *__doc_nanogui_Theme_mWindowFillFocused =
R"doc(The fill color for a Window that is in focus (default:
intensity=``45``, alpha=``230``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mWindowFillUnfocused =
R"doc(The fill color for a Window that is not in focus (default:
intensity=``43``, alpha=``230``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mWindowHeaderGradientBot =
R"doc(The bottom gradient color for Window headings (default:
nanogui::Theme::mButtonGradientBotUnfocused).)doc";

static const char *__doc_nanogui_Theme_mWindowHeaderGradientTop =
R"doc(The top gradient color for Window headings (default:
nanogui::Theme::mButtonGradientTopUnfocused).)doc";

static const char *__doc_nanogui_Theme_mWindowHeaderHeight = R"doc(Default size of Window widget titles (default: ``30``).)doc";

static const char *__doc_nanogui_Theme_mWindowHeaderSepBot =
R"doc(The Window header bottom separation color (default:
nanogui::Theme::mBorderDark).)doc";

static const char *__doc_nanogui_Theme_mWindowHeaderSepTop =
R"doc(The Window header top separation color (default:
nanogui::Theme::mBorderLight).)doc";

static const char *__doc_nanogui_Theme_mWindowPopup =
R"doc(The popup window color (default: intensity=``50``, alpha=``255``; see
nanogui::Color::Color(int,int))).)doc";

static const char *__doc_nanogui_Theme_mWindowPopupTransparent =
R"doc(The transparent popup window color (default: intensity=``50``,
alpha=``0``; see nanogui::Color::Color(int,int))).)doc";

static const char *__doc_nanogui_Theme_mWindowTitleFocused =
R"doc(The title color for a Window that is in focus (default:
intensity=``255``, alpha=``190``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_mWindowTitleUnfocused =
R"doc(The title color for a Window that is not in focus (default:
intensity=``220``, alpha=``160``; see nanogui::Color::Color(int,int)).)doc";

static const char *__doc_nanogui_Theme_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_new = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Theme_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_ToolButton = R"doc(Simple radio+toggle button with an icon.)doc";

static const char *__doc_nanogui_ToolButton_ToolButton = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_delete = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_new = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_ToolButton_operator_new_5 = R"doc()doc";

/*static const char *__doc_nanogui_UniformBufferStd140 =
R"doc(Helper class for accumulating uniform buffer data following the
'std140' packing format.)doc";

static const char *__doc_nanogui_UniformBufferStd140_push_back = R"doc()doc";

static const char *__doc_nanogui_UniformBufferStd140_push_back_2 = R"doc()doc";

static const char *__doc_nanogui_UniformBufferStd140_push_back_3 = R"doc()doc";*/

static const char *__doc_nanogui_VScrollPanel =
R"doc(Adds a vertical scrollbar around a widget that is too big to fit into
a certain area.)doc";

static const char *__doc_nanogui_VScrollPanel_VScrollPanel = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_draw = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_load = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_mChildPreferredHeight = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_mScroll = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_mUpdateLayout = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_mouseDragEvent = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_delete = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_new = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_performLayout = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_preferredSize = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_save = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_scroll =
R"doc(Return the current scroll amount as a value between 0 and 1. 0 means
scrolled to the top and 1 to the bottom.)doc";

static const char *__doc_nanogui_VScrollPanel_scrollEvent = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_setScroll =
R"doc(Set the scroll amount to a value between 0 and 1. 0 means scrolled to
the top and 1 to the bottom.)doc";

static const char *__doc_nanogui_Widget =
R"doc(Base class of all widgets.

Widget is the base class of all widgets in ``nanogui``. It can also be
used as an panel to arrange an arbitrary number of child widgets using
a layout generator (see Layout).)doc";

static const char *__doc_nanogui_Widget_Widget = R"doc(Construct a new widget with the given parent widget)doc";

static const char *__doc_nanogui_Widget_absolutePosition = R"doc(Return the absolute position on screen)doc";

static const char *__doc_nanogui_Widget_add = R"doc(Variadic shorthand notation to construct and add a child widget)doc";

static const char *__doc_nanogui_Widget_addChild =
R"doc(Add a child widget to the current widget at the specified index.

This function almost never needs to be called by hand, since the
constructor of Widget automatically adds the current widget to its
parent)doc";

static const char *__doc_nanogui_Widget_addChild_2 = R"doc(Convenience function which appends a widget at the end)doc";

static const char *__doc_nanogui_Widget_childAt = R"doc(Retrieves the child at the specific position)doc";

static const char *__doc_nanogui_Widget_childAt_2 = R"doc(Retrieves the child at the specific position)doc";

static const char *__doc_nanogui_Widget_childCount = R"doc(Return the number of child widgets)doc";

static const char *__doc_nanogui_Widget_childIndex = R"doc(Returns the index of a specific child or -1 if not found)doc";

static const char *__doc_nanogui_Widget_children = R"doc(Return the list of child widgets of the current widget)doc";

static const char *__doc_nanogui_Widget_contains = R"doc(Check if the widget contains a certain position)doc";

static const char *__doc_nanogui_Widget_cursor = R"doc(Return a pointer to the cursor of the widget)doc";

static const char *__doc_nanogui_Widget_draw = R"doc(Draw the widget (and all child widgets))doc";

static const char *__doc_nanogui_Widget_enabled = R"doc(Return whether or not this widget is currently enabled)doc";

static const char *__doc_nanogui_Widget_findWidget = R"doc(Determine the widget located at the given position value (recursive))doc";

static const char *__doc_nanogui_Widget_fixedHeight = R"doc()doc";

static const char *__doc_nanogui_Widget_fixedSize = R"doc(Return the fixed size (see setFixedSize()))doc";

static const char *__doc_nanogui_Widget_fixedWidth = R"doc()doc";

static const char *__doc_nanogui_Widget_focusEvent =
R"doc(Handle a focus change event (default implementation: record the focus
status, but do nothing))doc";

static const char *__doc_nanogui_Widget_focused = R"doc(Return whether or not this widget is currently focused)doc";

static const char *__doc_nanogui_Widget_fontSize =
R"doc(Return current font size. If not set the default of the current theme
will be returned)doc";

static const char *__doc_nanogui_Widget_hasFontSize = R"doc(Return whether the font size is explicitly specified for this widget)doc";

static const char *__doc_nanogui_Widget_height = R"doc(Return the height of the widget)doc";

static const char *__doc_nanogui_Widget_iconExtraScale =
R"doc(The amount of extra scaling applied to *icon* fonts. See
nanogui::Widget::mIconExtraScale.)doc";

static const char *__doc_nanogui_Widget_icon_scale =
R"doc(Convenience definition for subclasses to get the full icon scale for
this class of Widget. It simple returns the value ``mTheme->mIconScale
* this->mIconExtraScale``.

Remark:
    See also: nanogui::Theme::mIconScale and
    nanogui::Widget::mIconExtraScale. This tiered scaling strategy may
    not be appropriate with fonts other than ``entypo.ttf``.)doc";

static const char *__doc_nanogui_Widget_id = R"doc(Return the ID value associated with this widget, if any)doc";

static const char *__doc_nanogui_Widget_keyboardCharacterEvent = R"doc(Handle text input (UTF-32 format) (default implementation: do nothing))doc";

static const char *__doc_nanogui_Widget_keyboardEvent = R"doc(Handle a keyboard event (default implementation: do nothing))doc";

static const char *__doc_nanogui_Widget_layout = R"doc(Return the used Layout generator)doc";

static const char *__doc_nanogui_Widget_layout_2 = R"doc(Return the used Layout generator)doc";

static const char *__doc_nanogui_Widget_load = R"doc(Restore the state of the widget from the given Serializer instance)doc";

static const char *__doc_nanogui_Widget_mChildren = R"doc()doc";

static const char *__doc_nanogui_Widget_mCursor = R"doc()doc";

static const char *__doc_nanogui_Widget_mEnabled =
R"doc(Whether or not this Widget is currently enabled. Various different
kinds of derived types use this to determine whether or not user input
will be accepted. For example, when ``mEnabled == false``, the state
of a CheckBox cannot be changed, or a TextBox will not allow new
input.)doc";

static const char *__doc_nanogui_Widget_mFixedSize = R"doc()doc";

static const char *__doc_nanogui_Widget_mFocused = R"doc()doc";

static const char *__doc_nanogui_Widget_mFontSize = R"doc()doc";

static const char *__doc_nanogui_Widget_mIconExtraScale =
R"doc(The amount of extra icon scaling used in addition the the theme's
default icon font scale. Default value is ``1.0``, which implies that
nanogui::Widget::icon_scale simply returns the value of
nanogui::Theme::mIconScale.

Most widgets do not need extra scaling, but some (e.g., CheckBox,
TextBox) need to adjust the Theme's default icon scaling
(nanogui::Theme::mIconScale) to properly display icons within their
bounds (upscale, or downscale).

```
Note:
When using ``nvgFontSize`` for icons in subclasses, make sure to call
the :func:`nanogui::Widget::icon_scale` function.  Expected usage when
*drawing* icon fonts is something like:
   virtual void draw(NVGcontext *ctx) {
       // fontSize depends on the kind of Widget.  Search for `FontSize`
       // in the Theme class (e.g., standard vs button)
       float ih = fontSize;
       // assuming your Widget has a declared `mIcon`
       if (nvgIsFontIcon(mIcon)) {
           ih *= icon_scale();
           nvgFontFace(ctx, "icons");
           nvgFontSize(ctx, ih);
           /// remaining drawing code (see button.cpp for more)
       }
   }

```)doc";

static const char *__doc_nanogui_Widget_mId = R"doc()doc";

static const char *__doc_nanogui_Widget_mLayout = R"doc()doc";

static const char *__doc_nanogui_Widget_mMouseFocus = R"doc()doc";

static const char *__doc_nanogui_Widget_mParent = R"doc()doc";

static const char *__doc_nanogui_Widget_mPos = R"doc()doc";

static const char *__doc_nanogui_Widget_mSize = R"doc()doc";

static const char *__doc_nanogui_Widget_mTheme = R"doc()doc";

static const char *__doc_nanogui_Widget_mTooltip = R"doc()doc";

static const char *__doc_nanogui_Widget_mVisible =
R"doc(Whether or not this Widget is currently visible. When a Widget is not
currently visible, no time is wasted executing its drawing method.)doc";

static const char *__doc_nanogui_Widget_mouseButtonEvent =
R"doc(Handle a mouse button event (default implementation: propagate to
children))doc";

static const char *__doc_nanogui_Widget_mouseDragEvent = R"doc(Handle a mouse drag event (default implementation: do nothing))doc";

static const char *__doc_nanogui_Widget_mouseEnterEvent =
R"doc(Handle a mouse enter/leave event (default implementation: record this
fact, but do nothing))doc";

static const char *__doc_nanogui_Widget_mouseMotionEvent =
R"doc(Handle a mouse motion event (default implementation: propagate to
children))doc";

static const char *__doc_nanogui_Widget_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_new = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Widget_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_Widget_parent = R"doc(Return the parent widget)doc";

static const char *__doc_nanogui_Widget_parent_2 = R"doc(Return the parent widget)doc";

static const char *__doc_nanogui_Widget_performLayout =
R"doc(Invoke the associated layout generator to properly place child
widgets, if any)doc";

static const char *__doc_nanogui_Widget_position = R"doc(Return the position relative to the parent widget)doc";

static const char *__doc_nanogui_Widget_preferredSize = R"doc(Compute the preferred size of the widget)doc";

static const char *__doc_nanogui_Widget_removeChild = R"doc(Remove a child widget by index)doc";

static const char *__doc_nanogui_Widget_removeChild_2 = R"doc(Remove a child widget by value)doc";

static const char *__doc_nanogui_Widget_requestFocus = R"doc(Request the focus to be moved to this widget)doc";

static const char *__doc_nanogui_Widget_save = R"doc(Save the state of the widget into the given Serializer instance)doc";

static const char *__doc_nanogui_Widget_screen = R"doc(Walk up the hierarchy and return the parent screen)doc";

static const char *__doc_nanogui_Widget_scrollEvent =
R"doc(Handle a mouse scroll event (default implementation: propagate to
children))doc";

static const char *__doc_nanogui_Widget_setCursor = R"doc(Set the cursor of the widget)doc";

static const char *__doc_nanogui_Widget_setEnabled = R"doc(Set whether or not this widget is currently enabled)doc";

static const char *__doc_nanogui_Widget_setFixedHeight = R"doc(Set the fixed height (see setFixedSize()))doc";

static const char *__doc_nanogui_Widget_setFixedSize =
R"doc(Set the fixed size of this widget

If nonzero, components of the fixed size attribute override any values
computed by a layout generator associated with this widget. Note that
just setting the fixed size alone is not enough to actually change its
size; this is done with a call to setSize or a call to performLayout()
in the parent widget.)doc";

static const char *__doc_nanogui_Widget_setFixedWidth = R"doc(Set the fixed width (see setFixedSize()))doc";

static const char *__doc_nanogui_Widget_setFocused = R"doc(Set whether or not this widget is currently focused)doc";

static const char *__doc_nanogui_Widget_setFontSize = R"doc(Set the font size of this widget)doc";

static const char *__doc_nanogui_Widget_setHeight = R"doc(Set the height of the widget)doc";

static const char *__doc_nanogui_Widget_setIconExtraScale =
R"doc(Sets the amount of extra scaling applied to *icon* fonts. See
nanogui::Widget::mIconExtraScale.)doc";

static const char *__doc_nanogui_Widget_setId = R"doc(Associate this widget with an ID value (optional))doc";

static const char *__doc_nanogui_Widget_setLayout = R"doc(Set the used Layout generator)doc";

static const char *__doc_nanogui_Widget_setParent = R"doc(Set the parent widget)doc";

static const char *__doc_nanogui_Widget_setPosition = R"doc(Set the position relative to the parent widget)doc";

static const char *__doc_nanogui_Widget_setSize = R"doc(set the size of the widget)doc";

static const char *__doc_nanogui_Widget_setTheme = R"doc(Set the Theme used to draw this widget)doc";

static const char *__doc_nanogui_Widget_setTooltip = R"doc()doc";

static const char *__doc_nanogui_Widget_setVisible =
R"doc(Set whether or not the widget is currently visible (assuming all
parents are visible))doc";

static const char *__doc_nanogui_Widget_setWidth = R"doc(Set the width of the widget)doc";

static const char *__doc_nanogui_Widget_size = R"doc(Return the size of the widget)doc";

static const char *__doc_nanogui_Widget_theme = R"doc(Return the Theme used to draw this widget)doc";

static const char *__doc_nanogui_Widget_theme_2 = R"doc(Return the Theme used to draw this widget)doc";

static const char *__doc_nanogui_Widget_tooltip = R"doc()doc";

static const char *__doc_nanogui_Widget_visible =
R"doc(Return whether or not the widget is currently visible (assuming all
parents are visible))doc";

static const char *__doc_nanogui_Widget_visibleRecursive =
R"doc(Check if this widget is currently visible, taking parent widgets into
account)doc";

static const char *__doc_nanogui_Widget_width = R"doc(Return the width of the widget)doc";

static const char *__doc_nanogui_Widget_window = R"doc(Walk up the hierarchy and return the parent window)doc";

static const char *__doc_nanogui_Window = R"doc(Top-level window widget.)doc";

static const char *__doc_nanogui_Window_Window = R"doc()doc";

static const char *__doc_nanogui_Window_buttonPanel = R"doc(Return the panel used to house window buttons)doc";

static const char *__doc_nanogui_Window_center = R"doc(Center the window in the current Screen)doc";

static const char *__doc_nanogui_Window_dispose = R"doc(Dispose the window)doc";

static const char *__doc_nanogui_Window_draw = R"doc(Draw the window)doc";

static const char *__doc_nanogui_Window_load = R"doc()doc";

static const char *__doc_nanogui_Window_mButtonPanel = R"doc()doc";

static const char *__doc_nanogui_Window_mDrag = R"doc()doc";

static const char *__doc_nanogui_Window_mModal = R"doc()doc";

static const char *__doc_nanogui_Window_mTitle = R"doc()doc";

static const char *__doc_nanogui_Window_modal = R"doc(Is this a model dialog?)doc";

static const char *__doc_nanogui_Window_mouseButtonEvent =
R"doc(Handle mouse events recursively and bring the current window to the
top)doc";

static const char *__doc_nanogui_Window_mouseDragEvent = R"doc(Handle window drag events)doc";

static const char *__doc_nanogui_Window_operator_delete = R"doc()doc";

static const char *__doc_nanogui_Window_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_Window_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_Window_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_Window_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_Window_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_Window_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_Window_operator_new = R"doc()doc";

static const char *__doc_nanogui_Window_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_Window_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_Window_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_Window_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_Window_performLayout =
R"doc(Invoke the associated layout generator to properly place child
widgets, if any)doc";

static const char *__doc_nanogui_Window_preferredSize = R"doc(Compute the preferred size of the widget)doc";

static const char *__doc_nanogui_Window_refreshRelativePlacement =
R"doc(Internal helper function to maintain nested window position values;
overridden in Popup)doc";

static const char *__doc_nanogui_Window_save = R"doc()doc";

static const char *__doc_nanogui_Window_scrollEvent =
R"doc(Accept scroll events and propagate them to the widget under the mouse
cursor)doc";

static const char *__doc_nanogui_Window_setModal = R"doc(Set whether or not this is a modal dialog)doc";

static const char *__doc_nanogui_Window_setTitle = R"doc(Set the window title)doc";

static const char *__doc_nanogui_Window_title = R"doc(Return the window title)doc";

static const char *__doc_nanogui_active = R"doc(Return whether or not a main loop is currently active)doc";

static const char *__doc_nanogui_chdir_to_bundle_parent =
R"doc(Move to the application bundle's parent directory

This is function is convenient when deploying .app bundles on OSX. It
adjusts the file path to the parent directory containing the bundle.)doc";

static const char *__doc_nanogui_detail_FormWidget = R"doc(A specialization for adding a CheckBox to a FormHelper.)doc";

static const char *__doc_nanogui_detail_FormWidget_2 = R"doc(A specialization for adding a TextBox to a FormHelper.)doc";

static const char *__doc_nanogui_detail_FormWidget_3 = R"doc(A specialization for adding a ColorPicker to a FormHelper.)doc";

static const char *__doc_nanogui_detail_FormWidget_FormWidget = R"doc(Creates a new FormWidget with underlying type CheckBox.)doc";

static const char *__doc_nanogui_detail_FormWidget_FormWidget_2 = R"doc(Creates a new FormWidget with underlying type TextBox.)doc";

static const char *__doc_nanogui_detail_FormWidget_FormWidget_3 = R"doc(Creates a new FormWidget with underlying type ColorPicker.)doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_10 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_11 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_12 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_13 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_14 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_15 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_16 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_17 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_18 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_19 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_2 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_20 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_21 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_3 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_4 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_5 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_6 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_7 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_8 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_delete_9 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_10 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_11 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_12 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_13 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_14 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_15 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_2 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_3 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_4 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_5 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_6 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_7 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_8 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_operator_new_9 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_setCallback = R"doc(Pass-through function for nanogui::TextBox::setCallback.)doc";

static const char *__doc_nanogui_detail_FormWidget_setEditable = R"doc(Pass-through function for nanogui::Widget::setEnabled.)doc";

static const char *__doc_nanogui_detail_FormWidget_setEditable_2 = R"doc(Pass-through function for nanogui::Widget::setEnabled.)doc";

static const char *__doc_nanogui_detail_FormWidget_setValue = R"doc(Pass-through function for nanogui::CheckBox::setChecked.)doc";

static const char *__doc_nanogui_detail_FormWidget_setValue_2 = R"doc(Pass-through function for nanogui::ColorPicker::setColor.)doc";

static const char *__doc_nanogui_detail_FormWidget_value = R"doc(Returns the value of nanogui::CheckBox::checked.)doc";

static const char *__doc_nanogui_detail_FormWidget_value_2 = R"doc(Returns the value of nanogui::ColorPicker::color.)doc";

static const char *__doc_nanogui_detail_type_traits = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_2 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_3 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_4 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_5 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_6 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_7 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_8 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_9 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_integral = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_integral_2 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_integral_3 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_integral_4 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_integral_5 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_integral_6 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_integral_7 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_integral_8 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_integral_9 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_type = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_type_2 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_type_3 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_type_4 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_type_5 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_type_6 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_type_7 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_type_8 = R"doc()doc";

static const char *__doc_nanogui_detail_type_traits_type_9 = R"doc()doc";

static const char *__doc_nanogui_file_dialog =
R"doc(Open a native file open/save dialog.

Parameter ``filetypes``:
    Pairs of permissible formats with descriptions like ``("png",
    "Portable Network Graphics")``.

Parameter ``save``:
    Set to ``True`` if you would like subsequent file dialogs to open
    at whatever folder they were in when they close this one.)doc";

static const char *__doc_nanogui_file_dialog_2 =
R"doc(Open a native file open dialog, which allows multiple selection.

Parameter ``filetypes``:
    Pairs of permissible formats with descriptions like ``("png",
    "Portable Network Graphics")``.

Parameter ``save``:
    Set to ``True`` if you would like subsequent file dialogs to open
    at whatever folder they were in when they close this one.

Parameter ``multiple``:
    Set to ``True`` if you would like to be able to select multiple
    files at once. May not be simultaneously true with \p save.)doc";

static const char *__doc_nanogui_frustum =
R"doc(Creates a perspective projection matrix.

Parameter ``left``:
    The left border of the viewport.

Parameter ``right``:
    The right border of the viewport.

Parameter ``bottom``:
    The bottom border of the viewport.

Parameter ``top``:
    The top border of the viewport.

Parameter ``nearVal``:
    The near plane.

Parameter ``farVal``:
    The far plane.)doc";

static const char *__doc_nanogui_init =
R"doc(Static initialization; should be called once before invoking **any**
NanoGUI functions **if** you are having NanoGUI manage OpenGL / GLFW.
This method is effectively a wrapper call to ``glfwInit()``, so if you
are managing OpenGL / GLFW on your own *do not call this method*.

```
Refer to :ref:`nanogui_example_3` for how you might go about managing OpenGL
and GLFW on your own, while still using NanoGUI's classes.

```)doc";

static const char *__doc_nanogui_leave =
R"doc(Request the application main loop to terminate (e.g. if you detached
mainloop).)doc";

static const char *__doc_nanogui_loadImageDirectory =
R"doc(Load a directory of PNG images and upload them to the GPU (suitable
for use with ImagePanel))doc";

static const char *__doc_nanogui_lookAt =
R"doc(Creates a "look at" matrix that describes the position and orientation
of e.g. a camera

Parameter ``origin``:
    The position of the camera.

Parameter ``target``:
    The gaze target of the camera.

Parameter ``up``:
    The up vector of the camera.

```
Warning:
These are used to form an orthonormal basis.  The first basis vector is
defined as ``f = (target - origin).normalized()``.

```)doc";

static const char *__doc_nanogui_mainloop =
R"doc(Enter the application main loop

Parameter ``refresh``:
    NanoGUI issues a redraw call whenever an keyboard/mouse/.. event
    is received. In the absence of any external events, it enforces a
    redraw once every ``refresh`` milliseconds. To disable the refresh
    timer, specify a negative value here.

Parameter ``detach``:
    This parameter only exists in the Python bindings. When the active
    ``Screen`` instance is provided via the ``detach`` parameter, the
    ``mainloop()`` function becomes non-blocking and returns
    immediately (in this case, the main loop runs in parallel on a
    newly created thread). This feature is convenient for prototyping
    user interfaces on an interactive Python command prompt. When
    ``detach != None``, the function returns an opaque handle that
    will release any resources allocated by the created thread when
    the handle's ``join()`` method is invoked (or when it is garbage
    collected).

Remark:
    Unfortunately, Mac OS X strictly requires all event processing to
    take place on the application's main thread, which is
    fundamentally incompatible with this type of approach. Thus,
    NanoGUI relies on a rather crazy workaround on Mac OS (kudos to
    Dmitriy Morozov): ``mainloop()`` launches a new thread as before
    but then uses libcoro to swap the thread execution environment
    (stack, registers, ..) with the main thread. This means that the
    main application thread is hijacked and processes events in the
    main loop to satisfy the requirements on Mac OS, while the thread
    that actually returns from this function is the newly created one
    (paradoxical, as that may seem). Deleting or ``join()``ing the
    returned handle causes application to wait for the termination of
    the main loop and then swap the two thread environments back into
    their initial configuration.)doc";

static const char *__doc_nanogui_nanogui_get_image = R"doc(Helper function used by nvgImageIcon)doc";

static const char *__doc_nanogui_nvgIsFontIcon =
R"doc(Determine whether an icon ID is a font-based icon (e.g. from
``entypo.ttf``).

```
See :func:`nanogui::nvgIsImageIcon` for details.

```

Parameter ``value``:
    The integral value of the icon.

Returns:
    Whether or not this is a font icon (from ``entypo.ttf``).)doc";

static const char *__doc_nanogui_nvgIsImageIcon =
R"doc(Determine whether an icon ID is a texture loaded via ``nvgImageIcon``.

```
The implementation defines all ``value < 1024`` as image icons, and
everything ``>= 1024`` as an Entypo icon (see :ref:`file_nanogui_entypo.h`).
The value ``1024`` exists to provide a generous buffer on how many images
may have been loaded by NanoVG.

```

Parameter ``value``:
    The integral value of the icon.

Returns:
    Whether or not this is an image icon.)doc";

static const char *__doc_nanogui_operator_const_NVGcolor =
R"doc(Allows for conversion between nanogui::Color and the NanoVG NVGcolor
class.)doc";

static const char *__doc_nanogui_ortho =
R"doc(Creates an orthographic projection matrix.

Parameter ``left``:
    The left border of the viewport.

Parameter ``right``:
    The right border of the viewport.

Parameter ``bottom``:
    The bottom border of the viewport.

Parameter ``top``:
    The top border of the viewport.

Parameter ``nearVal``:
    The near plane.

Parameter ``farVal``:
    The far plane.)doc";

static const char *__doc_nanogui_project =
R"doc(Projects the vector ``obj`` into the specified viewport.

Performs a homogeneous transformation of a vector into "screen space",
as defined by the provided model and projection matrices, and the
dimensions of the viewport.

Parameter ``obj``:
    The vector being transformed.

Parameter ``model``:
    The model matrix.

Parameter ``proj``:
    The projection matrix.

Parameter ``viewportSize``:
    The dimensions of the viewport to project into.)doc";

static const char *__doc_nanogui_ref =
R"doc(Reference counting helper.

The *ref* template is a simple wrapper to store a pointer to an
object. It takes care of increasing and decreasing the object's
reference count as needed. When the last reference goes out of scope,
the associated object will be deallocated.

The advantage over C++ solutions such as ``std::shared_ptr`` is that
the reference count is very compactly integrated into the base object
itself.)doc";

static const char *__doc_nanogui_ref_get = R"doc(Return a const pointer to the referenced object)doc";

static const char *__doc_nanogui_ref_get_2 = R"doc(Return a pointer to the referenced object)doc";

static const char *__doc_nanogui_ref_m_ptr = R"doc()doc";

static const char *__doc_nanogui_ref_operator_T0 = R"doc(Return a pointer to the referenced object)doc";

static const char *__doc_nanogui_ref_operator_assign = R"doc(Move another reference into the current one)doc";

static const char *__doc_nanogui_ref_operator_assign_2 = R"doc(Overwrite this reference with another reference)doc";

static const char *__doc_nanogui_ref_operator_assign_3 = R"doc(Overwrite this reference with a pointer to another object)doc";

static const char *__doc_nanogui_ref_operator_bool = R"doc(Check if the object is defined)doc";

static const char *__doc_nanogui_ref_operator_eq = R"doc(Compare this reference with another reference)doc";

static const char *__doc_nanogui_ref_operator_eq_2 = R"doc(Compare this reference with a pointer)doc";

static const char *__doc_nanogui_ref_operator_mul = R"doc(Return a C++ reference to the referenced object)doc";

static const char *__doc_nanogui_ref_operator_mul_2 = R"doc(Return a const C++ reference to the referenced object)doc";

static const char *__doc_nanogui_ref_operator_ne = R"doc(Compare this reference with another reference)doc";

static const char *__doc_nanogui_ref_operator_ne_2 = R"doc(Compare this reference with a pointer)doc";

static const char *__doc_nanogui_ref_operator_sub = R"doc(Access the object referenced by this reference)doc";

static const char *__doc_nanogui_ref_operator_sub_2 = R"doc(Access the object referenced by this reference)doc";

static const char *__doc_nanogui_ref_ref = R"doc(Create a ``nullptr``-valued reference)doc";

static const char *__doc_nanogui_ref_ref_2 = R"doc(Construct a reference from a pointer)doc";

static const char *__doc_nanogui_ref_ref_3 = R"doc(Copy constructor)doc";

static const char *__doc_nanogui_ref_ref_4 = R"doc(Move constructor)doc";

static const char *__doc_nanogui_scale =
R"doc(Construct homogeneous coordinate scaling matrix

Returns a 3D homogeneous coordinate matrix that scales the X, Y, and Z
components with the corresponding entries of the 3D vector ``v``. The
``w`` component is left unchanged

Parameter ``v``:
    The vector representing the scaling for each axis.)doc";

static const char *__doc_nanogui_shutdown = R"doc(Static shutdown; should be called before the application terminates.)doc";

static const char *__doc_nanogui_translate =
R"doc(Construct homogeneous coordinate translation matrix

Returns a 3D homogeneous coordinate matrix that translates the X, Y,
and Z components by the corresponding entries of the 3D vector ``v``.
The ``w`` component is left unchanged

Parameter ``v``:
    The vector representing the translation for each axis.)doc";

static const char *__doc_nanogui_unproject =
R"doc(Unprojects the vector ``win`` out of the specified viewport.

The reverse transformation of project --- use the same matrices and
viewport dimensions to easily transition between the two spaces.

Parameter ``win``:
    The vector being transformed out of "screen space".

Parameter ``model``:
    The model matrix.

Parameter ``proj``:
    The projection matrix.

Parameter ``viewportSize``:
    The dimensions of the viewport to project out of.)doc";

static const char *__doc_nanogui_utf8 =
R"doc(Convert a single UTF32 character code to UTF8.

```
NanoGUI uses this to convert the icon character codes
defined in :ref:`file_nanogui_entypo.h`.

```

Parameter ``c``:
    The UTF32 character to be converted.)doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

