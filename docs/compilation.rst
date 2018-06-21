Compilation
========================================================================================

NanoGUI uses a CMake build system to ensure portability. All dependencies are
cloned and compiled in one batch, which should generally reduce the amount of
configuration effort to zero. Assuming that NanoGUI was cloned into the current
working directory, the following commands need to be executed:

.. code-block:: bash

   # enter the top-level NanoGUI directory
   $ cd nanogui

   # make a build directory and enter that
   $ mkdir build
   $ cd build

   # generate your Makefile
   $ cmake ..

   # now that you have a Makefile, use that to build
   $ make -j 4

For Windows, the process is nearly the same:

.. code-block:: bash

   # enter the top-level NanoGUI directory
   $ cd nanogui

   # make a build directory and enter that
   $ mkdir build
   $ cd build

   # Specify VS Version AND 64bit, otherwise it defaults to 32.
   # The version number and year may be different for you, Win64
   # can be appended to any of them.  Execute `cmake -G` to get
   # a listing of the available generators.
   #
   # 32 bit Windows builds are /not/ supported
   $ cmake -G "Visual Studio 14 2015 Win64" ..

   # Either open the .sln with Visual Studio, or run
   $ cmake --build . --config Release

.. _compilation_configuring_a_git_submodule:

Configuring a Git Submodule
----------------------------------------------------------------------------------------

This section outlines how to get a `git submodule <submodule>`_ setup if you do not know
how.  Once you have a submodule setup, see the :ref:`compilation_default_configurations`
section for information on building NanoGUI from a parent CMake project.

.. code-block:: none

   some_repo/
       CMakeLists.txt  # <- The parent project build system.
       ext/
           nanogui/    # <- The nanogui git submodule.

So assuming you are in the root directory ``some_repo``:

.. code-block:: console

   $ mkdir ext
   $ cd ext
   $ git submodule add https://github.com/wjakob/nanogui.git

What this does is inform ``git`` that there is a sub-project that needs to be cloned as
well, it will create a file ``.gitmodules`` representing this fact.  Since NanoGUI also
has submodules, when we set this up **the first time**, these have **not** been cloned.

.. code-block:: console

   $ git submodule update --init --recursive

Once you run that command, all of the submodules of NanoGUI will now be downloaded.  In
the future, put instructions in your README informing users to clone your repository
recursively:

.. code-block:: console

   $ git clone --recursive https://github.com/username/repository.git

.. tip::

   You can inform users that forgot the ``--recursive`` from your CMake build system
   how to recover.  The ``ext/nanogui`` directory on a non-recursive clone will be an
   empty directory.  So including something like the following should be sufficient to
   inform your users how to recover:

   .. code-block:: cmake

      if (NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui/include")
        message(FATAL_ERROR "The NanoGUI dependency is missing!  You probably did not "
                            "clone the project with --recursive. It is possible to "
                            "recover by calling\n    "
                            "git submodule update --init --recursive")
      endif()

.. _submodule: https://git-scm.com/book/en/v2/Git-Tools-Submodules

.. _compilation_default_configurations:

Default Configurations
----------------------------------------------------------------------------------------

By default, NanoGUI will

+---------------------------------+---------------------------+
| Impact / effect                 | CMake Option              |
+=================================+===========================+
| Build the example programs.     | ``NANOGUI_BUILD_EXAMPLE`` |
+---------------------------------+---------------------------+
| Build as a *shared* library.    | ``NANOGUI_BUILD_SHARED``  |
+---------------------------------+---------------------------+
| Build the Python plugins.       | ``NANOGUI_BUILD_PYTHON``  |
+---------------------------------+---------------------------+
| Use GLAD if on Windows.         | ``NANOGUI_USE_GLAD``      |
+---------------------------------+---------------------------+
| Generate an ``install`` target. | ``NANOGUI_INSTALL``       |
+---------------------------------+---------------------------+

Users developing projects that reference NanoGUI as a ``git submodule`` (this
is **strongly** encouraged, see the :ref:`compilation_configuring_a_git_submodule`
section) can set up the parent project's CMake configuration
file as follows (this assumes that ``nanogui`` lives in the directory
``ext/nanogui`` relative to the parent project):

.. code-block:: cmake

    # Disable building extras we won't need (pure C++ project)
    set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
    set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
    set(NANOGUI_INSTALL       OFF CACHE BOOL " " FORCE)

    # Add the configurations from nanogui
    add_subdirectory(ext/nanogui)

    # For reliability of parallel build, make the NanoGUI targets dependencies
    set_property(TARGET glfw_objects nanogui PROPERTY FOLDER "dependencies")

Required Variables Exposed
----------------------------------------------------------------------------------------

Due to the nature of building an OpenGL application for different platforms, three
variables are populated to allow for easy incorporation with your CMake build.  After
you have executed ``add_subdirectory`` as shown above, you will need to add the
following (assuming the target you are building is called ``myTarget``):

.. code-block:: cmake

   # Various preprocessor definitions have been generated by NanoGUI
   add_definitions(${NANOGUI_EXTRA_DEFS})

   # On top of adding the path to nanogui/include, you may need extras
   include_directories(${NANOGUI_EXTRA_INCS})

   # Compile a target using NanoGUI
   add_executable(myTarget myTarget.cpp)

   # Lastly, additional libraries may have been built for you.  In addition to linking
   # against NanoGUI, we need to link against those as well.
   target_link_libraries(myTarget nanogui ${NANOGUI_EXTRA_LIBS})

Advanced Compilation Details
----------------------------------------------------------------------------------------

NanoGUI and Python
****************************************************************************************

Although it is |year|, you may still for example wish to build the Python bindings for
Python 2.7.  The variable you would set **before** ``add_subdirectory`` is
``NANOGUI_PYTHON_VERSION``.  For example,

.. code-block:: cmake

   set(NANOGUI_PYTHON_VERSION "2.7")
   # can also use minor versions
   set(NANOGUI_PYTHON_VERSION "3.6.2")


NanoGUI and Eigen
****************************************************************************************

NanoGUI uses Eigen_ internally for various vector types.  Eigen is an advanced header
only template library, which NanoGUI vendors in the ``ext`` folder.  It is important to
understand the implication of Eigen being header only: **only one version of Eigen can
be included**.

There is a CMake bypass variable available in NanoGUI: ``NANOGUI_EIGEN_INCLUDE_DIR``.
You would set this variable **before** ``add_subdirectory``.  Since you will want to
provide the same kind of bypass for users of your library, the following snippet is a
good starting point.  For this example code:

1. The parent CMake project is called ``myproj``.  A good CMake practice to adopt is to
   prefix your project's name to any variables you intend to expose.  This allows parent
   projects to know where the variable came from, and avoids name collisions.
2. First ``find_package`` is used to try and find Eigen.  The philosophy is that the
   user is responsible for ensuring that the version of Eigen they want to use will be
   found.
3. Since NanoGUI needs to remain self-contained, the side-effect is that even if the
   user does *not* have Eigen installed, you can fallback and use the one vendored with
   NanoGUI.
4. The following directory structure:

   .. code-block:: none

      myproj/
          CMakeLists.txt         <- Where this example code is
          ext/
              nanogui/
                  CMakeLists.txt <- NanoGUI's build system
                  ext/
                      eigen/     <- NanoGUI's internal copy of Eigen


.. code-block:: cmake

   # `if NOT` is what enables the same bypass for your project
   if(NOT MYPROJ_EIGEN3_INCLUDE_DIR)
     # Grab or find the Eigen3 include directory.
     find_package(Eigen3 QUIET)
     if(EIGEN3_INCLUDE_DIR)
       set(MYPROJ_EIGEN3_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR})
     else()
       # use the internal NanoGUI copy of Eigen
       set(MYPROJ_EIGEN3_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui/ext/eigen)
     endif()
   endif()

   message(STATUS "Using Eigen3 from directory: ${MYPROJ_EIGEN3_INCLUDE_DIR}")
   set(NANOGUI_EIGEN_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR} CACHE BOOL " " FORCE)
   # set any other NanoGUI specific variables you need (shown in above sections)
   add_subdirectory(ext/nanogui)

   # include it for your project as well (or append to a list
   # and include that list later, depending on your setup)
   include_directories(${MYPROJ_EIGEN3_INCLUDE_DIR})

.. _Eigen: https://eigen.tuxfamily.org/dox/

NanoGUI, GLFW, and Other Projects
****************************************************************************************

Suppose you want to use NanoGUI as your GUI toolkit, but you also have another library
you want to use that depends on ``glfw``.  Call the second library Foo.  Generally
speaking, it is unlikely that library Foo will provide you with mechanisms to explicitly
specify where ``glfw`` comes from.  You could try to work on a patch with the developers
of library Foo to allow this to be overridden, but you may need to maintain your own
fork of library Foo.  There is just as much justification to allow the bypass as there
is to not want it in a build system.

Since NanoGUI merges the ``glfw`` objects into the library being built, you can actually
just specify ``nanogui`` as the ``glfw`` dependency directly.  So lets suppose that
library Foo was looking for ``glfw`` like this:

.. code-block:: cmake

   find_package(GLFW3)
   if(GLFW3_FOUND)
     include_directories(${GLFW3_INCLUDE_DIRS})
     target_link_libraries(foo ${GLFW3_LIBRARIES})
   endif()

You can cheat around this pretty easily.  For the modification to library Foo's build
system, all we do is wrap ``find_package``:

.. code-block:: diff

   + if(NOT GLFW3_FOUND)
       find_package(GLFW3)
   + endif()
     if(GLFW3_FOUND)
       include_directories(${GLFW3_INCLUDE_DIRS})
       target_link_libraries(foo ${GLFW3_LIBRARIES})
     endif()

Now that ``find_package`` will only execute if ``NOT GLFW3_FOUND``, in your build system
you make sure to set all three ``glfw`` variables (found, include, and libraries).  It
might look something like this:

.. code-block:: cmake

   # ... any other nanogui configs ...
   # same directory structure as Eigen example
   add_subdirectory(ext/nanogui)

   # nanogui needs to be added first so the 'nanogui' target is defined
   # and can be used in the generator expression for the libraries
   set(GLFW3_FOUND ON)
   set(GLFW3_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui/ext/glfw/include)
   set(GLFW3_LIBRARIES $<TARGET_FILE:nanogui>)

   add_subdirectory(ext/foo)

   # IMPORTANT! You need to force NanoGUI to build first
   # Assuming their library target is called 'foo'
   add_dependencies(foo nanogui)


Depending on what you need to do, the above may not be sufficient.  But it is at least
a starting point to being able to "share" NanoGUI as the vendor of ``glfw``.

.. _nanogui_including_custom_fonts:

Including Custom Fonts
****************************************************************************************

NanoGUI uses the Roboto_ font for text.  The font faces available are

1. ``Roboto-Regular.ttf``: loaded as ``"sans"``.
2. ``Roboto-Bold.ttf``: loaded as ``"sans-bold"``.
3. ``RobotoMono-Regular.ttf``: loaded as ``"mono"``.
4. ``RobotoMono-Bold.ttf``: loaded as ``"mono-bold"``.

If you wish to add your own custom font, all you need is a True Type file (a ``.ttf``
extension).  Suppose you had the following directory structure:

.. code-block:: none

   myproj/
       ext/
           nanogui/       <- The submodule.
       CMakeLists.txt     <- Add to this file.
       resources/
           customfont.ttf <- The custom font to embed.

Simply append the path to ``NANOGUI_EXTRA_RESOURCES`` **before** doing
``add_subdirectory(ext/nanogui)``:

.. code-block:: cmake

   list(APPEND NANOGUI_EXTRA_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/customfont.ttf")

NanoGUI will automatically embed the ``customfont.ttf`` file, keeping in mind that
``${CMAKE_CURRENT_SOURCE_DIR}`` is **required** to guarantee that CMake will expand
the *correct* path to the font.

.. tip::

   See :ref:`nanogui_usage_customization_loading_custom_fonts` for more information on
   how to load extra fonts.

.. _Roboto: https://fonts.google.com/specimen/Roboto

.. _nanogui_including_custom_icon_fonts:

Including Custom Icon Fonts
****************************************************************************************

NanoGUI uses the Entypo_ font for icons, which is loaded as ``"icons"``.  If you want to
be able to use an alternative icon font, the process is similar to loading custom fonts
for text but with additional requirements.  Suppose you wanted to load a custom icon
font called ``customicons.ttf``.  Then the following directory structure is assumed:

.. code-block:: none

   myproj/
       ext/
           nanogui/                      <- The submodule
       CMakeLists.txt                    <- Add to this file.
       resources/
           customicons/
               customicons.ttf           <- The custom icon font to embed.
               customicons.h             <- The C++ header defining the constants.
               constants_customicons.cpp <- The python bindings code.

1. The naming must be **exact**: if the font was ``SomeFont.ttf``, then the nanogui
   build system searches instead for ``SomeFont.h`` and ``constants_SomeFont.cpp``.
2. These files must all be in **the same directory**.

What the three files represent:

- ``customicons.ttf``: the icon font that will be embedded.
- ``customicons.h``: the C++ header file that enumerates the ``#define`` directives.
  This file is what enables you to actually use the icons in code.
- ``constants_customicons.cpp``: the python bindings for the font.  Note that this file
  is only required if ``NANOGUI_BUILD_PYTHON`` is ``ON``.

Simply specify the path to the custom icons font with ``NANOGUI_EXTRA_ICON_RESOURCES``:

.. code-block:: cmake

   list(
     APPEND
     NANOGUI_EXTRA_ICON_RESOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/resources/customicons/customicons.ttf"
   )

Keep in mind that ``${CMAKE_CURRENT_SOURCE_DIR}`` is **required** to guarantee that
CMake will expand the *correct* path to the icon font.

.. note::

   It is assumed that somewhere in your project **after**
   ``add_subdirectory(ext/nanogui)`` you are also doing
   ``include_directories(${NANOGUI_EXTRA_INCS})``.  In the example above, the file
   ``customicons.h`` will automatically be copied to a location included in
   ``NANOGUI_EXTRA_INCS`` such that in your own code, you write
   ``#include <nanogui/customicons.h>`` noting the extra ``nanogui/``.  In other words,
   this ``customicons.h`` becomes a part of the distribution (e.g., if you were to
   install NanoGUI).  As such, you should **not** need to do something like
   ``include_directories("${CMAKE_CURRENT_SOURCE_DIR}/resources/customicons")``.

.. warning::

   See the :ref:`nanogui_usage_customization_default_icon_fonts` section for
   requirements on the numeric values in custom icon fonts.

.. tip::

   Have the ``.svg`` icons for a custom icon font?  Use the scripts available in the
   `nanogui-custom-font-generator <https://github.com/svenevs/nanogui-custom-font-generator>`_
   repository to generate the requisite three files.

.. _Entypo: http://www.entypo.com

.. _nanogui_compiling_the_docs:

Compiling the Documentation
----------------------------------------------------------------------------------------

The documentation system relies on 'Doxygen', 'Sphinx', 'Breathe', and
'Exhale'.  It uses the 'Read the Docs' theme for the layout of the generated
html.  Documenting C++ with Sphinx is evolving rapidly, as such we encourage you
to build the documentation using `Virtualenv <https://virtualenv.pypa.io/en/stable/>`_.
This way if the NanoGUI documentation needs different versions of something
(e.g., Sphinx), you will not need to change your system installation.

1. Install Doxygen for your operating system.  On Unix based systems, this
   should be available through your package manager (apt-get, brew, dnf, etc).

2. Install ``virtualenv``:

   .. code-block:: bash

      $ pip3 install virtualenv

Now that you have the relevant tools, you can build the documentation with

.. code-block:: bash

   # Enter the documentation directory
   $ cd <path/to/nanogui>/docs

   # Create the virtual environment.  Note that 'venv' is exactly
   # the name excluded in `conf.py`, using a different name will
   # result in *hundreds* of extra build warnings!
   $ virtualenv venv

   # Activate the virtual environment (Windows: python venv/bin/activate_this.py)
   $ source venv/bin/activate

   # Install the documentation requirements for NanoGUI
   (venv) $ pip install -r requirements.txt

   # Build the documentation
   (venv) $ make html

   # When you are done, leave the virtual environment with `deactivate`

The output will be generated in ``_build``, the root html document is located
at ``_build/html/index.html``.

.. note::

   When building the documentation locally, there can be subtle differences in
   the rendered pages than what is hosted online.  You should largely be able
   to ignore this.
