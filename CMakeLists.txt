cmake_minimum_required (VERSION 2.8.3)

project("NanoGUI")

option(NANOGUI_BUILD_EXAMPLE "Build NanoGUI example application?" ON)
option(NANOGUI_BUILD_SHARED  "Build NanoGUI as a shared library?" ON)
option(NANOGUI_BUILD_PYTHON  "Build a Python plugin for NanoGUI?" ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

option(GLFW_BUILD_EXAMPLES " " OFF)
option(GLFW_BUILD_TESTS " " OFF)
option(GLFW_BUILD_DOCS " " OFF)
option(GLFW_INSTALL " " OFF)
if (NANOGUI_BUILD_SHARED)
  # Build shared library version of GLFW
  option(BUILD_SHARED_LIBS " " ON)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND APPLE)
  # Quench annoying deprecation warnings when compiling GLFW on OSX
  set(CMAKE_C_FLAGS "-Wno-deprecated-declarations")
endif()
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw" "ext_build/glfw")

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)
if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -flto")
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

# Run simple C converter to put font files into the data segment
add_executable(bin2c resources/bin2c.c)
set(bin2c_cmdline resources.cpp resources.h)
file(GLOB resources "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.ttf")
foreach(file ${resources})
  list(APPEND bin2c_cmdline ${file})
endforeach()

add_custom_command(
  OUTPUT resources.cpp resources.h
  COMMAND bin2c ARGS ${bin2c_cmdline}
  DEPENDS bin2c ${resources}
  COMMENT "Running bin2c"
  PRE_BUILD VERBATIM)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (NANOGUI_BUILD_SHARED)
  add_definitions ("-DNANOGUI_SHARED")
endif()

set(extra_source "")

if(WIN32)
  list(APPEND extra_source "${CMAKE_CURRENT_SOURCE_DIR}/ext/glew/src/glew.c")
  include_directories(ext/glew/include)
  add_definitions (/D "_CRT_SECURE_NO_WARNINGS")
  set(extra_libs opengl32)
elseif (APPLE)
  # Find standard libraries needed for OpenGL/GLFW
  find_library(cocoa_library Cocoa)
  find_library(opengl_library OpenGL)
  find_library(corevideo_library CoreVideo)
  find_library(iokit_library IOKit)
  set(extra_libs ${cocoa_library} ${opengl_library} ${corevideo_library} ${iokit_library})
  list(APPEND extra_source src/darwin.mm)
elseif("${CMAKE_SYSTEM}" MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(extra_libs GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 pthread dl)
endif()

include_directories(ext/eigen ext/glfw/include ext/nanovg/src include ${CMAKE_CURRENT_BINARY_DIR})

if(NANOGUI_BUILD_SHARED)
  set(NANOGUI_LIBRARY_TYPE "SHARED")
else()
  set(NANOGUI_LIBRARY_TYPE "STATIC")
endif()

if(APPLE AND NANOGUI_BUILD_SHARED)
  set(CMAKE_MACOSX_RPATH ON)
endif()

add_library(nanogui ${NANOGUI_LIBRARY_TYPE}
  ext/nanovg/src/nanovg.c resources.cpp
  include/nanogui/glutil.h src/glutil.cpp
  include/nanogui/nanogui.h src/nanogui.cpp
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
  include/nanogui/graph.h src/graph.cpp
  include/nanogui/toolbutton.h
  include/nanogui/opengl.h
  include/nanogui/common.h
  ${extra_source}
)

if (CMAKE_COMPILER_IS_GNUCC)
  set_source_files_properties(ext/nanovg/src/nanovg.c PROPERTIES COMPILE_FLAGS -Wno-unused-result)
endif()

if(APPLE AND NANOGUI_BUILD_SHARED)
  target_link_libraries(nanogui glfw ${extra_libs})
endif()

if(NANOGUI_BUILD_SHARED)
  set_target_properties(nanogui PROPERTIES COMPILE_DEFINITIONS "NANOGUI_BUILD_SHARED")
endif()

if (WIN32 AND MSVC)
  # Enforce link time code generation on MSVC, /MP enables multithreaded builds
  set_target_properties(nanogui PROPERTIES COMPILE_FLAGS "/GL /MP")
  set_target_properties(nanogui PROPERTIES LINK_FLAGS "/LTCG")
endif()

if(NANOGUI_BUILD_EXAMPLE)
  add_executable(example src/example.cpp)
  target_link_libraries(example nanogui glfw ${extra_libs})
endif()

file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (NANOGUI_BUILD_PYTHON)
  set(Python_ADDITIONAL_VERSIONS 3.4 3.5 3.6)
  find_package(PythonLibs REQUIRED)
  include_directories("ext/pybind11/include" ${PYTHON_INCLUDE_DIR})
  add_library(nanogui_python SHARED src/python.cpp)
  set_target_properties(nanogui_python PROPERTIES OUTPUT_NAME "nanogui")
  target_link_libraries(nanogui_python nanogui glfw ${extra_libs})
  if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(src/python.cpp PROPERTIES COMPILE_FLAGS -Wno-unused-variable)
  endif()

  set_target_properties(nanogui_python PROPERTIES PREFIX "")
  if (WIN32)
    if (MSVC)
      # Enforce size-based optimization and link time code generation on MSVC (~30% smaller binaries in experiments)
      # /bigobj is needed for bigger binding projects due to the limit to 64k addressable sections
      # /MP enables multithreaded builds
      set_target_properties(nanogui_python PROPERTIES COMPILE_FLAGS "/Os /GL /MP /bigobj")
      set_target_properties(nanogui_python PROPERTIES LINK_FLAGS "/LTCG")
    endif()
    # .PYD file extension on Windows
    set_target_properties(nanogui_python PROPERTIES SUFFIX ".pyd")

    # Link against the Python shared library
    target_link_libraries(nanogui_python ${PYTHON_LIBRARY})
  elseif(UNIX)
    # .SO file extension on Linux/Mac OS
    set_target_properties(nanogui_python PROPERTIES SUFFIX ".so")

    # Strip unnecessary sections of the binary on Linux/Mac OS
    if(APPLE)
      set_target_properties(nanogui_python PROPERTIES MACOSX_RPATH ".")
      set_target_properties(nanogui_python PROPERTIES LINK_FLAGS "-undefined dynamic_lookup -dead_strip")
      if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
        add_custom_command(TARGET nanogui_python POST_BUILD COMMAND strip -u -r ${PROJECT_BINARY_DIR}/nanogui.so)
      endif()
    else()
      if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
        add_custom_command(TARGET nanogui_python POST_BUILD COMMAND strip ${PROJECT_BINARY_DIR}/nanogui.so)
      endif()
    endif()
  endif()
endif()

# Create documentation for python plugin (optional target for developers)
add_custom_target(mkdoc COMMAND
  python3 ${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11/tools/mkdoc.py
  -Iinclude -Iext/nanovg/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include/nanogui/*.h > ${CMAKE_CURRENT_SOURCE_DIR}/src/py_doc.h)

# vim: set et ts=2 sw=2 ft=cmake nospell:
