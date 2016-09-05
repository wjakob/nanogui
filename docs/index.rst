NanoGUI
========================================================================================

NanoGUI is a minimalistic cross-platform widget library for OpenGL 3.x or higher. It
supports automatic layout generation, stateful C++11 lambdas callbacks, a variety of
useful widget types and Retina-capable rendering on Apple devices thanks to NanoVG_ by
Mikko Mononen. Python bindings of all functionality are provided using pybind11_.

.. _NanoVG: https://github.com/memononen/NanoVG
.. _pybind11: https://github.com/wjakob/pybind11

Example Screenshot
----------------------------------------------------------------------------------------

.. image:: ../resources/screenshot.png
   :alt: Screenshot of Example 1.
   :align: center

Description
----------------------------------------------------------------------------------------

NanoGUI builds on GLFW_ for cross-platform OpenGL context creation and event handling,
GLAD_ to use OpenGL 3.x or higher Windows, Eigen_ for basic vector types, and NanoVG_ to
draw 2D primitives.

Note that the dependency library NanoVG already includes some basic example code to draw
good-looking static widgets; what NanoGUI does is to flesh it out into a complete GUI
toolkit with event handling, layout generation, etc.

NanoGUI currently works on Mac OS X (Clang) Linux (GCC or Clang) and Windows (Visual
Studio ≥ 2015); it requires a recent C++11 capable compiler. All dependencies are
jointly built using a CMake-based build system.

.. _GLFW: http://www.glfw.org/
.. _GLAD: https://github.com/Dav1dde/glad
.. _Eigen: http://eigen.tuxfamily.org/index.php?title=Main_Page

"Simple mode"
----------------------------------------------------------------------------------------

Christian Schüller contributed a convenience class that makes it possible to create
AntTweakBar-style variable manipulators using just a few lines of code. Refer to
:ref:`nanogui_example_2` for how to create the image below.

.. image:: ../resources/screenshot2.png
   :alt: Screenshot of Example 2.
   :align: center

License
----------------------------------------------------------------------------------------

NanoGUI is provided under a BSD-style license that can be found in the LICENSE_ file. By
using, distributing, or contributing to this project, you agree to the terms and
conditions of this license.

.. _LICENSE: https://github.com/wjakob/nanogui/blob/master/LICENSE.txt

Contents
========================================================================================

.. toctree::
   :maxdepth: 2

   usage
   compilation
   examples
   generated_api/library_root
   contributing


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
