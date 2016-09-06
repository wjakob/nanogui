.. _developer_contribution:

Contributing
========================================================================================

Thank you for your interest in this project! Please refer to the following sections on
how to contribute code and bug reports.

Reporting bugs
----------------------------------------------------------------------------------------

At the moment, this project is run in the spare time of a single person
(`Wenzel Jakob <http://rgl.epfl.ch/people/wjakob>`_) with very limited resources for
issue tracker tickets. Thus, before submitting a question or bug report, please take a
moment of your time and ensure that your issue isn't already discussed in the project
documentation elsewhere on this site.

Feature requests are generally closed unless they come with a pull requests
that implement the desired functionality.

Assuming that you have identified a previously unknown problem or an important question,
it's essential that you submit a self-contained and minimal piece of code that
reproduces the problem. In other words: no external dependencies, isolate the
function(s) that cause breakage, submit matched and complete C++ or Python snippets
(depending on how you are using NanoGUI) that can be easily compiled and run on my end.

Pull requests
----------------------------------------------------------------------------------------
Contributions are submitted, reviewed, and accepted using Github pull requests. Please
refer to `this article <https://help.github.com/articles/using-pull-requests>`_ for
details and adhere to the following rules to make the process as smooth as possible:

- Make a new branch for every feature you're working on.
- Make small and clean pull requests that are easy to review but make sure they do add
  value by themselves.
- Make sure you have tested any new functionality (e.g. if you made a new Widget).
- This project has a strong focus on providing general solutions using a minimal amount
  of code, thus small pull requests are greatly preferred.
- Read the remainder of this document, adhering to the bindings and documentation
  requirements.
- If making a purely documentation PR, please prefix the commit with ``[docs]``

  - E.g. ``[docs] Adding documentation for class X.``

Python Binding Requirements
----------------------------------------------------------------------------------------

Since NanoGUI builds for both C++, as well as produces Python bindings, you **must**
account for both sides of the API regardless of how you use the project.  If you are
adding a new method, class, etc (not fixing an existing one), you must write the code
to produce the relevant binding in ``python/python.cpp``.

Code Style Requirements
----------------------------------------------------------------------------------------

- Tabs are 4 spaces -- please do not submit PRs with *tab* characters.
- Most code follows an 80 column ruler, wherever reasonable.
- Pointers and references have modifiers next to variable name, not the type:

  - **Yes**: ``void *p``, **No**: ``void* p``
  - **Yes**: ``Color &c``, **No**: ``Color& c``

- Template classes / functions: ``template <typename T> method()``

  - Space between template and ``<``, on same line where reasonable

- Opening curly braces for definitions / loops / ifs are on the same line as the statement

  - **Yes**:

    .. code-block:: cpp

       for(auto &&c : myVec) {
           // ... computation ...
       }
  - **No**:

    .. code-block:: cpp

       for(auto &&c : myVec)
       {
           // ... computation ...
       }

Code Documentation Requirements
----------------------------------------------------------------------------------------

When adding new classes, methods, etc please provide meaningful and well formatted
documentation of the new functionality.  We use Doxygen comments to document the code,
using the "JavaDoc" style.  For consistency, please do not use the QT or other formats.

If you are familiar with how to use Doxygen-style comments:

- You should indent by four spaces for things like ``param``, etc.
- ``\brief``: a brief description.
- ``\tparam``: a template parameter.
- ``\param``: a parameter.
- ``\return``: what the return value represents (where applicable).

For a quick crash-course on documenting using Doxygen:

1. If you are adding a new file, please include the disclaimer at the top **immediately**
   followed by ``/** \file */``.  So if you are creating the file ``nanogui/file.h``

   .. code-block:: cpp

      /*
       nanogui/file.h -- A brief description of what the file contains.

       NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
       The widget drawing code is based on the NanoVG demo application
       by Mikko Mononen.

       All rights reserved. Use of this source code is governed by a
       BSD-style license that can be found in the LICENSE.txt file.
      */
      /** \file */

   changing the first line to the right name / description of your file.

2. Documenting a newly added Struct or Class requires special attention.  If you are
   adding a class ``Thing`` in file ``nanogui/thing.h``, the class level documentation
   needs to explicitly declare the location for Doxygen to parse everything correctly.

   .. code-block:: cpp

      /**
       * \class Thing thing.h nanogui/thing.h
       *
       * This is the actual documentation for the thing.
       */
      class Thing { ... };

   This simply tells Doxygen how to format the various ``include`` directives.  If you
   are writing a Struct, replace ``\class`` with ``\struct``.

3. Please fully document all parameters, template parameters, and return types where
   applicable.  In some cases it is sufficient to include just a brief one-line
   documentation string, e.g. the for the :func:`nanogui::Screen::caption` method, it
   is simple enough that the following is sufficient (note the **three** ``///``):

   .. code-block:: cpp

      /// Get the window title bar caption
      const std::string &caption() const { return mCaption; }

   However, more complicated methods should be thoroughly documented.  As an example,
   this method demonstrates template parameters, parameters, and return value documentation:

   .. code-block:: cpp

      /**
       * \brief A useless function for getting sizes.
       *
       * This method has specific things that must be pointed out, but they
       * were too long to include in the 'brief' documentation.
       *
       * \tparam T
       *     The type we are evaluating the size of.
       *
       * \param returnFake
       *     If set to true, a random positive number will be returned. This
       *     comment is a bit longer and can span multiple lines, making sure
       *     to indent each new line.
       *
       *     Warning: this had an empty line before it and will NOT appear in
       *     the documentation of this parameter, but instead it will appear
       *     in the documentation of the method!
       *
       * \return
       *     The result of ``sizeof(T)``.
       */
      template <typename T>
      size_t exampleTemplateFunction(bool returnFake = false) { ... }

Styling the Code
----------------------------------------------------------------------------------------

Since we are using both Doxygen and Sphinx, we have access to a wealth of interesting
documentation styling.

:From Doxygen:
    You can use things like ``\throws``, ``\remark``, and even ``\ref`` to generate html
    links to other items.

:From Sphinx:
    On the Sphinx side, you now have access to full reStructuredText syntax.  This
    includes:

    - ``**bold**`` to make **bold** text
    - ``*italics*`` for *italics*
    - ````teletype```` for ``teletype`` text.

    You can additionally include more complex reStructuredText such as grid tables, as
    well as Sphinx directives.  You will need to use the ``\rst`` and ``\endrst``
    commands for these:

    .. code-block:: cpp

       /**
        * \brief Some method you are documenting.
        *
        * \rst
        * I am now in a verbatim reStructuredText environment, and can create a grid table.
        *
        * I could create a python code listing using
        *
        * .. code-block:: py
        *
        *    print("Some python code.")
        *
        * You can also use the note or warning directives to highlight important concepts:
        *
        * .. note::
        *    You may or may not segfault.
        *
        * .. warning::
        *    I guarantee you will segfault.
        * \endrst
        */

    .. warning::
       In normal reStructuredText, if you simply indent a block of code by four spaces
       it will render as a code listing.  While this will build as expected for the
       C++ documentation on RTD, it will **fail** to build ``py_doc.h`` correctly.

       For code listings, **always** begin an ``\rst`` section and use ``.. code-block``
       as shown above.

TODO
----------------------------------------------------------------------------------------

Documentation Completion
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Already familiar with NanoGUI or a subset of its classes?  The documentation for the
following files is incomplete, waiting for your PR.  Document a whole class, or even
just a method of a given class.

If you make progress on / complete an item with your PR, please update / remove it from
the table on this page (``docs/contributing.rst``).

.. note::
   The NanoGUI documentation hosted online does not include ``private`` methods or
   member variables at this time.  However, documentation for these is welcome!

.. warning::
   In some of these files, you will see preprocessor blocks like

   .. code-block:: cpp

      #ifndef DOXYGEN_SHOULD_SKIP_THIS
      ... code that the breaks the documentation ...
      #endif // DOXYGEN_SHOULD_SKIP_THIS

   Please take care not to remove these!

+-----------------+------------------------------------------------------------+
| Filename        | Action Item                                                |
+=================+============================================================+
| button.h        | - Most member methods.                                     |
|                 | - All member variables.                                    |
+-----------------+------------------------------------------------------------+
| checkbox.h      | - All member methods and variables.                        |
+-----------------+------------------------------------------------------------+
| colorpicker.h   | - Constructor and callback.                                |
|                 | - All member variables.                                    |
+-----------------+------------------------------------------------------------+
| colorwheel.h    | - Most methods and member variables.                       |
+-----------------+------------------------------------------------------------+
| combobox.h      | - Most member methods and variables.                       |
+-----------------+------------------------------------------------------------+
| formhelper.h    | - More detailed documentation explaining parameters        |
|                 |   for ``FormHelper`` methods.                              |
|                 | - Most member variables.                                   |
+-----------------+------------------------------------------------------------+
| graph.h         | - All member methods and variables.                        |
+-----------------+------------------------------------------------------------+
| imagepanel.h    | - All member methods and variables.                        |
+-----------------+------------------------------------------------------------+
| imageview.h     | - Most member methods.                                     |
+-----------------+------------------------------------------------------------+
| label.h         | - Some member methods and variables.                       |
+-----------------+------------------------------------------------------------+
| layout.h        | - Nearly everything.                                       |
+-----------------+------------------------------------------------------------+
| popup.h         | - Some member methods and variables.                       |
|                 | - Explicit parameter documentation would be very useful.   |
+-----------------+------------------------------------------------------------+
| popupbutton.h   | - Almost everything.                                       |
+-----------------+------------------------------------------------------------+
| progressbar.h   | - Almost everything.                                       |
+-----------------+------------------------------------------------------------+
| screen.h        | - Documentation for the manual GLFW API.                   |
|                 | - All member variables.                                    |
+-----------------+------------------------------------------------------------+
| slider.h        | - Almost everything.                                       |
+-----------------+------------------------------------------------------------+
| stackedwidget.h | - Almost everything.                                       |
+-----------------+------------------------------------------------------------+
| tabheader.h     | - Some member methods.                                     |
|                 | - Some reformatting of existing documentation to           |
|                 |   use ``\param`` or ``\return`` etc.                       |
+-----------------+------------------------------------------------------------+
| tabwidget.h     | - Some member methods.                                     |
|                 | - Some reformatting of existing documentation to           |
|                 |   use ``\param`` or ``\return`` etc.                       |
+-----------------+------------------------------------------------------------+
| textbox.h       | - Almost everything.                                       |
+-----------------+------------------------------------------------------------+
| theme.h         | - Explicit documentation for what these all represent.     |
+-----------------+------------------------------------------------------------+
| toolbutton.h    | - Documentation of the constructor explaining the range of |
|                 |   values that can be used for ``icon``.  Or at least where |
|                 |   to look for that?                                        |
+-----------------+------------------------------------------------------------+
| vscrollpanel.h  | - Almost everything.                                       |
+-----------------+------------------------------------------------------------+
| widget.h        | - Member variables.                                        |
+-----------------+------------------------------------------------------------+
| window.h        | - Some member methods.                                     |
|                 | - All member variables.                                    |
+-----------------+------------------------------------------------------------+


Advanced Contribution Opportunity
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Currently, all partial and full template specializations are skipped.  Specifically,
nearly everything in ``include/nanogui/serializer/*``.  According to the
`Breathe documentation <https://breathe.readthedocs.io/en/latest/doxygen.html#template>`_
this should be possible.  The likely cause of this issue is that the version of Breathe
packaged for use with ``pip`` is not up to date.  Your task would be to find a way
to use ``docs/requirements.txt`` to install the **current source** from the master
branch of Breathe instead of using PyPi.

You can test locally by making sure you do not have Breathe installed with ``pip``, and
compiling it yourself (make sure you add it to your ``PATH`` so you can use it in
Python).

Then try moving the ``#ifndef DOXYGEN_SHOULD_SKIP_THIS`` to expose a single template
specialization in a file of your choice, and try and get the documentation to build.
If you succeed with this, the next step will be to find a way to get Read the Docs to
build the current source of Breathe rather than using PyPi.

In theory, all of these are possible.
