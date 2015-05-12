cmake_minimum_required (VERSION 2.8.3)

project("NanoGUI")

option(NANOGUI_INSTALL "Generate installation target for NanoGUI" ON)


# Parse arguments for nested build process
if(NOT CMAKE_EXTERNAL_ARGUMENTS)
  set(CMAKE_EXTERNAL_ARGUMENTS_LIST "")
else()
  string(REPLACE "$" ";" CMAKE_EXTERNAL_ARGUMENTS "${CMAKE_EXTERNAL_ARGUMENTS}")
  set(CMAKE_EXTERNAL_ARGUMENTS_LIST ${CMAKE_EXTERNAL_ARGUMENTS})
endif()

# Run simple C converter to put font files into the data segment
add_executable(bin2c resources/bin2c.c)
set(bin2c_cmdline resources.cpp resources.h)
file(GLOB resources "${CMAKE_SOURCE_DIR}/resources/*.ttf")
foreach(file ${resources})
  list(APPEND bin2c_cmdline ${file})
endforeach()

include(ExternalProject)

add_custom_command(
  OUTPUT resources.cpp resources.h
  COMMAND bin2c ARGS ${bin2c_cmdline}
  DEPENDS bin2c ${resources}
  COMMENT "Running bin2c"
  PRE_BUILD VERBATIM)


if(NANOGUI_INSTALL)
ExternalProject_Add(glfw_p
  URL             "${CMAKE_SOURCE_DIR}/ext/glfw"
  PREFIX          "ext_build"
  CMAKE_ARGS      ${CMAKE_EXTERNAL_ARGUMENTS_LIST} -DGLFW_BUILD_TESTS=0 -DGLFW_BUILD_EXAMPLES=0 -DGLFW_BUILD_DOCS=0
)
else()
ExternalProject_Add(glfw_p
  URL             "${CMAKE_SOURCE_DIR}/ext/glfw"
  PREFIX          "ext_build"
  CMAKE_ARGS      ${CMAKE_EXTERNAL_ARGUMENTS_LIST} -DGLFW_BUILD_TESTS=0 -DGLFW_BUILD_EXAMPLES=0 -DGLFW_BUILD_DOCS=0 -DGLFW_INSTALL=0
  INSTALL_COMMAND ""
)
endif()

set(extra_source "")
if(WIN32)
  if(NANOGUI_INSTALL)
    ExternalProject_Add(glew_p
      URL             "${CMAKE_SOURCE_DIR}/ext/glew"
      PREFIX          "ext_build"
      CMAKE_ARGS      ${CMAKE_EXTERNAL_ARGUMENTS_LIST}
    )
  else()
    ExternalProject_Add(glew_p
      URL             "${CMAKE_SOURCE_DIR}/ext/glew"
      PREFIX          "ext_build"
      CMAKE_ARGS      ${CMAKE_EXTERNAL_ARGUMENTS_LIST}
      INSTALL_COMMAND ""
    )
  endif()
  add_definitions (/D "_CRT_SECURE_NO_WARNINGS")
  include_directories(ext/glew/include)
  set(extra_libs opengl32
    $<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/ext_build/src/glew_p-build/Debug/glew.lib>
    $<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/ext_build/src/glew_p-build/Release/glew.lib>
    $<$<CONFIG:RelWithDebInfo>:${CMAKE_BINARY_DIR}/ext_build/src/glew_p-build/RelWithDebInfo/glew.lib>
    $<$<CONFIG:MinSizeRel>:${CMAKE_BINARY_DIR}/ext_build/src/glew_p-build/MinSizeRel/glew.lib>
  )
elseif (APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
  # Find standard libraries needed for OpenGL/GLFW
  find_library(cocoa_library Cocoa)
  find_library(opengl_library OpenGL)
  find_library(corevideo_library CoreVideo)
  find_library(iokit_library IOKit)
  set(extra_libs ${cocoa_library} ${opengl_library} ${corevideo_library} ${iokit_library})
  set(extra_source src/darwin.mm)
elseif("${CMAKE_SYSTEM}" MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(extra_libs GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 pthread dl)
endif()

include_directories(ext/eigen ext/glfw/include ext/nanovg/src include ${CMAKE_BINARY_DIR})
add_library(nanogui STATIC
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
  include/nanogui/toolbutton.h
  include/nanogui/popupbutton.h src/popupbutton.cpp
  include/nanogui/combobox.h src/combobox.cpp
  include/nanogui/progressbar.h src/progressbar.cpp
  include/nanogui/slider.h src/slider.cpp
  include/nanogui/messagedialog.h src/messagedialog.cpp
  include/nanogui/textbox.h src/textbox.cpp
  include/nanogui/imagepanel.h src/imagepanel.cpp
  include/nanogui/imageview.h src/imageview.cpp
  include/nanogui/vscrollpanel.h src/vscrollpanel.cpp
  ${extra_source}
)

add_dependencies(nanogui glfw_p)

if(WIN32)
  add_dependencies(nanogui glew_p)
endif()

file(GLOB NANOVG_HEADERS ext/nanovg/src/*.h)

if(NANOGUI_INSTALL)
  install(TARGETS nanogui ARCHIVE DESTINATION lib)
  install(FILES ${NANOVG_HEADERS} DESTINATION include)
  install(DIRECTORY ext/eigen/Eigen DESTINATION include)
  install(DIRECTORY include/nanogui DESTINATION include)
endif()

link_directories(${CMAKE_BINARY_DIR}/ext_build/src/glfw_p-build/src)
add_executable(example src/example.cpp)
target_link_libraries(example nanogui glfw3 ${extra_libs})
file(COPY resources/icons DESTINATION ${CMAKE_BINARY_DIR})

# vim: set et ts=2 sw=2 ft=cmake nospell:
