cmake_minimum_required (VERSION 2.8.3)

project("NanoGUI")

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/src")
  message(FATAL_ERROR "The NanoGUI dependency repositories (GLFW, etc.) are missing! "
    "You probably did not clone the project with --recursive. It is possible to recover "
    "by calling \"git submodule update --init --recursive\"")
endif()

option(NANOGUI_BUILD_EXAMPLE "Build NanoGUI example application?" ON)
option(NANOGUI_BUILD_SHARED  "Build NanoGUI as a shared library?" ON)
option(NANOGUI_BUILD_PYTHON  "Build a Python plugin for NanoGUI?" ON)
set(NANOGUI_PYTHON_VERSION "" CACHE STRING "Python version to use for compiling the Python plugin")

# Required libraries for linking against nanogui (all targets)
set(NANOGUI_EXTRA_LIBS "")

# Platform-dependent files for libnanogui
set(LIBNANOGUI_EXTRA_SOURCE "")

if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

if(APPLE AND NANOGUI_BUILD_SHARED)
  set(CMAKE_MACOSX_RPATH ON)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL " " FORCE)
set(BUILD_SHARED_LIBS ${NANOGUI_BUILD_SHARED} CACHE BOOL " " FORCE)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # Quench annoying deprecation warnings when compiling GLFW on OSX
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
endif()

# Compile GLFW
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw" "ext_build/glfw")

# Python support: add NANOGUI_PYTHON flag to all targets
if (NANOGUI_BUILD_PYTHON)
  add_definitions ("-DNANOGUI_PYTHON")
endif()

# Shared library mode: add NANOGUI_SHARED flag to all targets
if (NANOGUI_BUILD_SHARED)
  add_definitions ("-DNANOGUI_SHARED")
endif()

if (MSVC)
  # Disable annoying MSVC warnings (all targets)
  add_definitions(/D "_CRT_SECURE_NO_WARNINGS")

  # Parallel build on MSVC (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")

    # Disable Eigen vectorization for Windows 32 bit builds (issues with unaligned access segfaults)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DDEIGEN_DONT_VECTORIZE /DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT")
  endif()
endif()

# Compile with compiler warnings turned on
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

if (UNIX)
  # Enable C++11 mode on Linux/OSX (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(WIN32)
  # Build and include GLEW on Windows
  list(APPEND LIBNANOGUI_EXTRA_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/ext/glew/src/glew.c")
  set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/ext/glew/src/glew.c" PROPERTIES COMPILE_DEFINITIONS GLEW_BUILD)
  include_directories(ext/glew/include)
endif()

# Required core libraries on various platforms
if (WIN32)
  list(APPEND NANOGUI_EXTRA_LIBS opengl32)
elseif (APPLE)
  find_library(cocoa_library Cocoa)
  find_library(opengl_library OpenGL)
  find_library(corevideo_library CoreVideo)
  find_library(iokit_library IOKit)
  list(APPEND NANOGUI_EXTRA_LIBS ${cocoa_library} ${opengl_library} ${corevideo_library} ${iokit_library})
  list(APPEND LIBNANOGUI_EXTRA_SOURCE src/darwin.mm)
elseif("${CMAKE_SYSTEM}" MATCHES "Linux")
  list(APPEND NANOGUI_EXTRA_LIBS GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 pthread dl rt)
endif()

include_directories(ext/eigen ext/glfw/include ext/nanovg/src include ${CMAKE_CURRENT_BINARY_DIR})

# Run simple C converter to put font files into the data segment
add_executable(bin2c resources/bin2c.c)
set(bin2c_cmdline resources.cpp resources.h)
file(GLOB resources "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.ttf")
foreach(file ${resources})
  list(APPEND bin2c_cmdline ${file})
endforeach()

# Run bin2c on resource files
add_custom_command(
  OUTPUT resources.cpp resources.h
  COMMAND bin2c ARGS ${bin2c_cmdline}
  DEPENDS bin2c ${resources}
  COMMENT "Running bin2c"
  PRE_BUILD VERBATIM)

# Needed to generated files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Set library type
if(NANOGUI_BUILD_SHARED)
  set(NANOGUI_LIBRARY_TYPE "SHARED")
else()
  set(NANOGUI_LIBRARY_TYPE "STATIC")
endif()

# Compile main NanoGUI library
add_library(nanogui ${NANOGUI_LIBRARY_TYPE}
  # Merge GLFW into the NanoGUI library
  $<TARGET_OBJECTS:glfw_objects>
  # Merge NanoVG into the NanoGUI library
  ext/nanovg/src/nanovg.c
  # Merge GLEW into the NanoGUI library (only if needed)
  ${LIBNANOGUI_EXTRA_SOURCE}
  # Fonts etc.
  resources.cpp
  include/nanogui/glutil.h src/glutil.cpp
  include/nanogui/common.h src/common.cpp
  include/nanogui/widget.h src/widget.cpp
  include/nanogui/theme.h src/theme.cpp
  include/nanogui/layout.h src/layout.cpp
  include/nanogui/screen.h src/screen.cpp
  include/nanogui/label.h src/label.cpp
  include/nanogui/window.h src/window.cpp
  include/nanogui/popup.h src/popup.cpp
  include/nanogui/checkbox.h src/checkbox.cpp
  include/nanogui/button.h src/button.cpp
  include/nanogui/popupbutton.h src/popupbutton.cpp
  include/nanogui/combobox.h src/combobox.cpp
  include/nanogui/progressbar.h src/progressbar.cpp
  include/nanogui/slider.h src/slider.cpp
  include/nanogui/messagedialog.h src/messagedialog.cpp
  include/nanogui/textbox.h src/textbox.cpp
  include/nanogui/imagepanel.h src/imagepanel.cpp
  include/nanogui/imageview.h src/imageview.cpp
  include/nanogui/vscrollpanel.h src/vscrollpanel.cpp
  include/nanogui/colorwheel.h src/colorwheel.cpp
  include/nanogui/colorpicker.h src/colorpicker.cpp
  include/nanogui/graph.h src/graph.cpp
  include/nanogui/formhelper.h
  include/nanogui/toolbutton.h
  include/nanogui/opengl.h
  include/nanogui/nanogui.h
)

# Compile/link flags for NanoGUI
set_property(TARGET nanogui APPEND PROPERTY COMPILE_DEFINITIONS "NANOGUI_BUILD ")

if (NANOGUI_BUILD_SHARED)
  target_link_libraries(nanogui ${NANOGUI_EXTRA_LIBS})
endif()

if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG AND NANOGUI_BUILD_SHARED)
  # Link-time code generation (only for shared library)
  if (MSVC)
    set_property(TARGET nanogui APPEND_STRING PROPERTY LINK_FLAGS "/LTCG ")
    set_property(TARGET nanogui APPEND_STRING PROPERTY COMPILE_FLAGS "/GL ")
  elseif(UNIX)
    set_property(TARGET nanogui APPEND_STRING PROPERTY COMPILE_FLAGS "-fvisibility=hidden ")
    set_property(TARGET nanogui APPEND_STRING PROPERTY COMPILE_FLAGS "-flto ")
  endif()
endif()

if (NANOGUI_BUILD_SHARED)
  # When GLFW is merged into the NanoGUI library, this flag must be specified
  set_property(TARGET nanogui APPEND PROPERTY COMPILE_DEFINITIONS "_GLFW_BUILD_DLL ")
endif()

if (NANOGUI_BUILD_SHARED AND NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
  if (APPLE)
    # Strip .dylib library on OSX
    add_custom_command(TARGET nanogui POST_BUILD COMMAND strip -u -r ${CMAKE_CURRENT_BINARY_DIR}/libnanogui.dylib)
  elseif(UNIX)
    # Strip .so library on Linux
    add_custom_command(TARGET nanogui POST_BUILD COMMAND strip ${CMAKE_CURRENT_BINARY_DIR}/libnanogui.so)
  endif()
endif()

# Quench warnings while compiling NanoVG
if (CMAKE_COMPILER_IS_GNUCC)
  set_source_files_properties(ext/nanovg/src/nanovg.c PROPERTIES COMPILE_FLAGS -Wno-unused-result)
elseif(MSVC)
  set_source_files_properties(ext/nanovg/src/nanovg.c PROPERTIES COMPILE_FLAGS "/wd4005 /wd4456 /wd4457")
endif()

if (NANOGUI_BUILD_SHARED)
  set_source_files_properties(ext/nanovg/src/nanovg.c PROPERTIES COMPILE_DEFINITIONS "NVG_BUILD;NVG_SHARED")
else()
  set_source_files_properties(ext/nanovg/src/nanovg.c PROPERTIES COMPILE_DEFINITIONS "NVG_BUILD")
endif()


# Build example application if desired
if(NANOGUI_BUILD_EXAMPLE)
  add_executable(example1 src/example1.cpp)
  add_executable(example2 src/example2.cpp)
  target_link_libraries(example1 nanogui ${NANOGUI_EXTRA_LIBS})
  target_link_libraries(example2 nanogui ${NANOGUI_EXTRA_LIBS})

  # Copy icons for example application
  file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

if (NANOGUI_BUILD_PYTHON)
  # Detect Python
  set(Python_ADDITIONAL_VERSIONS 3.4 3.5 3.6)
  find_package(PythonLibs ${NANOGUI_PYTHON_VERSION})
  if (NOT PYTHONLIBS_FOUND)
    # Python not found -- disable the plugin
    set(NANOGUI_BUILD_PYTHON OFF CACHE BOOL "Build a Python plugin for NanoGUI?" FORCE)
  endif()
endif()

if (NANOGUI_BUILD_PYTHON)
  # Need PIC code in libnanogui even when compiled as a static library
  set_target_properties(nanogui PROPERTIES POSITION_INDEPENDENT_CODE ON)
  set_target_properties(glfw_objects PROPERTIES POSITION_INDEPENDENT_CODE ON)

  include_directories("ext/pybind11/include" ${PYTHON_INCLUDE_DIR})
  add_library(nanogui_python SHARED python/python.cpp python/python.h python/py_doc.h)
  set_target_properties(nanogui_python PROPERTIES OUTPUT_NAME "nanogui")
  target_link_libraries(nanogui_python nanogui ${NANOGUI_EXTRA_LIBS})

  # Quench GCC-related warnings
  if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(python/python.cpp PROPERTIES COMPILE_FLAGS -Wno-unused-variable)
  endif()

  set_target_properties(nanogui_python PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python)
  set_target_properties(nanogui_python PROPERTIES PREFIX "")

  if (WIN32)
    # .PYD file extension on Windows
    set_target_properties(nanogui_python PROPERTIES SUFFIX ".pyd")

    # Set output path
    set_target_properties(nanogui_python PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "Release/python")
    set_target_properties(nanogui_python PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "Debug/python")
    set_target_properties(nanogui_python PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "MinSizeRel/python")
    set_target_properties(nanogui_python PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "RelWithDebInfo/python")
    set_target_properties(nanogui_python PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "Release/python")
    set_target_properties(nanogui_python PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "Debug/python")
    set_target_properties(nanogui_python PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "MinSizeRel/python")
    set_target_properties(nanogui_python PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "RelWithDebInfo/python")

    # Link against the Python shared library
    target_link_libraries(nanogui_python ${PYTHON_LIBRARY})

    if (MSVC)
      # Optimize size, /bigobj is needed for due to the heavy template metaprogramming in pybind11
      set_target_properties(nanogui_python PROPERTIES COMPILE_FLAGS "/Os /bigobj")
    endif()
  elseif(UNIX)
    # .SO file extension on Linux/Mac OS
    set_target_properties(nanogui_python PROPERTIES SUFFIX ".so")

    # Optimize for a small binary size
    if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
      set_target_properties(nanogui_python PROPERTIES COMPILE_FLAGS "-Os")
    endif()

    # Strip unnecessary sections of the binary on Linux/Mac OS
    if(APPLE)
      set_target_properties(nanogui_python PROPERTIES MACOSX_RPATH ".")
      set_target_properties(nanogui_python PROPERTIES LINK_FLAGS "-undefined dynamic_lookup -dead_strip")

      if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
        add_custom_command(TARGET nanogui_python POST_BUILD COMMAND strip -u -r ${CMAKE_CURRENT_BINARY_DIR}/python/nanogui.so)
      endif()
    else()
      if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
        add_custom_command(TARGET nanogui_python POST_BUILD COMMAND strip ${CMAKE_CURRENT_BINARY_DIR}/python/nanogui.so)
      endif()
    endif()
  endif()
endif()

# Create documentation for python plugin (optional target for developers)
add_custom_target(mkdoc COMMAND
  python3 ${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11/tools/mkdoc.py
  -Iinclude -Iext/nanovg/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include/nanogui/*.h > ${CMAKE_CURRENT_SOURCE_DIR}/python/py_doc.h)

get_directory_property(NANOGUI_HAS_PARENT PARENT_DIRECTORY)
if(NANOGUI_HAS_PARENT)
  # This project is included from somewhere else. Export NANOGUI_EXTRA_LIBS variable
  set(NANOGUI_EXTRA_LIBS ${NANOGUI_EXTRA_LIBS} PARENT_SCOPE)
endif()

# vim: set et ts=2 sw=2 ft=cmake nospell:
