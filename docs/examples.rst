.. _nanogui_example_programs:

Examples
========================================================================================

There are example programs available for you to play with / understand how the different
pieces fit together.  The C++ examples are in ``nanogui/src/``, and the equivalent
Python examples are in ``nanogui/python``.

.. _nanogui_example_1:

Example 1
----------------------------------------------------------------------------------------

.. image:: ../resources/screenshot.png
   :alt: Screenshot of Example 1.
   :align: center

The first example program is rather long, but not altogether complicated.  It
is effectively an enumeration of how one would go about adding various different kinds
of Widgets to the window being displayed.

- `Example 1 in C++ <https://github.com/wjakob/nanogui/blob/master/src/example1.cpp>`_
- `Example 1 in Python <https://github.com/wjakob/nanogui/blob/master/python/example1.py>`_

.. _nanogui_example_2:

Example 2
----------------------------------------------------------------------------------------

.. image:: ../resources/screenshot2.png
   :alt: Screenshot of Example 2.
   :align: center

The second program demonstrates how simple label/editor widget-type layouts can
be written in a very concise manner.

- `Example 2 in C++ <https://github.com/wjakob/nanogui/blob/master/src/example2.cpp>`_
- `Example 2 in Python <https://github.com/wjakob/nanogui/blob/master/python/example2.py>`_

.. _nanogui_example_3:

Example 3
----------------------------------------------------------------------------------------

The third example program demonstrates how to manage OpenGL / GLFW on your own for the
C++ side, and how to detach the NanoGUI ``mainloop()`` on the Python side.

- `Example 3 in C++ <https://github.com/wjakob/nanogui/blob/master/src/example3.cpp>`_
- `Example 3 in Python <https://github.com/wjakob/nanogui/blob/master/python/example3.py>`_

.. _nanogui_example_4:

Example 4
----------------------------------------------------------------------------------------

The fourth example program demonstrates the GLCanvas widget, which renders an
arbitrary sequence of OpenGL commands into a NanoGUI widget.

- `Example 4 in C++ <https://github.com/wjakob/nanogui/blob/master/src/example4.cpp>`_
- `Example 4 in Python <https://github.com/wjakob/nanogui/blob/master/python/example4.py>`_

.. _nanogui_example_icons:

Example Icons
----------------------------------------------------------------------------------------

NanoGUI includes various icons made available from :ref:`file_nanogui_entypo.h`,
courtesy of `Daniel Bruce's Entypo <http://www.entypo.com/>`_ glyphs.  The file level
documentation is useful as a reference for selecting an icon, but the web rendering may
be misleading --- NanoGUI uses a dark background for widgets by default.

Run the ``exampleIcon`` executable to see what the icons look like in NanoGUI.  The
setup of this file may also be helpful in understanding how to control the
:class:`nanogui::VScrollPanel`.

- `Example Icons in C++ <https://github.com/wjakob/nanogui/blob/master/src/example_icons.cpp>`_
- `Example Icons in Python <https://github.com/wjakob/nanogui/blob/master/python/example_icons.py>`_


.. _nanogui_example_repository:

Example Repository
----------------------------------------------------------------------------------------

Darren Mothersele has put together a compact and informative example repository that
demonstrates how easy it is to include NanoGUI into your project.  You download / browse
the source on `GitHub <https://github.com/darrenmothersele/nanogui-test.git>`_.
