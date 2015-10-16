/*
  This file contains docstrings for the Python bindings.
  Do not edit! These were automatically extracted by mkdoc.py
 */

#define __EXPAND(x)                              x
#define __COUNT(_1, _2, _3, _4, _5, COUNT, ...)  COUNT
#define __VA_SIZE(...)                           __EXPAND(__COUNT(__VA_ARGS__, 5, 4, 3, 2, 1))
#define __CAT1(a, b)                             a ## b
#define __CAT2(a, b)                             __CAT1(a, b)
#define __DOC1(n1)                               __doc_##n1
#define __DOC2(n1, n2)                           __doc_##n1##_##n2
#define __DOC3(n1, n2, n3)                       __doc_##n1##_##n2##_##n3
#define __DOC4(n1, n2, n3, n4)                   __doc_##n1##_##n2##_##n3##_##n4
#define __DOC5(n1, n2, n3, n4, n5)               __doc_##n1##_##n2##_##n3##_##n4_##n5
#define DOC(...)                                 __EXPAND(__EXPAND(__CAT2(__DOC, __VA_SIZE(__VA_ARGS__)))(__VA_ARGS__))

#if defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


static const char *__doc_nanogui_AdvancedGridLayout = 
R"doc(Advanced Grid layout

The is a fancier grid layout with support for items that span multiple rows
or columns, and per-widget alignment flags. Each row and column
additionally stores a stretch factor that controls how additional space is
redistributed. The downside of this flexibility is that a layout anchor
data structure must be provided for each widget.

An example:

<pre> using AdvancedGridLayout::Anchor; Label *label = new Label(window, "A
label"); // Add a centered label at grid position (1, 5), which spans two
horizontal cells layout->setAnchor(label, Anchor(1, 5, 2, 1,
Alignent::Middle, Alignment::Middle)); </pre>

The grid is initialized with user-specified column and row size vectors
(which can be expanded later on if desired). If a size value of zero is
specified for a column or row, the size is set to the maximum preferred
size of any widgets contained in the same row or column. Any remaining
space is redistributed according to the row and column stretch factors.

The high level usage somewhat resembles the classic HIG layout: https://web
.archive.org/web/20070813221705/http://www.autel.cz/dmi/tutorial.html
https://github.com/jaapgeurts/higlayout)doc";

static const char *__doc_nanogui_AdvancedGridLayout_AdvancedGridLayout = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_Anchor = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_Anchor_2 = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_Anchor_3 = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_align = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_pos = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_Anchor_size = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_anchor = R"doc(Retrieve the anchor data structure for a given widget)doc";

static const char *__doc_nanogui_AdvancedGridLayout_appendCol = R"doc(Append a column of the given size (and stretch factor))doc";

static const char *__doc_nanogui_AdvancedGridLayout_appendRow = R"doc(Append a row of the given size (and stretch factor))doc";

static const char *__doc_nanogui_AdvancedGridLayout_colCount = R"doc(Return the number of cols)doc";

static const char *__doc_nanogui_AdvancedGridLayout_computeLayout = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_mAnchor = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_mColStretch = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_mCols = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_mMargin = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_mRowStretch = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_mRows = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_margin = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_performLayout = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_preferredSize = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_rowCount = R"doc(Return the number of rows)doc";

static const char *__doc_nanogui_AdvancedGridLayout_setAnchor = R"doc(Specify the anchor data structure for a given widget)doc";

static const char *__doc_nanogui_AdvancedGridLayout_setColStretch = R"doc(Set the stretch factor of a given column)doc";

static const char *__doc_nanogui_AdvancedGridLayout_setMargin = R"doc()doc";

static const char *__doc_nanogui_AdvancedGridLayout_setRowStretch = R"doc(Set the stretch factor of a given row)doc";

static const char *__doc_nanogui_Arcball = R"doc(Arcball helper class to interactively rotate objects on-screen)doc";

static const char *__doc_nanogui_Arcball_Arcball = R"doc()doc";

static const char *__doc_nanogui_Arcball_Arcball_2 = R"doc()doc";

static const char *__doc_nanogui_Arcball_active = R"doc()doc";

static const char *__doc_nanogui_Arcball_button = R"doc()doc";

static const char *__doc_nanogui_Arcball_mActive = R"doc()doc";

static const char *__doc_nanogui_Arcball_mIncr = R"doc()doc";

static const char *__doc_nanogui_Arcball_mLastPos = R"doc()doc";

static const char *__doc_nanogui_Arcball_mQuat = R"doc()doc";

static const char *__doc_nanogui_Arcball_mSize = R"doc()doc";

static const char *__doc_nanogui_Arcball_mSpeedFactor = R"doc()doc";

static const char *__doc_nanogui_Arcball_matrix = R"doc()doc";

static const char *__doc_nanogui_Arcball_motion = R"doc()doc";

static const char *__doc_nanogui_Arcball_setSize = R"doc()doc";

static const char *__doc_nanogui_Arcball_setSpeedFactor = R"doc()doc";

static const char *__doc_nanogui_Arcball_setState = R"doc()doc";

static const char *__doc_nanogui_Arcball_size = R"doc()doc";

static const char *__doc_nanogui_Arcball_speedFactor = R"doc()doc";

static const char *__doc_nanogui_Arcball_state = R"doc()doc";

static const char *__doc_nanogui_BoxLayout = 
R"doc(Simple horizontal/vertical box layout

This widget stacks up a bunch of widgets horizontally or vertically. It
adds margins around the entire container and a custom spacing between
adjacent widgets)doc";

static const char *__doc_nanogui_BoxLayout_BoxLayout = 
R"doc(Construct a box layout which packs widgets in the given ``orientation``

Parameter ``alignment``:
    Widget alignment perpendicular to the chosen orientation.

Parameter ``margin``:
    Margin around the layout container

Parameter ``spacing``:
    Extra spacing placed between widgets)doc";

static const char *__doc_nanogui_BoxLayout_alignment = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_mAlignment = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_mMargin = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_mOrientation = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_mSpacing = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_margin = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_orientation = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_performLayout = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_preferredSize = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_setAlignment = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_setMargin = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_setOrientation = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_setSpacing = R"doc()doc";

static const char *__doc_nanogui_BoxLayout_spacing = R"doc()doc";

static const char *__doc_nanogui_Button = R"doc()doc";

static const char *__doc_nanogui_Button_Button = R"doc()doc";

static const char *__doc_nanogui_Button_backgroundColor = R"doc()doc";

static const char *__doc_nanogui_Button_buttonFlags = R"doc()doc";

static const char *__doc_nanogui_Button_buttonGroup = R"doc()doc";

static const char *__doc_nanogui_Button_callback = R"doc(Set the push callback (for any type of button))doc";

static const char *__doc_nanogui_Button_caption = R"doc()doc";

static const char *__doc_nanogui_Button_changeCallback = R"doc(Set the change callback (for toggle buttons))doc";

static const char *__doc_nanogui_Button_draw = R"doc()doc";

static const char *__doc_nanogui_Button_icon = R"doc()doc";

static const char *__doc_nanogui_Button_iconPosition = R"doc()doc";

static const char *__doc_nanogui_Button_mBackgroundColor = R"doc()doc";

static const char *__doc_nanogui_Button_mButtonGroup = R"doc()doc";

static const char *__doc_nanogui_Button_mCallback = R"doc()doc";

static const char *__doc_nanogui_Button_mCaption = R"doc()doc";

static const char *__doc_nanogui_Button_mChangeCallback = R"doc()doc";

static const char *__doc_nanogui_Button_mFlags = R"doc()doc";

static const char *__doc_nanogui_Button_mIcon = R"doc()doc";

static const char *__doc_nanogui_Button_mIconPosition = R"doc()doc";

static const char *__doc_nanogui_Button_mPushed = R"doc()doc";

static const char *__doc_nanogui_Button_mTextColor = R"doc()doc";

static const char *__doc_nanogui_Button_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_Button_preferredSize = R"doc()doc";

static const char *__doc_nanogui_Button_pushed = R"doc()doc";

static const char *__doc_nanogui_Button_setBackgroundColor = R"doc()doc";

static const char *__doc_nanogui_Button_setButtonGroup = R"doc(Set the button group (for radio buttons))doc";

static const char *__doc_nanogui_Button_setCallback = R"doc()doc";

static const char *__doc_nanogui_Button_setCaption = R"doc()doc";

static const char *__doc_nanogui_Button_setChangeCallback = R"doc()doc";

static const char *__doc_nanogui_Button_setFlags = R"doc()doc";

static const char *__doc_nanogui_Button_setIcon = R"doc()doc";

static const char *__doc_nanogui_Button_setIconPosition = R"doc()doc";

static const char *__doc_nanogui_Button_setPushed = R"doc()doc";

static const char *__doc_nanogui_Button_setTextColor = R"doc()doc";

static const char *__doc_nanogui_Button_textColor = R"doc()doc";

static const char *__doc_nanogui_CheckBox = R"doc()doc";

static const char *__doc_nanogui_CheckBox_CheckBox = R"doc()doc";

static const char *__doc_nanogui_CheckBox_callback = R"doc()doc";

static const char *__doc_nanogui_CheckBox_caption = R"doc()doc";

static const char *__doc_nanogui_CheckBox_checked = R"doc()doc";

static const char *__doc_nanogui_CheckBox_draw = R"doc()doc";

static const char *__doc_nanogui_CheckBox_mCallback = R"doc()doc";

static const char *__doc_nanogui_CheckBox_mCaption = R"doc()doc";

static const char *__doc_nanogui_CheckBox_mChecked = R"doc()doc";

static const char *__doc_nanogui_CheckBox_mPushed = R"doc()doc";

static const char *__doc_nanogui_CheckBox_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_CheckBox_preferredSize = R"doc()doc";

static const char *__doc_nanogui_CheckBox_pushed = R"doc()doc";

static const char *__doc_nanogui_CheckBox_setCallback = R"doc()doc";

static const char *__doc_nanogui_CheckBox_setCaption = R"doc()doc";

static const char *__doc_nanogui_CheckBox_setChecked = R"doc()doc";

static const char *__doc_nanogui_CheckBox_setPushed = R"doc()doc";

static const char *__doc_nanogui_Color = R"doc(Stores an RGBA color value)doc";

static const char *__doc_nanogui_ColorPicker = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_ColorPicker = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_callback = R"doc(Set the change callback)doc";

static const char *__doc_nanogui_ColorPicker_color = R"doc(Get the current color)doc";

static const char *__doc_nanogui_ColorPicker_mCallback = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_mColorWheel = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_mPickButton = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_setCallback = R"doc()doc";

static const char *__doc_nanogui_ColorPicker_setColor = R"doc(Set the current color)doc";

static const char *__doc_nanogui_ColorWheel = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_ColorWheel = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_adjustPosition = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_callback = R"doc(Set the change callback)doc";

static const char *__doc_nanogui_ColorWheel_color = R"doc(Get the current color)doc";

static const char *__doc_nanogui_ColorWheel_draw = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_hue2rgb = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_mBlack = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_mCallback = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_mDragRegion = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_mHue = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_mWhite = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_mouseDragEvent = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_preferredSize = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_setCallback = R"doc()doc";

static const char *__doc_nanogui_ColorWheel_setColor = R"doc(Set the current color)doc";

static const char *__doc_nanogui_Color_Color = R"doc()doc";

static const char *__doc_nanogui_Color_Color_10 = R"doc()doc";

static const char *__doc_nanogui_Color_Color_11 = R"doc()doc";

static const char *__doc_nanogui_Color_Color_12 = R"doc(Construct a color vector from MatrixBase (needed to play nice with Eigen))doc";

static const char *__doc_nanogui_Color_Color_2 = R"doc()doc";

static const char *__doc_nanogui_Color_Color_3 = R"doc()doc";

static const char *__doc_nanogui_Color_Color_4 = R"doc()doc";

static const char *__doc_nanogui_Color_Color_5 = R"doc()doc";

static const char *__doc_nanogui_Color_Color_6 = R"doc()doc";

static const char *__doc_nanogui_Color_Color_7 = R"doc()doc";

static const char *__doc_nanogui_Color_Color_8 = R"doc()doc";

static const char *__doc_nanogui_Color_Color_9 = R"doc()doc";

static const char *__doc_nanogui_Color_b = R"doc(Return a reference to the blue channel)doc";

static const char *__doc_nanogui_Color_b_2 = R"doc(Return a reference to the blue channel (const version))doc";

static const char *__doc_nanogui_Color_contrastingColor = R"doc()doc";

static const char *__doc_nanogui_Color_g = R"doc(Return a reference to the green channel)doc";

static const char *__doc_nanogui_Color_g_2 = R"doc(Return a reference to the green channel (const version))doc";

static const char *__doc_nanogui_Color_operator_assign = R"doc(Assign a color vector from MatrixBase (needed to play nice with Eigen))doc";

static const char *__doc_nanogui_Color_r = R"doc(Return a reference to the red channel)doc";

static const char *__doc_nanogui_Color_r_2 = R"doc(Return a reference to the red channel (const version))doc";

static const char *__doc_nanogui_ComboBox = R"doc()doc";

static const char *__doc_nanogui_ComboBox_ComboBox = R"doc(Create an empty combo box)doc";

static const char *__doc_nanogui_ComboBox_ComboBox_2 = R"doc(Create a new combo box with the given items)doc";

static const char *__doc_nanogui_ComboBox_ComboBox_3 = 
R"doc(Create a new combo box with the given items, providing both short and long
descriptive labels for each item)doc";

static const char *__doc_nanogui_ComboBox_callback = R"doc()doc";

static const char *__doc_nanogui_ComboBox_items = R"doc()doc";

static const char *__doc_nanogui_ComboBox_itemsShort = R"doc()doc";

static const char *__doc_nanogui_ComboBox_mCallback = R"doc()doc";

static const char *__doc_nanogui_ComboBox_mItems = R"doc()doc";

static const char *__doc_nanogui_ComboBox_mItemsShort = R"doc()doc";

static const char *__doc_nanogui_ComboBox_mSelectedIndex = R"doc()doc";

static const char *__doc_nanogui_ComboBox_selectedIndex = R"doc()doc";

static const char *__doc_nanogui_ComboBox_setCallback = R"doc()doc";

static const char *__doc_nanogui_ComboBox_setItems = R"doc()doc";

static const char *__doc_nanogui_ComboBox_setItems_2 = R"doc()doc";

static const char *__doc_nanogui_ComboBox_setSelectedIndex = R"doc()doc";

static const char *__doc_nanogui_FloatBox = R"doc()doc";

static const char *__doc_nanogui_FloatBox_FloatBox_Scalar_ = R"doc()doc";

static const char *__doc_nanogui_FloatBox_setCallback = R"doc()doc";

static const char *__doc_nanogui_FloatBox_setValue = R"doc()doc";

static const char *__doc_nanogui_FloatBox_value = R"doc()doc";

static const char *__doc_nanogui_FormHelper = 
R"doc(Convenience class to create simple AntTweakBar-style layouts that expose
variables of various types using NanoGUI widgets

Example:

<pre> [ ... initialize NanoGUI, construct screen ... ]

FormHelper* h = new FormHelper(screen);

// Add a new windows widget h->addWindow(Eigen::Vector2i(10,10),"Menu");

// Start a new group h->addGroup("Group 1");

// Expose an integer variable by reference h->addVariable("integer
variable", aInt);

// Expose a float variable via setter/getter functions h->addVariable(
[&](float value){ aFloat = value; }, [&](){ return *aFloat; }, "float
variable");

// add a new button h->addButton("Button",[&](){ std::cout << "Button
pressed" << std::endl; }); </pre>)doc";

static const char *__doc_nanogui_FormHelper_FormHelper = R"doc(Create a helper class to construct NanoGUI widgets on the given screen)doc";

static const char *__doc_nanogui_FormHelper_addButton = R"doc(Add a button with a custom callback)doc";

static const char *__doc_nanogui_FormHelper_addGroup = R"doc(Add a new group that may contain several sub-widgets)doc";

static const char *__doc_nanogui_FormHelper_addVariable = R"doc(Add a new data widget controlled using custom getter/setter functions)doc";

static const char *__doc_nanogui_FormHelper_addVariable_2 = R"doc(Add a new data widget that exposes a raw variable in memory)doc";

static const char *__doc_nanogui_FormHelper_addWidget = R"doc(Add an arbitrary (optionally labeled) widget to the layout)doc";

static const char *__doc_nanogui_FormHelper_addWindow = R"doc(Add a new top-level window)doc";

static const char *__doc_nanogui_FormHelper_fixedSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_groupFontName = R"doc()doc";

static const char *__doc_nanogui_FormHelper_groupFontSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_labelFontName = R"doc()doc";

static const char *__doc_nanogui_FormHelper_labelFontSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mFixedSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mGroupFontName = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mGroupFontSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mLabelFontName = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mLabelFontSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mLayout = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mPostGroupSpacing = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mPreGroupSpacing = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mRefreshCallbacks = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mScreen = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mVariableSpacing = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mWidgetFontSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_mWindow = R"doc()doc";

static const char *__doc_nanogui_FormHelper_refresh = R"doc(Cause all widgets to re-synchronize with the underlying variable state)doc";

static const char *__doc_nanogui_FormHelper_setFixedSize = R"doc(Specify a fixed size for newly added widgets)doc";

static const char *__doc_nanogui_FormHelper_setGroupFontName = R"doc()doc";

static const char *__doc_nanogui_FormHelper_setGroupFontSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_setLabelFontName = R"doc()doc";

static const char *__doc_nanogui_FormHelper_setLabelFontSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_setWidgetFontSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_setWindow = R"doc()doc";

static const char *__doc_nanogui_FormHelper_widgetFontSize = R"doc()doc";

static const char *__doc_nanogui_FormHelper_window = R"doc(Access the currently active Window instance)doc";

static const char *__doc_nanogui_GLFramebuffer = R"doc(Helper class for creating framebuffer objects)doc";

static const char *__doc_nanogui_GLFramebuffer_GLFramebuffer = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_bind = R"doc(Bind the framebuffer boject)doc";

static const char *__doc_nanogui_GLFramebuffer_blit = R"doc(Blit the framebuffer object onto the screen)doc";

static const char *__doc_nanogui_GLFramebuffer_free = R"doc(Release all associated resources)doc";

static const char *__doc_nanogui_GLFramebuffer_init = R"doc(Create a new framebuffer with the specified size and number of MSAA samples)doc";

static const char *__doc_nanogui_GLFramebuffer_mColor = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_mDepth = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_mFramebuffer = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_mSamples = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_mSize = R"doc()doc";

static const char *__doc_nanogui_GLFramebuffer_ready = R"doc(Return whether or not the framebuffer object has been initialized)doc";

static const char *__doc_nanogui_GLFramebuffer_release = R"doc(Release/unbind the framebuffer object)doc";

static const char *__doc_nanogui_GLFramebuffer_samples = R"doc(Return the number of MSAA samples)doc";

static const char *__doc_nanogui_GLShader = 
R"doc(Helper class for compiling and linking OpenGL shaders and uploading
associated vertex and index buffers from Eigen matrices)doc";

static const char *__doc_nanogui_GLShader_Buffer = R"doc()doc";

static const char *__doc_nanogui_GLShader_Buffer_compSize = R"doc()doc";

static const char *__doc_nanogui_GLShader_Buffer_dim = R"doc()doc";

static const char *__doc_nanogui_GLShader_Buffer_glType = R"doc()doc";

static const char *__doc_nanogui_GLShader_Buffer_id = R"doc()doc";

static const char *__doc_nanogui_GLShader_Buffer_size = R"doc()doc";

static const char *__doc_nanogui_GLShader_Buffer_version = R"doc()doc";

static const char *__doc_nanogui_GLShader_GLShader = R"doc(Create an unitialized OpenGL shader)doc";

static const char *__doc_nanogui_GLShader_attrib = R"doc(Return the handle of a named shader attribute (-1 if it does not exist))doc";

static const char *__doc_nanogui_GLShader_attribVersion = R"doc(Return the version number of a given attribute)doc";

static const char *__doc_nanogui_GLShader_bind = R"doc(Select this shader for subsequent draw calls)doc";

static const char *__doc_nanogui_GLShader_bufferSize = R"doc(Return the size of all registered buffers in bytes)doc";

static const char *__doc_nanogui_GLShader_define = R"doc(Set a preprocessor definition)doc";

static const char *__doc_nanogui_GLShader_downloadAttrib = R"doc(Download a vertex buffer object into an Eigen matrix)doc";

static const char *__doc_nanogui_GLShader_downloadAttrib_2 = R"doc()doc";

static const char *__doc_nanogui_GLShader_drawArray = R"doc(Draw a sequence of primitives)doc";

static const char *__doc_nanogui_GLShader_drawIndexed = R"doc(Draw a sequence of primitives using a previously uploaded index buffer)doc";

static const char *__doc_nanogui_GLShader_free = R"doc(Release underlying OpenGL objects)doc";

static const char *__doc_nanogui_GLShader_freeAttrib = R"doc(Completely free an existing attribute buffer)doc";

static const char *__doc_nanogui_GLShader_hasAttrib = R"doc(Check if an attribute was registered a given name)doc";

static const char *__doc_nanogui_GLShader_init = R"doc(Initialize the shader using the specified source strings)doc";

static const char *__doc_nanogui_GLShader_initFromFiles = R"doc(Initialize the shader using the specified files on disk)doc";

static const char *__doc_nanogui_GLShader_invalidateAttribss = R"doc(Invalidate the version numbers assiciated with attribute data)doc";

static const char *__doc_nanogui_GLShader_mBufferObjects = R"doc()doc";

static const char *__doc_nanogui_GLShader_mDefinitions = R"doc()doc";

static const char *__doc_nanogui_GLShader_mFragmentShader = R"doc()doc";

static const char *__doc_nanogui_GLShader_mGeometryShader = R"doc()doc";

static const char *__doc_nanogui_GLShader_mName = R"doc()doc";

static const char *__doc_nanogui_GLShader_mProgramShader = R"doc()doc";

static const char *__doc_nanogui_GLShader_mVertexArrayObject = R"doc()doc";

static const char *__doc_nanogui_GLShader_mVertexShader = R"doc()doc";

static const char *__doc_nanogui_GLShader_name = R"doc(Return the name of the shader)doc";

static const char *__doc_nanogui_GLShader_resetAttribVersion = R"doc(Reset the version number of a given attribute)doc";

static const char *__doc_nanogui_GLShader_setUniform = R"doc(Initialize a uniform parameter with a 4x4 matrix)doc";

static const char *__doc_nanogui_GLShader_setUniform_2 = R"doc(Initialize a uniform parameter with an integer value)doc";

static const char *__doc_nanogui_GLShader_setUniform_3 = R"doc(Initialize a uniform parameter with a float value)doc";

static const char *__doc_nanogui_GLShader_setUniform_4 = R"doc(Initialize a uniform parameter with a 2D vector)doc";

static const char *__doc_nanogui_GLShader_setUniform_5 = R"doc(Initialize a uniform parameter with a 3D vector)doc";

static const char *__doc_nanogui_GLShader_setUniform_6 = R"doc(Initialize a uniform parameter with a 4D vector)doc";

static const char *__doc_nanogui_GLShader_shareAttrib = 
R"doc(Create a symbolic link to an attribute of another GLShader. This avoids
duplicating unnecessary data)doc";

static const char *__doc_nanogui_GLShader_uniform = R"doc(Return the handle of a uniform attribute (-1 if it does not exist))doc";

static const char *__doc_nanogui_GLShader_uploadAttrib = R"doc(Upload an Eigen matrix as a vertex buffer object (refreshing it as needed))doc";

static const char *__doc_nanogui_GLShader_uploadAttrib_2 = R"doc()doc";

static const char *__doc_nanogui_GLShader_uploadIndices = R"doc(Upload an index buffer)doc";

static const char *__doc_nanogui_Graph = R"doc()doc";

static const char *__doc_nanogui_Graph_Graph = R"doc()doc";

static const char *__doc_nanogui_Graph_backgroundColor = R"doc()doc";

static const char *__doc_nanogui_Graph_caption = R"doc()doc";

static const char *__doc_nanogui_Graph_draw = R"doc()doc";

static const char *__doc_nanogui_Graph_footer = R"doc()doc";

static const char *__doc_nanogui_Graph_foregroundColor = R"doc()doc";

static const char *__doc_nanogui_Graph_header = R"doc()doc";

static const char *__doc_nanogui_Graph_mBackgroundColor = R"doc()doc";

static const char *__doc_nanogui_Graph_mCaption = R"doc()doc";

static const char *__doc_nanogui_Graph_mFooter = R"doc()doc";

static const char *__doc_nanogui_Graph_mForegroundColor = R"doc()doc";

static const char *__doc_nanogui_Graph_mHeader = R"doc()doc";

static const char *__doc_nanogui_Graph_mTextColor = R"doc()doc";

static const char *__doc_nanogui_Graph_mValues = R"doc()doc";

static const char *__doc_nanogui_Graph_preferredSize = R"doc()doc";

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
R"doc(Grid layout

Widgets are arranged in a grid that has a fixed grid resolution
``resolution`` along one of the axes. The layout orientation indicates the
fixed dimension; widgets are also appended on this axis. The spacing
between items can be specified per axis. The horizontal/vertical alignment
can be specified per row and column.)doc";

static const char *__doc_nanogui_GridLayout_GridLayout = R"doc(Create a 2-column grid layout by default)doc";

static const char *__doc_nanogui_GridLayout_alignment = R"doc()doc";

static const char *__doc_nanogui_GridLayout_computeLayout = R"doc()doc";

static const char *__doc_nanogui_GridLayout_mAlignment = R"doc()doc";

static const char *__doc_nanogui_GridLayout_mDefaultAlignment = R"doc()doc";

static const char *__doc_nanogui_GridLayout_mMargin = R"doc()doc";

static const char *__doc_nanogui_GridLayout_mOrientation = R"doc()doc";

static const char *__doc_nanogui_GridLayout_mResolution = R"doc()doc";

static const char *__doc_nanogui_GridLayout_mSpacing = R"doc()doc";

static const char *__doc_nanogui_GridLayout_margin = R"doc()doc";

static const char *__doc_nanogui_GridLayout_orientation = R"doc()doc";

static const char *__doc_nanogui_GridLayout_performLayout = R"doc()doc";

static const char *__doc_nanogui_GridLayout_preferredSize = R"doc()doc";

static const char *__doc_nanogui_GridLayout_resolution = R"doc()doc";

static const char *__doc_nanogui_GridLayout_setColAlignment = R"doc()doc";

static const char *__doc_nanogui_GridLayout_setColAlignment_2 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_setMargin = R"doc()doc";

static const char *__doc_nanogui_GridLayout_setOrientation = R"doc()doc";

static const char *__doc_nanogui_GridLayout_setResolution = R"doc()doc";

static const char *__doc_nanogui_GridLayout_setRowAlignment = R"doc()doc";

static const char *__doc_nanogui_GridLayout_setRowAlignment_2 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_setSpacing = R"doc()doc";

static const char *__doc_nanogui_GridLayout_setSpacing_2 = R"doc()doc";

static const char *__doc_nanogui_GridLayout_spacing = R"doc()doc";

static const char *__doc_nanogui_GroupLayout = 
R"doc(Special layout for widgets grouped by labels

This widget resembles a box layout in that it arranges a set of widgets
vertically. All widgets are indented on the horizontal axis except for
Label widgets, which are not indented.

This creates a pleasing layout where a number of widgets are grouped under
some high-level heading.)doc";

static const char *__doc_nanogui_GroupLayout_GroupLayout = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_groupIndent = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_groupSpacing = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_mGroupIndent = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_mGroupSpacing = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_mMargin = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_mSpacing = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_margin = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_performLayout = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_preferredSize = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_setGroupIndent = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_setGroupSpacing = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_setMargin = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_setSpacing = R"doc()doc";

static const char *__doc_nanogui_GroupLayout_spacing = R"doc()doc";

static const char *__doc_nanogui_ImagePanel = R"doc()doc";

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

static const char *__doc_nanogui_ImagePanel_preferredSize = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_setCallback = R"doc()doc";

static const char *__doc_nanogui_ImagePanel_setImages = R"doc()doc";

static const char *__doc_nanogui_ImageView = R"doc()doc";

static const char *__doc_nanogui_ImageView_ImageView = R"doc()doc";

static const char *__doc_nanogui_ImageView_draw = R"doc()doc";

static const char *__doc_nanogui_ImageView_image = R"doc()doc";

static const char *__doc_nanogui_ImageView_mImage = R"doc()doc";

static const char *__doc_nanogui_ImageView_preferredSize = R"doc()doc";

static const char *__doc_nanogui_ImageView_setImage = R"doc()doc";

static const char *__doc_nanogui_IntBox = R"doc()doc";

static const char *__doc_nanogui_IntBox_IntBox_Scalar_ = R"doc()doc";

static const char *__doc_nanogui_IntBox_setCallback = R"doc()doc";

static const char *__doc_nanogui_IntBox_setValue = R"doc()doc";

static const char *__doc_nanogui_IntBox_value = R"doc()doc";

static const char *__doc_nanogui_Label = 
R"doc(Text label widget

The font and color can be customized. When Widget::setFixedWidth() is used,
the text is wrapped when it surpasses the specified width)doc";

static const char *__doc_nanogui_Label_Label = R"doc()doc";

static const char *__doc_nanogui_Label_caption = R"doc(Get the label's text caption)doc";

static const char *__doc_nanogui_Label_color = R"doc(Get the label color)doc";

static const char *__doc_nanogui_Label_draw = R"doc(Draw the label)doc";

static const char *__doc_nanogui_Label_font = R"doc(Get the currently active font)doc";

static const char *__doc_nanogui_Label_mCaption = R"doc()doc";

static const char *__doc_nanogui_Label_mColor = R"doc()doc";

static const char *__doc_nanogui_Label_mFont = R"doc()doc";

static const char *__doc_nanogui_Label_preferredSize = R"doc(Compute the size needed to fully display the label)doc";

static const char *__doc_nanogui_Label_setCaption = R"doc(Set the label's text caption)doc";

static const char *__doc_nanogui_Label_setColor = R"doc(Set the label color)doc";

static const char *__doc_nanogui_Label_setFont = 
R"doc(Set the currently active font (2 are available by default: 'sans' and
'sans-bold'))doc";

static const char *__doc_nanogui_Layout = R"doc(Basic interface of a layout engine)doc";

static const char *__doc_nanogui_Layout_performLayout = R"doc()doc";

static const char *__doc_nanogui_Layout_preferredSize = R"doc()doc";

static const char *__doc_nanogui_MessageDialog = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_MessageDialog = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_callback = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_mCallback = R"doc()doc";

static const char *__doc_nanogui_MessageDialog_setCallback = R"doc()doc";

static const char *__doc_nanogui_Object = R"doc(Reference counted object base class)doc";

static const char *__doc_nanogui_Object_Object = R"doc(Default constructor)doc";

static const char *__doc_nanogui_Object_Object_2 = R"doc(Copy constructor)doc";

static const char *__doc_nanogui_Object_decRef = 
R"doc(Decrease the reference count of the object and possibly deallocate it.

The object will automatically be deallocated once the reference count
reaches zero.)doc";

static const char *__doc_nanogui_Object_getRefCount = R"doc(Return the current reference count)doc";

static const char *__doc_nanogui_Object_incRef = R"doc(Increase the object's reference count by one)doc";

static const char *__doc_nanogui_Object_m_refCount = R"doc()doc";

static const char *__doc_nanogui_Popup = 
R"doc(Popup window for combo boxes, popup buttons, nested dialogs etc.

Usually the Popup instance is constructed by another widget (e.g.
PopupButton) and does not need to be created by hand.)doc";

static const char *__doc_nanogui_PopupButton = R"doc()doc";

static const char *__doc_nanogui_PopupButton_PopupButton = R"doc()doc";

static const char *__doc_nanogui_PopupButton_chevronIcon = R"doc()doc";

static const char *__doc_nanogui_PopupButton_draw = R"doc()doc";

static const char *__doc_nanogui_PopupButton_mChevronIcon = R"doc()doc";

static const char *__doc_nanogui_PopupButton_mPopup = R"doc()doc";

static const char *__doc_nanogui_PopupButton_performLayout = R"doc()doc";

static const char *__doc_nanogui_PopupButton_popup = R"doc()doc";

static const char *__doc_nanogui_PopupButton_popup_2 = R"doc()doc";

static const char *__doc_nanogui_PopupButton_preferredSize = R"doc()doc";

static const char *__doc_nanogui_PopupButton_setChevronIcon = R"doc()doc";

static const char *__doc_nanogui_Popup_Popup = 
R"doc(Create a new popup parented to a screen (first argument) and a parent
window)doc";

static const char *__doc_nanogui_Popup_anchorHeight = 
R"doc(Return the anchor height; this determines the vertical shift relative to
the anchor position)doc";

static const char *__doc_nanogui_Popup_anchorPos = 
R"doc(Set the anchor position in the parent window; the placement of the popup is
relative to it)doc";

static const char *__doc_nanogui_Popup_draw = R"doc(Draw the popup window)doc";

static const char *__doc_nanogui_Popup_mAnchorHeight = R"doc()doc";

static const char *__doc_nanogui_Popup_mAnchorPos = R"doc()doc";

static const char *__doc_nanogui_Popup_mParentWindow = R"doc()doc";

static const char *__doc_nanogui_Popup_parentWindow = R"doc(Return the parent window of the popup)doc";

static const char *__doc_nanogui_Popup_parentWindow_2 = R"doc(Return the parent window of the popup)doc";

static const char *__doc_nanogui_Popup_performLayout = 
R"doc(Invoke the associated layout generator to properly place child widgets, if
any)doc";

static const char *__doc_nanogui_Popup_refreshRelativePlacement = R"doc(Internal helper function to maintain nested window position values)doc";

static const char *__doc_nanogui_Popup_setAnchorHeight = 
R"doc(Set the anchor height; this determines the vertical shift relative to the
anchor position)doc";

static const char *__doc_nanogui_Popup_setAnchorPos = 
R"doc(Return the anchor position in the parent window; the placement of the popup
is relative to it)doc";

static const char *__doc_nanogui_ProgressBar = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_ProgressBar = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_draw = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_mValue = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_preferredSize = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_setValue = R"doc()doc";

static const char *__doc_nanogui_ProgressBar_value = R"doc()doc";

static const char *__doc_nanogui_Screen = 
R"doc(Represents a display surface (i.e. a full-screen or windowed GLFW window)
and forms the root element of a hierarchy of nanogui widgets)doc";

static const char *__doc_nanogui_Screen_Screen = R"doc(Create a new screen)doc";

static const char *__doc_nanogui_Screen_Screen_2 = 
R"doc(Default constructor

Performs no initialization at all. Use this if the application is
responsible for setting up GLFW, OpenGL, etc.

In this case, override Screen and call initalize() with a pointer to an
existing ``GLFWwindow`` instance

You will also be responsible in this case to deliver GLFW callbacks to the
appropriate callback event handlers below)doc";

static const char *__doc_nanogui_Screen_background = R"doc(Return the screen's background color)doc";

static const char *__doc_nanogui_Screen_caption = R"doc(Get the window titlebar caption)doc";

static const char *__doc_nanogui_Screen_centerWindow = R"doc()doc";

static const char *__doc_nanogui_Screen_charCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_cursorPosCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_disposeWindow = R"doc()doc";

static const char *__doc_nanogui_Screen_drawAll = R"doc(Draw the Screen contents)doc";

static const char *__doc_nanogui_Screen_drawContents = R"doc(Draw the window contents -- put your OpenGL draw calls here)doc";

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

static const char *__doc_nanogui_Screen_mGLFWWindow = R"doc()doc";

static const char *__doc_nanogui_Screen_mLastInteraction = R"doc()doc";

static const char *__doc_nanogui_Screen_mModifiers = R"doc()doc";

static const char *__doc_nanogui_Screen_mMousePos = R"doc()doc";

static const char *__doc_nanogui_Screen_mMouseState = R"doc()doc";

static const char *__doc_nanogui_Screen_mNVGContext = R"doc()doc";

static const char *__doc_nanogui_Screen_mPixelRatio = R"doc()doc";

static const char *__doc_nanogui_Screen_mProcessEvents = R"doc()doc";

static const char *__doc_nanogui_Screen_mShutdownGLFWOnDestruct = R"doc()doc";

static const char *__doc_nanogui_Screen_mouseButtonCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_mousePos = R"doc(Return the last observed mouse position value)doc";

static const char *__doc_nanogui_Screen_moveWindowToFront = R"doc()doc";

static const char *__doc_nanogui_Screen_nvgContext = R"doc(Return a pointer to the underlying nanoVG draw context)doc";

static const char *__doc_nanogui_Screen_performLayout = R"doc(Compute the layout of all widgets)doc";

static const char *__doc_nanogui_Screen_performLayout_2 = R"doc()doc";

static const char *__doc_nanogui_Screen_resizeCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_resizeEvent = R"doc(Window resize event handler)doc";

static const char *__doc_nanogui_Screen_scrollCallbackEvent = R"doc()doc";

static const char *__doc_nanogui_Screen_setBackground = R"doc(Set the screen's background color)doc";

static const char *__doc_nanogui_Screen_setCaption = R"doc(Set the window titlebar caption)doc";

static const char *__doc_nanogui_Screen_setShutdownGLFWOnDestruct = R"doc()doc";

static const char *__doc_nanogui_Screen_setSize = R"doc(Set window size)doc";

static const char *__doc_nanogui_Screen_setVisible = R"doc(Set the top-level window visibility (no effect on full-screen windows))doc";

static const char *__doc_nanogui_Screen_shutdownGLFWOnDestruct = R"doc()doc";

static const char *__doc_nanogui_Screen_updateFocus = R"doc()doc";

static const char *__doc_nanogui_Slider = R"doc()doc";

static const char *__doc_nanogui_Slider_Slider = R"doc()doc";

static const char *__doc_nanogui_Slider_callback = R"doc()doc";

static const char *__doc_nanogui_Slider_draw = R"doc()doc";

static const char *__doc_nanogui_Slider_finalCallback = R"doc()doc";

static const char *__doc_nanogui_Slider_highlightColor = R"doc()doc";

static const char *__doc_nanogui_Slider_highlightedRange = R"doc()doc";

static const char *__doc_nanogui_Slider_mCallback = R"doc()doc";

static const char *__doc_nanogui_Slider_mFinalCallback = R"doc()doc";

static const char *__doc_nanogui_Slider_mHighlightColor = R"doc()doc";

static const char *__doc_nanogui_Slider_mHighlightedRange = R"doc()doc";

static const char *__doc_nanogui_Slider_mValue = R"doc()doc";

static const char *__doc_nanogui_Slider_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_Slider_mouseDragEvent = R"doc()doc";

static const char *__doc_nanogui_Slider_preferredSize = R"doc()doc";

static const char *__doc_nanogui_Slider_setCallback = R"doc()doc";

static const char *__doc_nanogui_Slider_setFinalCallback = R"doc()doc";

static const char *__doc_nanogui_Slider_setHighlightColor = R"doc()doc";

static const char *__doc_nanogui_Slider_setHighlightedRange = R"doc()doc";

static const char *__doc_nanogui_Slider_setValue = R"doc()doc";

static const char *__doc_nanogui_Slider_value = R"doc()doc";

static const char *__doc_nanogui_TextBox = R"doc()doc";

static const char *__doc_nanogui_TextBox_TextBox = R"doc()doc";

static const char *__doc_nanogui_TextBox_alignment = R"doc()doc";

static const char *__doc_nanogui_TextBox_callback = R"doc(Set the change callback)doc";

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

static const char *__doc_nanogui_TextBox_mSelectionPos = R"doc()doc";

static const char *__doc_nanogui_TextBox_mTextOffset = R"doc()doc";

static const char *__doc_nanogui_TextBox_mUnits = R"doc()doc";

static const char *__doc_nanogui_TextBox_mUnitsImage = R"doc()doc";

static const char *__doc_nanogui_TextBox_mValidFormat = R"doc()doc";

static const char *__doc_nanogui_TextBox_mValue = R"doc()doc";

static const char *__doc_nanogui_TextBox_mValueTemp = R"doc()doc";

static const char *__doc_nanogui_TextBox_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_TextBox_mouseDragEvent = R"doc()doc";

static const char *__doc_nanogui_TextBox_mouseEnterEvent = R"doc()doc";

static const char *__doc_nanogui_TextBox_mouseMotionEvent = R"doc()doc";

static const char *__doc_nanogui_TextBox_pasteFromClipboard = R"doc()doc";

static const char *__doc_nanogui_TextBox_position2CursorIndex = R"doc()doc";

static const char *__doc_nanogui_TextBox_preferredSize = R"doc()doc";

static const char *__doc_nanogui_TextBox_setAlignment = R"doc()doc";

static const char *__doc_nanogui_TextBox_setCallback = R"doc()doc";

static const char *__doc_nanogui_TextBox_setDefaultValue = R"doc()doc";

static const char *__doc_nanogui_TextBox_setEditable = R"doc()doc";

static const char *__doc_nanogui_TextBox_setFormat = R"doc(Specify a regular expression specifying valid formats)doc";

static const char *__doc_nanogui_TextBox_setUnits = R"doc()doc";

static const char *__doc_nanogui_TextBox_setUnitsImage = R"doc()doc";

static const char *__doc_nanogui_TextBox_setValue = R"doc()doc";

static const char *__doc_nanogui_TextBox_units = R"doc()doc";

static const char *__doc_nanogui_TextBox_unitsImage = R"doc()doc";

static const char *__doc_nanogui_TextBox_updateCursor = R"doc()doc";

static const char *__doc_nanogui_TextBox_value = R"doc()doc";

static const char *__doc_nanogui_Theme = R"doc()doc";

static const char *__doc_nanogui_Theme_Theme = R"doc()doc";

static const char *__doc_nanogui_Theme_mBorderDark = R"doc()doc";

static const char *__doc_nanogui_Theme_mBorderLight = R"doc()doc";

static const char *__doc_nanogui_Theme_mBorderMedium = R"doc()doc";

static const char *__doc_nanogui_Theme_mButtonCornerRadius = R"doc()doc";

static const char *__doc_nanogui_Theme_mButtonFontSize = R"doc()doc";

static const char *__doc_nanogui_Theme_mButtonGradientBotFocused = R"doc()doc";

static const char *__doc_nanogui_Theme_mButtonGradientBotPushed = R"doc()doc";

static const char *__doc_nanogui_Theme_mButtonGradientBotUnfocused = R"doc()doc";

static const char *__doc_nanogui_Theme_mButtonGradientTopFocused = R"doc()doc";

static const char *__doc_nanogui_Theme_mButtonGradientTopPushed = R"doc()doc";

static const char *__doc_nanogui_Theme_mButtonGradientTopUnfocused = R"doc()doc";

static const char *__doc_nanogui_Theme_mDisabledTextColor = R"doc()doc";

static const char *__doc_nanogui_Theme_mDropShadow = R"doc()doc";

static const char *__doc_nanogui_Theme_mFontBold = R"doc()doc";

static const char *__doc_nanogui_Theme_mFontIcons = R"doc()doc";

static const char *__doc_nanogui_Theme_mFontNormal = R"doc()doc";

static const char *__doc_nanogui_Theme_mIconColor = R"doc()doc";

static const char *__doc_nanogui_Theme_mStandardFontSize = R"doc()doc";

static const char *__doc_nanogui_Theme_mTextColor = R"doc()doc";

static const char *__doc_nanogui_Theme_mTextColorShadow = R"doc()doc";

static const char *__doc_nanogui_Theme_mTransparent = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowCornerRadius = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowDropShadowSize = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowFillFocused = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowFillUnfocused = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowHeaderGradientBot = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowHeaderGradientTop = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowHeaderHeight = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowHeaderSepBot = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowHeaderSepTop = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowPopup = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowPopupTransparent = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowTitleFocused = R"doc()doc";

static const char *__doc_nanogui_Theme_mWindowTitleUnfocused = R"doc()doc";

static const char *__doc_nanogui_ToolButton = R"doc()doc";

static const char *__doc_nanogui_ToolButton_ToolButton = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_VScrollPanel = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_draw = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_mChildPreferredHeight = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_mScroll = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_mouseButtonEvent = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_mouseDragEvent = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_mouseMotionEvent = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_performLayout = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_preferredSize = R"doc()doc";

static const char *__doc_nanogui_VScrollPanel_scrollEvent = R"doc()doc";

static const char *__doc_nanogui_Widget = 
R"doc(Base class of all widgets

Widget is the base class of all widgets in ``nanogui``. It can also be used
as an panel to arrange an arbitrary number of child widgets using a layout
generator (see Layout).)doc";

static const char *__doc_nanogui_Widget_Widget = R"doc(Construct a new widget with the given parent widget)doc";

static const char *__doc_nanogui_Widget_absolutePosition = R"doc(Return the absolute position on screen)doc";

static const char *__doc_nanogui_Widget_addChild = 
R"doc(Add a child widget to the current widget

This function almost never needs to be called by hand, since the
constructor of Widget automatically adds the current widget to its parent)doc";

static const char *__doc_nanogui_Widget_childCount = R"doc(Return the number of child widgets)doc";

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
R"doc(Return current font size. If not set the default of the current theme will
be returned)doc";

static const char *__doc_nanogui_Widget_height = R"doc(Return the height of the widget)doc";

static const char *__doc_nanogui_Widget_id = R"doc(Return the ID value associated with this widget, if any)doc";

static const char *__doc_nanogui_Widget_keyboardCharacterEvent = R"doc(Handle text input (UTF-32 format) (default implementation: do nothing))doc";

static const char *__doc_nanogui_Widget_keyboardEvent = R"doc(Handle a keyboard event (default implementation: do nothing))doc";

static const char *__doc_nanogui_Widget_layout = R"doc(Return the used Layout generator)doc";

static const char *__doc_nanogui_Widget_layout_2 = R"doc(Return the used Layout generator)doc";

static const char *__doc_nanogui_Widget_mChildren = R"doc()doc";

static const char *__doc_nanogui_Widget_mCursor = R"doc()doc";

static const char *__doc_nanogui_Widget_mEnabled = R"doc()doc";

static const char *__doc_nanogui_Widget_mFixedSize = R"doc()doc";

static const char *__doc_nanogui_Widget_mFocused = R"doc()doc";

static const char *__doc_nanogui_Widget_mFontSize = R"doc()doc";

static const char *__doc_nanogui_Widget_mId = R"doc()doc";

static const char *__doc_nanogui_Widget_mLayout = R"doc()doc";

static const char *__doc_nanogui_Widget_mMouseFocus = R"doc()doc";

static const char *__doc_nanogui_Widget_mParent = R"doc()doc";

static const char *__doc_nanogui_Widget_mPos = R"doc()doc";

static const char *__doc_nanogui_Widget_mSize = R"doc()doc";

static const char *__doc_nanogui_Widget_mTheme = R"doc()doc";

static const char *__doc_nanogui_Widget_mTooltip = R"doc()doc";

static const char *__doc_nanogui_Widget_mVisible = R"doc()doc";

static const char *__doc_nanogui_Widget_mouseButtonEvent = R"doc(Handle a mouse button event (default implementation: propagate to children))doc";

static const char *__doc_nanogui_Widget_mouseDragEvent = R"doc(Handle a mouse drag event (default implementation: do nothing))doc";

static const char *__doc_nanogui_Widget_mouseEnterEvent = 
R"doc(Handle a mouse enter/leave event (default implementation: record this fact,
but do nothing))doc";

static const char *__doc_nanogui_Widget_mouseMotionEvent = R"doc(Handle a mouse motion event (default implementation: propagate to children))doc";

static const char *__doc_nanogui_Widget_parent = R"doc(Return the parent widget)doc";

static const char *__doc_nanogui_Widget_parent_2 = R"doc(Return the parent widget)doc";

static const char *__doc_nanogui_Widget_performLayout = 
R"doc(Invoke the associated layout generator to properly place child widgets, if
any)doc";

static const char *__doc_nanogui_Widget_position = R"doc(Return the position relative to the parent widget)doc";

static const char *__doc_nanogui_Widget_preferredSize = R"doc(Compute the preferred size of the widget)doc";

static const char *__doc_nanogui_Widget_removeChild = R"doc(Remove a child widget by index)doc";

static const char *__doc_nanogui_Widget_removeChild_2 = R"doc(Remove a child widget by value)doc";

static const char *__doc_nanogui_Widget_requestFocus = R"doc(Request the focus to be moved to this widget)doc";

static const char *__doc_nanogui_Widget_scrollEvent = R"doc(Handle a mouse scroll event (default implementation: propagate to children))doc";

static const char *__doc_nanogui_Widget_setCursor = R"doc(Set the cursor of the widget)doc";

static const char *__doc_nanogui_Widget_setEnabled = R"doc(Set whether or not this widget is currently enabled)doc";

static const char *__doc_nanogui_Widget_setFixedHeight = R"doc(Set the fixed height (see setFixedSize()))doc";

static const char *__doc_nanogui_Widget_setFixedSize = 
R"doc(Set the fixed size of this widget

If nonzero, components of the fixed size attribute override any values
computed by a layout generator associated with this widget. Note that just
setting the fixed size alone is not enough to actually change its size;
this is done with a call to setSize or a call to performLayout() in the
parent widget.)doc";

static const char *__doc_nanogui_Widget_setFixedWidth = R"doc(Set the fixed width (see setFixedSize()))doc";

static const char *__doc_nanogui_Widget_setFocused = R"doc(Set whether or not this widget is currently focused)doc";

static const char *__doc_nanogui_Widget_setFontSize = R"doc(Set the font size of this widget)doc";

static const char *__doc_nanogui_Widget_setHeight = R"doc(Set the height of the widget)doc";

static const char *__doc_nanogui_Widget_setId = R"doc(Associate this widget with an ID value (optional))doc";

static const char *__doc_nanogui_Widget_setLayout = R"doc(Set the used Layout generator)doc";

static const char *__doc_nanogui_Widget_setParent = R"doc(Set the parent widget)doc";

static const char *__doc_nanogui_Widget_setPosition = R"doc(Set the position relative to the parent widget)doc";

static const char *__doc_nanogui_Widget_setSize = R"doc(set the size of the widget)doc";

static const char *__doc_nanogui_Widget_setTheme = R"doc(Set the Theme used to draw this widget)doc";

static const char *__doc_nanogui_Widget_setTooltip = R"doc()doc";

static const char *__doc_nanogui_Widget_setVisible = 
R"doc(Set whether or not the widget is currently visible (assuming all parents
are visible))doc";

static const char *__doc_nanogui_Widget_setWidth = R"doc(Set the width of the widget)doc";

static const char *__doc_nanogui_Widget_size = R"doc(Return the size of the widget)doc";

static const char *__doc_nanogui_Widget_theme = R"doc(Return the Theme used to draw this widget)doc";

static const char *__doc_nanogui_Widget_theme_2 = R"doc(Return the Theme used to draw this widget)doc";

static const char *__doc_nanogui_Widget_tooltip = R"doc()doc";

static const char *__doc_nanogui_Widget_visible = 
R"doc(Return whether or not the widget is currently visible (assuming all parents
are visible))doc";

static const char *__doc_nanogui_Widget_visibleRecursive = 
R"doc(Check if this widget is currently visible, taking parent widgets into
account)doc";

static const char *__doc_nanogui_Widget_width = R"doc(Return the width of the widget)doc";

static const char *__doc_nanogui_Widget_window = R"doc()doc";

static const char *__doc_nanogui_Window = R"doc()doc";

static const char *__doc_nanogui_Window_Window = R"doc()doc";

static const char *__doc_nanogui_Window_center = R"doc(Center the window in the current Screen)doc";

static const char *__doc_nanogui_Window_dispose = R"doc(Dispose the window)doc";

static const char *__doc_nanogui_Window_draw = R"doc(Draw the window)doc";

static const char *__doc_nanogui_Window_mDrag = R"doc()doc";

static const char *__doc_nanogui_Window_mModal = R"doc()doc";

static const char *__doc_nanogui_Window_mTitle = R"doc()doc";

static const char *__doc_nanogui_Window_modal = R"doc(Is this a model dialog?)doc";

static const char *__doc_nanogui_Window_mouseButtonEvent = R"doc(Handle mouse events recursively and bring the current window to the top)doc";

static const char *__doc_nanogui_Window_mouseDragEvent = R"doc(Handle window drag events)doc";

static const char *__doc_nanogui_Window_preferredSize = R"doc(Compute the preferred size of the widget)doc";

static const char *__doc_nanogui_Window_refreshRelativePlacement = 
R"doc(Internal helper function to maintain nested window position values;
overridden in Popup)doc";

static const char *__doc_nanogui_Window_scrollEvent = 
R"doc(Accept scroll events and propagate them to the widget under the mouse
cursor)doc";

static const char *__doc_nanogui_Window_setModal = R"doc(Set whether or not this is a modal dialog)doc";

static const char *__doc_nanogui_Window_setTitle = R"doc(Set the window title)doc";

static const char *__doc_nanogui_Window_title = R"doc(Return the window title)doc";

static const char *__doc_nanogui___nanogui_get_image = R"doc(Helper function used by nvgImageIcon)doc";

static const char *__doc_nanogui_chdir_to_bundle_parent = 
R"doc(Move to the application bundle's parent directory

This is function is convenient when deploying .app bundles on OSX. It
adjusts the file path to the parent directory containing the bundle.)doc";

static const char *__doc_nanogui_detail_FormWidget = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_2 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_3 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_FormWidget = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_FormWidget_2 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_FormWidget_3 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_setCallback = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_setEditable = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_setEditable_2 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_setValue = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_setValue_2 = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_value = R"doc()doc";

static const char *__doc_nanogui_detail_FormWidget_value_2 = R"doc()doc";

static const char *__doc_nanogui_file_dialog = 
R"doc(Open a native file open/save dialog.

Parameter ``filetypes``:
    Pairs of permissible formats with descriptions like ``("png", "Portable
    Network Graphics")``)doc";

static const char *__doc_nanogui_frustum = R"doc()doc";

static const char *__doc_nanogui_init = 
R"doc(Static initialization; should be called once before invoking any NanoGUI
functions)doc";

static const char *__doc_nanogui_leave = R"doc(Request the application main loop to terminate)doc";

static const char *__doc_nanogui_loadImageDirectory = 
R"doc(Load a directory of PNG images and upload them to the GPU (suitable for use
with ImagePanel))doc";

static const char *__doc_nanogui_lookAt = R"doc()doc";

static const char *__doc_nanogui_mainloop = R"doc(Enter the application main loop)doc";

static const char *__doc_nanogui_nvgIsFontIcon = 
R"doc(Determine whether an icon ID is a font-based icon (e.g. from the entypo.ttf
font))doc";

static const char *__doc_nanogui_nvgIsImageIcon = R"doc(Determine whether an icon ID is a texture loaded via nvgImageIcon)doc";

static const char *__doc_nanogui_ortho = R"doc()doc";

static const char *__doc_nanogui_project = R"doc()doc";

static const char *__doc_nanogui_ref = 
R"doc(Reference counting helper

The *ref* refeference template is a simple wrapper to store a pointer to an
object. It takes care of increasing and decreasing the reference count of
the object. When the last reference goes out of scope, the associated
object will be deallocated.

\ingroup libcore)doc";

static const char *__doc_nanogui_ref_get = R"doc(Return a const pointer to the referenced object)doc";

static const char *__doc_nanogui_ref_get_2 = R"doc(Return a pointer to the referenced object)doc";

static const char *__doc_nanogui_ref_m_ptr = R"doc()doc";

static const char *__doc_nanogui_ref_operator_assign = R"doc(Move another reference into the current one)doc";

static const char *__doc_nanogui_ref_operator_assign_2 = R"doc(Overwrite this reference with another reference)doc";

static const char *__doc_nanogui_ref_operator_assign_3 = R"doc(Overwrite this reference with a pointer to another object)doc";

static const char *__doc_nanogui_ref_operator_eq = R"doc(Compare this reference with another reference)doc";

static const char *__doc_nanogui_ref_operator_eq_2 = R"doc(Compare this reference with a pointer)doc";

static const char *__doc_nanogui_ref_operator_mul = R"doc(Return a C++ reference to the referenced object)doc";

static const char *__doc_nanogui_ref_operator_mul_2 = R"doc(Return a const C++ reference to the referenced object)doc";

static const char *__doc_nanogui_ref_operator_ne = R"doc(Compare this reference with another reference)doc";

static const char *__doc_nanogui_ref_operator_ne_2 = R"doc(Compare this reference with a pointer)doc";

static const char *__doc_nanogui_ref_operator_sub_ = R"doc(Access the object referenced by this reference)doc";

static const char *__doc_nanogui_ref_operator_sub__2 = R"doc(Access the object referenced by this reference)doc";

static const char *__doc_nanogui_ref_ref_T_ = R"doc(Create a nullptr reference)doc";

static const char *__doc_nanogui_ref_ref_T__2 = R"doc(Construct a reference from a pointer)doc";

static const char *__doc_nanogui_ref_ref_T__3 = R"doc(Copy constructor)doc";

static const char *__doc_nanogui_ref_ref_T__4 = R"doc(Move constructor)doc";

static const char *__doc_nanogui_scale = R"doc()doc";

static const char *__doc_nanogui_shutdown = R"doc(Static shutdown; should be called before the application terminates)doc";

static const char *__doc_nanogui_translate = R"doc()doc";

static const char *__doc_nanogui_unproject = R"doc()doc";

static const char *__doc_nanogui_utf8 = 
R"doc(Convert a single UTF32 character code to UTF8

NanoGUI uses this to convert the icon character codes defined in entypo.h)doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

