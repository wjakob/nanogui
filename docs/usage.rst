Usage
========================================================================================

.. _nanogui_usage_cpp:

C++
----------------------------------------------------------------------------------------

There are effectively two ways that you can use NanoGUI in C++: have NanoGUI initialize
and manage the OpenGL context (and GLFW), or you do it manually.

1. If you are letting NanoGUI take over, you **must** call :func:`nanogui::init`
   before trying to do anything else.  If you are managing OpenGL / GLFW yourself, make
   sure you **avoid** calling this method.

2. Create an instance of :class:`Screen <nanogui::Screen>` (or a derived class you have
   written).

   - NanoGUI managed OpenGL: call the explicit constructor.
   - Self managed OpenGL: call the empty constructor.
       - You must call the :func:`Screen::initialize <nanogui::Screen::initialize>` method.

3. Add any Widgets, Buttons, etc. you want to the screen instance, and call the
   :func:`Screen::setVisible <nanogui::Screen::setVisible>` and
   :func:`Screen::performLayout <nanogui::Screen::performLayout>` methods of your
   :class:`Screen <nanogui::Screen>` instance.

4. Now that everything is ready, call :func:`nanogui::mainloop`.

5. When all windows are closed, this function will exit, and you should follow it up
   with a call to :func:`nanogui::shutdown`.

:NanoGUI Managed OpenGL / GLFW:
    Refer to :ref:`nanogui_example_2` for a concise example of what that all looks like.

:Self Managed OpenGL / GLFW:
    Refer to :ref:`nanogui_example_3` for an as concise as possible example of what you
    will need to do to get the :class:`Screen <nanogui::Screen>` to work.

.. _nanogui_usage_python:

Python
----------------------------------------------------------------------------------------

The Python interface is very similar to the C++ API.  When you build NanoGUI with CMake,
a ``python`` folder is created with the library you ``import nanogui`` from.  Though
there are implementation details that differ greatly, the documentation and build
process for the Python side is roughly the same.  Refer to the
:ref:`nanogui_example_programs` and compare the source code for the two.

:ref:`nanogui_example_3` highlights the more notable differences between the APIs.
Specifically, that managing GLFW from Python has no meaning, as well as the main loop
for Python can easily be detached.

.. _nanogui_usage_how_fonts_are_used:

How Fonts are Used
----------------------------------------------------------------------------------------

Font Faces
****************************************************************************************

Not every Widget draws text.  However, all Widgets have three *private* member variables
that have an associated *getter*, *setter*, and *default override*.  Each private member
is initialized as the empty string (``""``).

+-----------------------+---------------+------------------+-------------------------+
| Widget Private Member | Getter Method | Setter Method    | Default Override Method |
+=======================+===============+==================+=========================+
| |mFont|               | |font|        | |setFont|        | |defaultFont|           |
+-----------------------+---------------+------------------+-------------------------+
| |mTooltipFont|        | |tooltipFont| | |setTooltipFont| | |defaultTooltipFont|    |
+-----------------------+---------------+------------------+-------------------------+
| |mIconFont|           | |iconFont|    | |setIconFont|    | |defaultIconFont|       |
+-----------------------+---------------+------------------+-------------------------+

.. |mFont|              replace:: :member:`Widget::mFont <nanogui::Widget::mFont>`
.. |font|               replace:: :func:`Widget::font <nanogui::Widget::font>`
.. |setFont|            replace:: :func:`Widget::setFont <nanogui::Widget::setFont>`
.. |defaultFont|        replace:: :func:`Widget::defaultFont <nanogui::Widget::defaultFont>`
.. |mTooltipFont|       replace:: :member:`Widget::mTooltipFont <nanogui::Widget::mTooltipFont>`
.. |tooltipFont|        replace:: :func:`Widget::tooltipFont <nanogui::Widget::tooltipFont>`
.. |setTooltipFont|     replace:: :func:`Widget::setTooltipFont <nanogui::Widget::setTooltipFont>`
.. |defaultTooltipFont| replace:: :func:`Widget::defaultTooltipFont <nanogui::Widget::defaultTooltipFont>`
.. |mIconFont|          replace:: :member:`Widget::mIconFont <nanogui::Widget::mIconFont>`
.. |iconFont|           replace:: :func:`Widget::iconFont <nanogui::Widget::iconFont>`
.. |setIconFont|        replace:: :func:`Widget::setIconFont <nanogui::Widget::setIconFont>`
.. |defaultIconFont|    replace:: :func:`Widget::defaultIconFont <nanogui::Widget::defaultIconFont>`

.. note::

   The following documentation will explain ``mFont``, but the mechanics are the same
   for ``mTooltipFont`` and ``mIconFont``.

.. warning::

   Although ``mIconFont`` has a *default override* method available, its usage is
   generally discouraged.  Incorporating an alternative default icon font is better
   achieved via a custom theme (see
   :ref:`nanogui_usage_customization_default_icon_fonts`) rather than a class-level
   override.

The idea to keep in mind for font faces in NanoGUI is that it is a hierarchical scheme.
These variables are kept private in order to help enforce this hierarchy, as well as
prevent usage of |mFont| directly (since it more often than not will be the empty
string, which is not a valid font face).

**Instance Level**
    If a given widget's |mFont| is not the empty string, this will be used.  Calling
    |setFont|, or by construction (e.g., :func:`Label::Label <nanogui::Label::Label>`'s
    ``font`` parameter), are how to set a font for an individual widget.

    In drawing code, the getter will be used:

    .. code-block:: cpp

       nvgFontFace(ctx, font().c_str());// ctx is the NanoVG Context

    The implementation of :func:`Widget::font <nanogui::Widget::font>`:

    .. code-block:: cpp

       std::string Widger::font() const {
           if (!mFont.empty())
               return mFont;
           return defaultFont();// <- subclasses can override
       }

**Class Level**
    If a widget does not have a font face set explicitly, it's class-level default
    |defaultFont| is used.  For example, the :class:`Button <nanogui::Button>` class
    defaults to a bold font face.  The implementation:

    .. code-block:: cpp

       std::string Button::defaultFont() const {
           if (mTheme)
               return mTheme->mDefaultBoldFont;
           return Theme::GlobalDefaultFonts::Bold;
       }

    .. note::

       The "Class Level" is better thought of as a proxy of sorts to the "Theme Level",
       when designing a widget that has a preference to a different font face such as
       bold or monospace, do **not** return hard-coded values such as ``"sans-bold"``.

       Doing so will prevent users of your custom widget from overriding default fonts
       from their custom theme class.

**Theme Level**
    Every widget being drawn should have access to a :class:`Theme <nanogui::Theme>`
    instance via its :member:`Widget::mTheme <nanogui::Widget::mTheme>` reference.  In
    very rare circumstances a Widget may not have a theme reference.  These widgets will
    not be drawn (the only time a widget does not have a theme is when it also
    does not have a parent).  Using the static theme defaults is the preferred fallback,
    as those font face names will always be valid in NanoGUI.

    +----------------+------------------------+--------------------------------+
    | Font Style     | Theme Member Variable  | Static Theme Fallback Variable |
    +================+========================+================================+
    | Normal         | |mDefaultFont|         | |GlobalNormal|                 |
    +----------------+------------------------+--------------------------------+
    | Bold           | |mDefaultBoldFont|     | |GlobalBold|                   |
    +----------------+------------------------+--------------------------------+
    | Monospace      | |mDefaultMonoFont|     | |GlobalMono|                   |
    +----------------+------------------------+--------------------------------+
    | Monospace Bold | |mDefaultMonoBoldFont| | |GlobalMonoBold|               |
    +----------------+------------------------+--------------------------------+
    | Icons          | |mDefaultIconFont|     | |GlobalIcons|                  |
    +----------------+------------------------+--------------------------------+

    .. |mDefaultFont|         replace:: :member:`mTheme->mDefaultFont <nanogui::Theme::mDefaultFont>`
    .. |GlobalNormal|         replace:: :member:`Theme::GlobalDefaultFonts::Normal <nanogui::Theme::GlobalDefaultFonts::Normal>`
    .. |mDefaultBoldFont|     replace:: :member:`mTheme->mDefaultBoldFont <nanogui::Theme::mDefaultBoldFont>`
    .. |GlobalBold|           replace:: :member:`Theme::GlobalDefaultFonts::Bold <nanogui::Theme::GlobalDefaultFonts::Bold>`
    .. |mDefaultMonoFont|     replace:: :member:`mTheme->mDefaultMonoFont <nanogui::Theme::mDefaultMonoFont>`
    .. |GlobalMono|           replace:: :member:`Theme::GlobalDefaultFonts::Mono <nanogui::Theme::GlobalDefaultFonts::Mono>`
    .. |mDefaultMonoBoldFont| replace:: :member:`mTheme->mDefaultMonoBoldFont <nanogui::Theme::mDefaultMonoBoldFont>`
    .. |GlobalMonoBold|       replace:: :member:`Theme::GlobalDefaultFonts::MonoBold <nanogui::Theme::GlobalDefaultFonts::MonoBold>`
    .. |mDefaultIconFont|     replace:: :member:`mTheme->mDefaultIconFont <nanogui::Theme::mDefaultIconFont>`
    .. |GlobalIcons|          replace:: :member:`Theme::GlobalDefaultFonts::Icons <nanogui::Theme::GlobalDefaultFonts::Icons>`


.. tip::

   See :ref:`nanogui_usage_customizing_the_default_fonts` for all of the possible
   theme default fonts and what the represent.


Font Sizes
****************************************************************************************

Where font sizes are concerned, it depends on the widget.  Often times
:member:`Theme::mStandardFontSize <nanogui::Theme::mStandardFontSize>` will be used, but
there is also the corresponding
:func:`Widget::setFontSize <nanogui::Widget::setFontSize>` function to set it explicitly
for a specific widget.

.. _nanogui_usage_how_icons_are_used:

How Icons are Used
----------------------------------------------------------------------------------------

Icons are specified as integers, and can either be an image or a font icon.  Not every
widget uses / supports icons, but when it does the functions
:func:`nvgIsImageIcon <nanogui::nvgIsImageIcon>` and its counterpart
:func:`nvgIsFontIcon <nanogui::nvgIsFontIcon>` are used to determine how the icon will
be rendered.  There can be a maximum of ``1024`` image icons loaded, all other integer
values are assumed to be font icons.

The :class:`Button <nanogui::Button>` is an example of a class that supports icons,
either via the constructor or by :func:`Button::setIcon <nanogui::Button::setIcon>`.

.. _nanogui_usage_image_icons:

Image Icons
****************************************************************************************

To load an image icon, use the underlying NanoVG library (``#include <nanovg.h>``).  The
function you will likely want to use:

.. code-block:: cpp

   // Creates image by loading it from the disk from specified file name.
   // Returns handle to the image.
   extern NVG_EXPORT int nvgCreateImage(NVGcontext* ctx, const char* filename, int imageFlags);

You can obtain the ``NVGcontext *`` specific to your instance of
:class:`Screen <nanogui::Screen>` via either
:func:`Screen::nvgContext <nanogui::Screen::nvgContext>` or
:member:`Screen::mNVGContext <nanogui::Screen::mNVGContext>`.  Assuming the return value
is not ``-1``, you can then use the integer return value as the icon for a given widget.

.. tip::

   If all of the images you want to load are in the same directory, you can use the
   :func:`loadImageDirectory <nanogui::loadImageDirectory>` function as well.


.. _nanogui_usage_font_icons:

Font Icons
****************************************************************************************

The default icon font ``"icons"`` (see :ref:`file_nanogui_entypo.h`) defines numerous
icons available for you to use.  If you are embedding custom icon fonts (see
:ref:`nanogui_usage_customization_default_icon_fonts`) but are **not** setting this to
be the default icon font (via overriding
:func:`Theme::defaultIconFont <nanogui::Theme::defaultIconFont>`), make sure you call
:func:`Widget::setIconFont <nanogui::Widget::setIconFont>`.  Otherwise, the default
``"icons"`` font will be used, either producing a non-existent or undesired icon to
display.

.. tip::

   Using :class:`Label <nanogui::Label>`?  You want to specify the font, rather than
   the icon font.  You can create a label with a *font* icon by doing something like

   .. code-block:: cpp

      new Label(
         parent,
         // nanogui::utf8 accommodates for font icons being in the "private use area"
         std::string(nanogui::utf8(ENTYPO_ICON_CHECK).data()),
         // this sets Widget::mFont, not Widget::mIconFont
         "icons"
      );

.. _nanogui_usage_customization:

Customization
----------------------------------------------------------------------------------------

The following sections describe how to change the default colors and fonts with NanoGUI.
Where custom fonts are concerned, **fonts must be loaded for each NanoVG context
created**, and each :class:`Screen <nanogui::Screen>` has its on ``NVGcontext``.  Once a
font is loaded, though, it can be used by any child of this
:class:`Screen <nanogui::Screen>` instance.

.. danger::

   When overriding font members of the :class:`Theme <nanogui::Theme>` class, **you
   cannot bind new fonts to an existing name**.  For example,
   :member:`Theme::mDefaultFont <nanogui::Theme::mDefaultFont>` starts as ``"sans"``.
   It is **not possible** to bind a new font face to the name ``"sans"`` (this is a
   limitation of the underlying NanoVG library).  For convenience, the following is the
   list of font names that are already bound, and therefore cannot be re-bound to
   another font face:

   +-----------------+-----------------------------------------------------------------------+
   | Reserved Name   | :class:`Theme <nanogui::Theme>` Member that Reserved this Name        |
   +=================+=======================================================================+
   | ``"sans"``      | :member:`mDefaultFont <nanogui::Theme::mDefaultFont>`                 |
   +-----------------+-----------------------------------------------------------------------+
   | ``"sans-bold"`` | :member:`mDefaultBoldFont <nanogui::Theme::mDefaultBoldFont>`         |
   +-----------------+-----------------------------------------------------------------------+
   | ``"mono"``      | :member:`mDefaultMonoFont <nanogui::Theme::mDefaultMonoFont>`         |
   +-----------------+-----------------------------------------------------------------------+
   | ``"mono-bold"`` | :member:`mDefaultMonoBoldFont <nanogui::Theme::mDefaultMonoBoldFont>` |
   +-----------------+-----------------------------------------------------------------------+
   | ``"icons"``     | :member:`mDefaultIconFont <nanogui::Theme::mDefaultIconFont>`         |
   +-----------------+-----------------------------------------------------------------------+

   Note that **no errors will occur** when you try and bind a new font face to an
   existing name.  It simply won't occur.

.. tip::

   The `NanoGUI Theme Builder <https://github.com/svenevs/nanogui-customization-demo#theme-builder>`_
   can be a useful tool in prototyping color and size modifications to the
   :class:`Theme <nanogui::Theme>` class.


.. _nanogui_usage_customization_loading_custom_fonts:

Loading Custom Fonts
****************************************************************************************

.. tip::

   See the :ref:`nanogui_including_custom_fonts` section for how to get custom fonts
   embedded in NanoGUI.

To load a custom font, you call the ``nanogui::createFontMem`` method which is available
via ``#include <nanogui/resources.h>``.  This resources file is generated from CMake
(via ``bin2c``).

The parameters to the function:

1. The ``NVGcontext *`` associated with a :class:`Screen <nanogui::Screen>` instance.
2. The name you want to register the font as.
3. The "basename" of the font file that was embedded.  If you are unsure, search for
   ``__nanogui_font_map`` in the generated ``generated_resources/src/resources.cpp`` in
   your *build* directory (``find . -name resources.cpp``).  The basename recorded will
   be the string key in that map.

.. code-block:: cpp

   auto *screen = new nanogui::Screen(/* ... */);
   auto *window = new nanogui::Window(screen, "Window Title");

   int fancyFont = nanogui::createFontMem(screen->nvgContext(), "fancy", "fancy.ttf");
   // -1 signals error loading the font
   if (fancyFont == -1)
       throw std::runtime_error("Could not load 'fancy.ttf'!");

   new nanogui::Label(window, "Label Text", "fancy");

.. note::

   Using a derived type of :class:`Theme <nanogui::Theme>` (say ``CustomTheme``) and
   loading this custom theme in a derived type of :class:`Screen <nanogui::Screen>` is
   the easiest way to ensure that custom fonts get loaded for the underlying
   ``NVGcontext``.  However, this is not a requirement.

.. _nanogui_usage_customization_colors_icons:

Customizing the Theme Colors and Default Icons
****************************************************************************************

Customizing the default colors and / or icons of :class:`Theme <nanogui::Theme>` is
straightforward, simply derive the class and overwrite any values you desire in your
derived class constructor:

.. code-block:: cpp

   #include <nanogui/theme.h>
   #include <nanogui/entypo.h> // the default icons font

   class CustomTheme : public nanogui::Theme {
   public:
       CustomTheme(NVGcontext *ctx) : nanogui::Theme(ctx) {
           using nanogui::Color;
           // ... change any colors you want ...
           mBorderDark = Color(111, 255);
           // ... change any default icons you want ...
           // default: ``ENTYPO_ICON_CHECK``
           mCheckBoxIcon = ENTYPO_ICON_CROSS;
           mCheckBoxIconExtraScale = 1.3f;
       }
   };

All of the member variables in :class:`Theme <nanogui::Theme>` are ``public``, so you
can also modify them directly (obtain the theme from
:func:`Widget::theme <nanogui::Widget::theme>`).

Where default icons for widgets are concerned (the above example shows how to change
:class:`CheckBox <nanogui::CheckBox>`), there is an associated floating point scale
factor for the icon.  This is because the *codepoint* of the icons in the default
Entypo+ icon font are not all perfectly compatible.  When changing the default icons,
you will likely want to also adjust the default icon scaling.

.. note::

   At this time, there are three widgets that have custom icon scaling:

   1. :class:`CheckBox <nanogui::CheckBox>` (via :member:`Theme::mCheckBoxIconExtraScale <nanogui::Theme::mCheckBoxIconExtraScale>`).
   2. :class:`PopupButton <nanogui::PopupButton>` (via :member:`Theme::mPopupIconExtraScale <nanogui::Theme::mPopupIconExtraScale>`).
   3. :class:`TextBox <nanogui::TextBox>` (via :member:`Theme::mTextBoxIconExtraScale <nanogui::Theme::mTextBoxIconExtraScale>`).

.. _nanogui_usage_customizing_the_default_fonts:

Customizing the Default Fonts
****************************************************************************************

.. tip::

   See the :ref:`nanogui_including_custom_fonts` section for how to get custom fonts
   embedded in NanoGUI.

Assuming you want to use a custom font face, you need to load the font yourself.  When
NanoGUI builds, it uses ``bin2c`` to generate ``nanogui/resources.h`` and the associated
implementation file.  Assuming you requested ``customfont.ttf`` and
``customfont-bold.ttf`` via ``NANOGUI_EXTRA_RESOURCES``, then you want to

1. Override the :class:`Theme <nanogui::Theme>` string members related to the font faces
   you want to change the defaults for.

2. Load the fonts using ``nanogui::createFontMem`` (see
   :ref:`nanogui_usage_customization_loading_custom_fonts` for more information on the
   parameters to this method).

.. code-block:: cpp

   #include <nanogui/theme.h>
   #include <nanogui/resources.h> // provides nanogui::createFontMem

   class CustomTheme : public nanogui::Theme {
   public:
       CustomTheme(NVGcontext *ctx) : nanogui::Theme(ctx) {
           // Step 1: override the nanogui::Theme font members you seek to change
           mDefaultFont     = "custom";
           mDefaultBoldFont = "custom-bold";
           // Step 2: load the custom fonts.
           mCustomFont     = nanogui::createFontMem(ctx, mDefaultFont.c_str(), "customfont.ttf");
           mCustomFontBold = nanogui::createFontMem(ctx, mDefaultBoldFont.c_str(), "customfont-bold.ttf");
           // -1 means error loading font
           if (mCustomFont == -1 || mCustomFontBold == -1)
               throw std::runtime_error("Could not load customfont!");
       }

   protected:
       int mCustomFont = -1;
       int mCustomFontBold = -1;
   };

.. note::

   Changing these sets the default fonts **globally**.  To change the font face for one
   specific widget, call :func:`Widget::setFont <nanogui::Widget::setFont>`.

.. _nanogui_usage_customization_default_icon_fonts:

Customizing the Default Icon Font
****************************************************************************************

.. tip::

   See the :ref:`nanogui_including_custom_icon_fonts` section for how to get custom icon
   fonts embedded in NanoGUI.

.. warning::

   The default icon font ``"icons"`` (see :ref:`file_nanogui_entypo.h`) has all
   characters defined in the
   `private use area range <http://jrgraphix.net/r/Unicode/E000-F8FF>`_.  This is not
   a hard requirement, but the values must be **greater** than ``1024`` in order for
   :func:`nvgIsImageIcon <nanogui::nvgIsImageIcon>` and
   :func:`nvgIsFontIcon <nanogui::nvgIsFontIcon>` to behave appropriately.

   See :ref:`nanogui_usage_how_icons_are_used` for more information.

The process for custom icon fonts is nearly the same: override a
:class:`Theme <nanogui::Theme>` member, and load the font.  However, now that you have
changed the icon font, you must override **all** theme variables related to actual icon
codes (since you are using a different font now).  Assuming you embedded
``customicons.ttf`` with ``NANOGUI_EXTRA_ICON_RESOURCES`` (meaning there was a
corresponding ``customicons.h`` C++ header file defining the newly available icons):

.. code-block:: cpp

   #include <nanogui/theme.h>
   #include <nanogui/resources.h>   // provides nanogui::createFontMem
   #include <nanogui/customicons.h> // copied to nanogui/customicons.h for you

   class CustomTheme : public nanogui::Theme {
   public:
       CustomTheme(NVGcontext *ctx) : nanogui::Theme(ctx) {
           // Step 1: override the nanogui::Theme font member related to icons
           mDefaultIconFont = "customicons";
           // Step 2: load the customicons font
           mCustomIconsFont = nanogui::createFontMem(ctx, mDefaultIconFont.c_str(), "customicons.ttf");
           // -1 means error loading font
           if (mCustomIconsFont == -1)
               throw std::runtime_error("Could not load customicons font!");

           // Step 3: overwrite *ALL* icon variables
           // mCheckBoxIcon = CUSTOMICONS_ICON_SOMETHING;
           // mCheckBoxIconExtraScale = ???;
       };

   protected:
       int mCustomIconsFont = -1;
   };

.. note::

   Changing this sets the default fonts **globally**.  To change the icon font face for
   one specific widget, call :func:`Widget::setIconFont <nanogui::Widget::setIconFont>`.

.. _nanogui_usage_using_custom_themes:

Using Custom Themes
****************************************************************************************

Now that we have some derived ``CustomTheme`` class, we want to use it.  First, let us
understand how :class:`Theme <nanogui::Theme>` is used.

.. code-block:: cpp

   auto *screen = new nanogui::Screen(/* ... */);
   auto *window = new nanogui::Window(window, "Window Title");
   window->setLayout(new nanogui::GroupLayout());
   new nanogui::Label(window, "label text");

When :class:`Screen <nanogui::Screen>` is initialized
(:func:`Screen::initialize <nanogui::Screen::initialize>`), the
:class:`Theme <nanogui::Theme>` is created.  Every :class:`Widget <nanogui::Widget>`
(for which :class:`Screen <nanogui::Screen>` is a derived type of) contains a reference
to a :class:`Themme <nanogui::Theme>` instance in :member:`nanogui::Widget::mTheme`.  So
when ``window`` and the label are created above, the same
:class:`Theme <nanogui::Theme>` instance now has three separate widgets that refer to it.

To apply a custom theme globally:

.. code-block:: cpp

   auto *screen = new nanogui::Screen(/* ... */);
   nanogui::ref<CustomTheme> theme = new CustomTheme(screen->nvgContext());
   screen->setTheme(theme);
   auto *window = new nanogui::Window(window, "Window Title");
   // add remaining widgets

Since the ``Theme`` is always inherited from the parent, all newly created children of
``screen`` will contain a reference to the ``CustomTheme`` instance.

.. note::

   Nothing requires that you set the theme globally on a ``Screen`` instance.  You can
   apply the theme to one specific :class:`Window <nanogui::Window>`, for example.

.. note::

   When :func:`Widget::setTheme <nanogui::Widget::setTheme>` is called, the call is
   propagated to all children.  So you can just as easily create all of the widgets
   first, and call ``setTheme`` on the desired parent.

.. tip::

   See :ref:`nanogui_usage_customization_loading_custom_fonts` for more information on
   the ``nanogui::createFontMem`` method.
