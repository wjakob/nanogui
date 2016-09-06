Usage
========================================================================================

C++
----------------------------------------------------------------------------------------

There are effectively two ways that you can use NanoGUI in C++: have NanoGUI initialize
and manage the OpenGL context (and GLFW), or you do it manually.

1. If you are letting NanoGUI take over, you **must** call :ref:`function_nanogui__init`
   before trying to do anything else.  If you are managing OpenGL / GLFW yourself, make
   sure you **avoid** calling this method.
2. Create an instance of :ref:`class_nanogui__Screen` (or a derivative class you have
   written).

   - NanoGUI managed OpenGL: call the explicit constructor.
   - Self managed OpenGL: call the empty constructor.
       - You must call the :func:`nanogui::Screen::initialize` method.

3. Add any Widgets, Buttons, etc. you want to the screen instance, and call the
   :func:`nanogui::Screen::setVisible` and :func:`nanogui::Screen::performLayout`
   methods of your instance.
4. Now that everything is ready, call :ref:`function_nanogui__mainloop`.
5. When all windows are closed, this function will exit, and you should follow it up
   with a call to :ref:`function_nanogui__shutdown`.

:NanoGUI Managed OpenGL / GLFW:
    Refer to :ref:`nanogui_example_2` for a concise example of what that all looks like.

:Self Managed OpenGL / GLFW:
    Refer to :ref:`nanogui_example_3` for an as concise as possible example of what you
    will need to do to get the :ref:`class_nanogui__Screen` to work.


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
