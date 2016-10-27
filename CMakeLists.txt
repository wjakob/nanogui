cmake_minimum_required (VERSION 2.8.12)

project("NanoGUI")

if (POLICY CMP0058)
  cmake_policy(SET CMP0058 NEW)
endif()

if (NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/src")
  message(FATAL_ERROR "The NanoGUI dependency repositories (GLFW, etc.) are missing! "
    "You probably did not clone the project with --recursive. It is possible to recover "
    "by calling \"git submodule update --init --recursive\"")
endif()

if (WIN32)
  set(NANOGUI_USE_GLAD_DEFAULT ON)
else()
  set(NANOGUI_USE_GLAD_DEFAULT OFF)
endif()

option(NANOGUI_BUILD_EXAMPLE "Build NanoGUI example application?" ON)
option(NANOGUI_BUILD_SHARED  "Build NanoGUI as a shared library?" ON)
option(NANOGUI_BUILD_PYTHON  "Build a Python plugin for NanoGUI?" ON)
option(NANOGUI_USE_GLAD      "Use Glad OpenGL loader library?" ${NANOGUI_USE_GLAD_DEFAULT})
option(NANOGUI_INSTALL       "Install NanoGUI on `make install`?" ON)

set(NANOGUI_PYTHON_VERSION "" CACHE STRING "Python version to use for compiling the Python plugin")

# Required libraries, flags, and include files for compiling and linking against nanogui (all targets)
set(NANOGUI_EXTRA_LIBS "")
set(NANOGUI_EXTRA_DEFS "")
set(NANOGUI_EXTRA_INCS "")

# Platform-dependent files for libnanogui
set(LIBNANOGUI_EXTRA_SOURCE "")
set(LIBNANOGUI_PYTHON_EXTRA_SOURCE "")

if(APPLE AND NANOGUI_BUILD_SHARED)
  set(CMAKE_MACOSX_RPATH ON)
endif()

include(CheckCXXCompilerFlag)

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
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
set(BUILD_SHARED_LIBS ${NANOGUI_BUILD_SHARED} CACHE BOOL " " FORCE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Quench annoying deprecation warnings when compiling GLFW on OSX
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
endif()

# Compile GLFW
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw" "ext_build/glfw")

# Python support: add NANOGUI_PYTHON flag to all targets
if (NANOGUI_BUILD_PYTHON)
  list(APPEND NANOGUI_EXTRA_DEFS -DNANOGUI_PYTHON)
endif()

# Shared library mode: add NANOGUI_SHARED flag to all targets
if (NANOGUI_BUILD_SHARED)
  list(APPEND NANOGUI_EXTRA_DEFS -DNANOGUI_SHARED)
endif()

if (MSVC)
  # Disable annoying MSVC warnings (all targets)
  add_definitions(/D "_CRT_SECURE_NO_WARNINGS")

  # Parallel build on MSVC (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")

    # Disable Eigen vectorization for Windows 32 bit builds (issues with unaligned access segfaults)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNANOGUI_EIGEN_DONT_ALIGN")
  endif()
endif()

# Compile with compiler warnings turned on
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|Intel)$")
  CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CPP11_FLAG)

  if (HAS_CPP14_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  elseif (HAS_CPP11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(FATAL_ERROR "Unsupported compiler -- pybind11 requires C++11 support!")
  endif()
endif()

# Various optimizations for shared library release builds
if (NANOGUI_BUILD_SHARED)
  if (U_CMAKE_BUILD_TYPE MATCHES REL AND CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
    # Set the default symbol visibility to hidden
    if (NOT CMAKE_CXX_FLAGS MATCHES "-fvisibility")
      set(CMAKE_CXX_FLAGS "-fvisibility=hidden ${CMAKE_CXX_FLAGS}")
    endif()

    # Enable link time optimization
    if (NOT CMAKE_CXX_FLAGS MATCHES "-flto")
      set(BACKUP_C_FLAGS ${CMAKE_CXX_FLAGS})
      set(BACKUP_CXX_FLAGS ${CMAKE_C_FLAGS})
      set(BACKUP_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
      set(BACKUP_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})

      if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_EXE_LINKER_FLAGS "-flto=thin ${CMAKE_EXE_LINKER_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "-flto=thin ${CMAKE_SHARED_LINKER_FLAGS}")
        set(CMAKE_CXX_FLAGS "-flto=thin ${CMAKE_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "-flto=thin ${CMAKE_C_FLAGS}")
        if (NOT APPLE AND U_CMAKE_BUILD_TYPE MATCHES MINSIZEREL)
            # Clang Gold plugin does not support -Os
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -O3")
        endif()
      else()
        set(CMAKE_EXE_LINKER_FLAGS "-flto ${CMAKE_EXE_LINKER_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "-flto ${CMAKE_SHARED_LINKER_FLAGS}")
        set(CMAKE_CXX_FLAGS "-flto -fno-fat-lto-objects ${CMAKE_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "-flto -fno-fat-lto-objects ${CMAKE_C_FLAGS}")
      endif()

      file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/CMakeTmp/test.c "int main(int argc, char **argv) { return 0; }")
      try_compile(HAS_LTO
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/CMakeTmp
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/CMakeTmp/test.c)

      if (HAS_LTO)
        message(STATUS "NanoGUI: LTO support enabled.")
      else()
        message(STATUS "NanoGUI: LTO not supported by the compiler.")
        set(CMAKE_C_FLAGS ${BACKUP_CXX_FLAGS})
        set(CMAKE_CXX_FLAGS ${BACKUP_C_FLAGS})
        set(CMAKE_EXE_LINKER_FLAGS ${BACKUP_EXE_LINKER_FLAGS})
        set(CMAKE_SHARED_LINKER_FLAGS ${BACKUP_SHARED_LINKER_FLAGS})
      endif()
    endif()
  elseif(MSVC)
    set(Configurations RELEASE RELWITHDEBINFO MINSIZEREL)
    set(LinkTypes EXE SHARED MODULE STATIC)
    foreach(Configuration ${Configurations})
      set("CMAKE_CXX_FLAGS_${Configuration}" "${CMAKE_CXX_FLAGS_${Configuration}} /GL")
      foreach(LinkType ${LinkTypes})
        set("CMAKE_${LinkType}_LINKER_FLAGS_${Configuration}" "${CMAKE_${LinkType}_LINKER_FLAGS_${Configuration}} /LTCG")
        message(STATUS CMAKE_${LinkType}_LINKER_FLAGS_${Configuration})
      endforeach()
    endforeach()
    message(STATUS "NanoGUI: LTO support enabled.")
  endif()
endif()

# Always use libc++ on Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()

if (NANOGUI_USE_GLAD)
  # Build and include GLAD on Windows
  list(APPEND LIBNANOGUI_EXTRA_SOURCE
     "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/src/glad.c"
	 "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/include/glad/glad.h"
	 "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/include/KHR/khrplatform.h")
  if (MSVC)
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/src/glad.c"
      PROPERTIES COMPILE_FLAGS "/wd4055 ")
  endif()
  include_directories(ext/glad/include)
  list(APPEND NANOGUI_EXTRA_DEFS -DNANOGUI_GLAD)
  list(APPEND NANOGUI_EXTRA_INCS "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/include")
endif()

list(APPEND NANOGUI_EXTRA_INCS
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/nanovg/src"
)

if (NOT NANOGUI_EIGEN_INCLUDE_DIR)
  set(NANOGUI_EIGEN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/eigen")
  list(APPEND NANOGUI_EXTRA_INCS "${NANOGUI_EIGEN_INCLUDE_DIR}")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "BSD")
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
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
elseif(CMAKE_SYSTEM MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "BSD")
  list(APPEND NANOGUI_EXTRA_LIBS GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 pthread rt)
  if(CMAKE_SYSTEM MATCHES "Linux")
    list(APPEND NANOGUI_EXTRA_LIBS dl)
  endif()
endif()

include_directories(${NANOGUI_EIGEN_INCLUDE_DIR} ext/glfw/include ext/nanovg/src include ${CMAKE_CURRENT_BINARY_DIR})

# Run simple C converter to put font files into the data segment
add_executable(bin2c resources/bin2c.c)
set(bin2c_cmdline nanogui_resources.cpp nanogui_resources.h)
file(GLOB resources "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.ttf")
foreach(file ${resources})
  list(APPEND bin2c_cmdline ${file})
endforeach()

# Run bin2c on resource files
add_custom_command(
  OUTPUT nanogui_resources.cpp nanogui_resources.h
  COMMAND $<TARGET_FILE:bin2c> ARGS ${bin2c_cmdline}
  DEPENDS bin2c ${resources}
  COMMENT "Running bin2c"
  PRE_BUILD VERBATIM)

# Needed to generated files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Set library type
if (NANOGUI_BUILD_SHARED)
  set(NANOGUI_LIBRARY_TYPE "SHARED")
else()
  set(NANOGUI_LIBRARY_TYPE "STATIC")
endif()

if (APPLE OR CMAKE_SYSTEM MATCHES "Linux")
  # Include coroutine support for running the mainloop in detached mode
  add_definitions(-DCORO_SJLJ)
  include_directories(ext/coro)
  list(APPEND LIBNANOGUI_PYTHON_EXTRA_SOURCE ext/coro/coro.c)
endif()

add_definitions(${NANOGUI_EXTRA_DEFS})

# Compile main NanoGUI library
add_library(nanogui-obj OBJECT
  # Merge NanoVG into the NanoGUI library
  ext/nanovg/src/nanovg.c
  # Merge GLAD into the NanoGUI library (only if needed)
  ${LIBNANOGUI_EXTRA_SOURCE}
  # Fonts etc.
  nanogui_resources.cpp
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
  include/nanogui/stackedwidget.h src/stackedwidget.cpp
  include/nanogui/tabheader.h src/tabheader.cpp
  include/nanogui/tabwidget.h src/tabwidget.cpp
  include/nanogui/glcanvas.h src/glcanvas.cpp
  include/nanogui/formhelper.h
  include/nanogui/toolbutton.h
  include/nanogui/opengl.h
  include/nanogui/nanogui.h
  include/nanogui/serializer/core.h
  include/nanogui/serializer/opengl.h
  include/nanogui/serializer/sparse.h
  src/serializer.cpp
)

add_library(nanogui ${NANOGUI_LIBRARY_TYPE}
  $<TARGET_OBJECTS:nanogui-obj>
  $<TARGET_OBJECTS:glfw_objects>
)

if (${NANOGUI_LIBRARY_TYPE} MATCHES "SHARED")
  set_property(TARGET nanogui-obj PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

# Compile/link flags for NanoGUI
set_property(TARGET nanogui-obj APPEND PROPERTY COMPILE_DEFINITIONS "NANOGUI_BUILD")

if (NANOGUI_USE_GLAD AND NANOGUI_BUILD_SHARED)
  set_property(TARGET nanogui-obj APPEND PROPERTY COMPILE_DEFINITIONS
    "GLAD_GLAPI_EXPORT;GLAD_GLAPI_EXPORT_BUILD")
endif()

if (NANOGUI_BUILD_SHARED)
  target_link_libraries(nanogui ${NANOGUI_EXTRA_LIBS})
endif()

if (NANOGUI_INSTALL)
  install(TARGETS nanogui
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)

  install(DIRECTORY include/nanogui DESTINATION include
          FILES_MATCHING PATTERN "*.h")
endif()

if (NANOGUI_BUILD_SHARED)
  # When GLFW is merged into the NanoGUI library, this flag must be specified
  set_property(TARGET nanogui-obj APPEND PROPERTY COMPILE_DEFINITIONS "_GLFW_BUILD_DLL")
endif()

if (NANOGUI_BUILD_SHARED AND NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEB)
  # Platform-specific strip flags for reducing the library size.
  if (APPLE)
    # Strip .dylib library on OSX
    add_custom_command(TARGET nanogui POST_BUILD COMMAND strip -u -r "$<TARGET_FILE_DIR:nanogui>/$<TARGET_FILE_NAME:nanogui>")
  elseif(UNIX)
    # Strip .so library on Linux
    add_custom_command(TARGET nanogui POST_BUILD COMMAND strip "$<TARGET_FILE_DIR:nanogui>/$<TARGET_FILE_NAME:nanogui>")
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
  add_executable(example3 src/example3.cpp)
  add_executable(example4 src/example4.cpp)
  target_link_libraries(example1 nanogui ${NANOGUI_EXTRA_LIBS})
  target_link_libraries(example2 nanogui ${NANOGUI_EXTRA_LIBS})
  target_link_libraries(example3 nanogui ${NANOGUI_EXTRA_LIBS})
  target_link_libraries(example4 nanogui ${NANOGUI_EXTRA_LIBS})

  # Copy icons for example application
  file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

  if (NANOGUI_INSTALL)
    install(TARGETS example1 example2 example3 example4
            RUNTIME DESTINATION bin)
  endif()
endif()

if (NANOGUI_BUILD_PYTHON)
  # Detect Python

  # Try to autodetect Python (can be overridden manually if needed)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11/tools")
  set(Python_ADDITIONAL_VERSIONS 3.4 3.5 3.6 3.7)
  find_package(PythonLibsNew ${MITSUBA_PYTHON_VERSION})
  if (NOT PYTHONLIBS_FOUND)
    # Python not found -- disable the plugin
    set(NANOGUI_BUILD_PYTHON OFF CACHE BOOL "Build a Python plugin for NanoGUI?" FORCE)
    message(WARNING "NanoGUI: not building the Python plugin!")
  else()
    message(STATUS "NanoGUI: building the Python plugin.")
  endif()
endif()

if (NANOGUI_BUILD_PYTHON)
  # Need PIC code in libnanogui even when compiled as a static library
  set_target_properties(nanogui-obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
  set_target_properties(glfw_objects PROPERTIES POSITION_INDEPENDENT_CODE ON)

  include_directories("ext/pybind11/include" ${PYTHON_INCLUDE_DIR})
  add_library(nanogui-python-obj OBJECT
    python/python.cpp
    python/constants.cpp
    python/eigen.cpp
    python/widget.cpp
    python/layout.cpp
    python/basics.cpp
    python/button.cpp
    python/tabs.cpp
    python/textbox.cpp
    python/glcanvas.cpp
    python/formhelper.cpp
    python/misc.cpp
    python/glutil.cpp
    python/python.h python/py_doc.h
    ${LIBNANOGUI_PYTHON_EXTRA_SOURCE})

  add_library(nanogui-python SHARED $<TARGET_OBJECTS:nanogui-python-obj>)
  set_property(TARGET nanogui-python-obj PROPERTY POSITION_INDEPENDENT_CODE ON)
  set_target_properties(nanogui-python PROPERTIES OUTPUT_NAME "nanogui")
  target_link_libraries(nanogui-python nanogui ${NANOGUI_EXTRA_LIBS})

  # Quench warnings on GCC
  if (CMAKE_COMPILER_IS_GNUCC)
    set_property(TARGET nanogui-python-obj APPEND PROPERTY COMPILE_OPTIONS "-Wno-unused-variable")
  endif()

  set_target_properties(nanogui-python PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python)
  # The prefix and extension are provided by FindPythonLibsNew.cmake
  set_target_properties(nanogui-python PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}")
  set_target_properties(nanogui-python PROPERTIES SUFFIX "${PYTHON_MODULE_EXTENSION}")

  if (WIN32)
    # Set output path
    set_target_properties(nanogui-python PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "Release/python")
    set_target_properties(nanogui-python PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "Debug/python")
    set_target_properties(nanogui-python PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "MinSizeRel/python")
    set_target_properties(nanogui-python PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "RelWithDebInfo/python")
    set_target_properties(nanogui-python PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "Release/python")
    set_target_properties(nanogui-python PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "Debug/python")
    set_target_properties(nanogui-python PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "MinSizeRel/python")
    set_target_properties(nanogui-python PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "RelWithDebInfo/python")

    # Link against the Python shared library
    target_link_libraries(nanogui-python ${PYTHON_LIBRARY})

    if (MSVC)
      # Optimize for size, /bigobj is needed for due to the heavy template metaprogramming in pybind11
      set_property(TARGET nanogui-python-obj APPEND PROPERTY COMPILE_OPTIONS
        "/bigobj" "$<$<CONFIG:Release>:/Os>" "$<$<CONFIG:MinSizeRel>:/Os>"
        "$<$<CONFIG:RelWithDebInfo>:/Os>")
    endif()
  elseif(UNIX)
    # Optimize for size
    if (U_CMAKE_BUILD_TYPE MATCHES REL)
      set_property(TARGET nanogui-python-obj APPEND PROPERTY COMPILE_OPTIONS "-Os")
    endif()

    # Strip unnecessary sections of the binary on Linux/Mac OS
    if(APPLE)
      set_target_properties(nanogui-python PROPERTIES MACOSX_RPATH ".")
      set_target_properties(nanogui-python PROPERTIES LINK_FLAGS "-undefined dynamic_lookup ")

      if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEB)
        add_custom_command(TARGET nanogui-python POST_BUILD COMMAND strip -u -r $<TARGET_FILE:nanogui-python>)
      endif()
    else()
      if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEB)
        add_custom_command(TARGET nanogui-python POST_BUILD COMMAND strip $<TARGET_FILE:nanogui-python>)
      endif()
    endif()
  endif()

  if (NANOGUI_INSTALL)
    install(TARGETS nanogui-python
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)
  endif()
endif()

get_directory_property(NANOGUI_HAS_PARENT PARENT_DIRECTORY)
if(NANOGUI_HAS_PARENT)
  # This project is included from somewhere else. Export NANOGUI_EXTRA_LIBS variable
  set(NANOGUI_EXTRA_LIBS ${NANOGUI_EXTRA_LIBS} PARENT_SCOPE)
  set(NANOGUI_EXTRA_DEFS ${NANOGUI_EXTRA_DEFS} PARENT_SCOPE)
  set(NANOGUI_EXTRA_INCS ${NANOGUI_EXTRA_INCS} PARENT_SCOPE)
else()
  # Create documentation for python plugin (optional target for developers)

  string(REPLACE " " ";" MKDOC_CXX_FLAGS_LIST ${CMAKE_CXX_FLAGS})
  get_property(MKDOC_INCLUDE_DIRECTORIES DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
  get_property(MKDOC_COMPILE_DEFINITIONS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY COMPILE_DEFINITIONS)

  foreach (value ${MKDOC_INCLUDE_DIRECTORIES})
    list(APPEND MKDOC_CXX_FLAGS_LIST -I${value})
  endforeach()

  foreach (value ${MKDOC_COMPILE_DEFINITIONS})
    list(APPEND MKDOC_CXX_FLAGS_LIST -D${value})
  endforeach()

  add_custom_target(mkdoc COMMAND
    python3 ${PROJECT_SOURCE_DIR}/docs/mkdoc_rst.py
      ${MKDOC_CXX_FLAGS_LIST}
      ${PROJECT_SOURCE_DIR}/include/nanogui/*.h
      > ${CMAKE_CURRENT_SOURCE_DIR}/python/py_doc.h)

endif()

# vim: set et ts=2 sw=2 ft=cmake nospell:
