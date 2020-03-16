cmake_minimum_required (VERSION 2.8.12)

if (POLICY CMP0048)
  # cmake warns if loaded from a min-3.0-required parent dir, so silence the warning:
  cmake_policy(SET CMP0048 NEW)
endif()

project("NanoGUI")

if (POLICY CMP0058)
  cmake_policy(SET CMP0058 NEW)
endif()

if (WIN32)
  set(NANOGUI_USE_GLAD_DEFAULT ON)
else()
  set(NANOGUI_USE_GLAD_DEFAULT OFF)
endif()

option(NANOGUI_BUILD_EXAMPLE "Build NanoGUI glfw example application?" ON)
option(NANOGUI_VULKAN_BACKEND  "Use Vulkan backend?" OFF)
option(NANOGUI_GLFW_BACKEND  "Use OpenGL backend?" OFF)
option(NANOGUI_DX11_BACKEND  "Use DX11 backend?" OFF)
option(NANOGUI_DX12_BACKEND  "Use DX12 backend?" OFF)
OPTION(NANOGUI_BGFX_BACKEND  "Use BGFX backend?" OFF)
option(NANOGUI_BUILD_EDITOR  "Build NanoGUI editor application?" ON)
option(NANOGUI_BUILD_HONSU   "Build NanoGUI timetracker application?" OFF)
option(NANOGUI_BUILD_SHARED  "Build NanoGUI as a shared library?" ON)
option(NANOGUI_BUILD_PYTHON  "Build a Python plugin for NanoGUI?" ON)
option(NANOGUI_USE_GLAD      "Use Glad OpenGL loader library?" ${NANOGUI_USE_GLAD_DEFAULT})
option(NANOGUI_INSTALL       "Install NanoGUI on `make install`?" ON)
option(NANOGUI_VULKAN_NOSDK  "Build vulkan with headers from ext folder. Travis-ci builds only" OFF)

set(NANOGUI_PYTHON_VERSION "" CACHE STRING "Python version to use for compiling the Python plugin")

# Check that we select backend and only one backend
if (NANOGUI_DX11_BACKEND OR NANOGUI_GLFW_BACKEND OR NANOGUI_DX12_BACKEND OR NANOGUI_VULKAN_BACKEND)  
  set(BACKENDS_SELECTED "")
  if (NANOGUI_GLFW_BACKEND) 
    add_definitions(-DNANOGUI_OPENGL_BACKEND=1)
    list(APPEND BACKENDS_SELECTED "OpenGl") 
  endif()
  if (NANOGUI_DX11_BACKEND) 
    add_definitions(-DNANOGUI_DX11_BACKEND=1)
    list(APPEND BACKENDS_SELECTED "DX11") 
  endif()
  if (NANOGUI_DX12_BACKEND) 
    add_definitions(-DNANOGUI_DX12_BACKEND=1)
    list(APPEND BACKENDS_SELECTED "DX12") 
  endif() 
  if (NANOGUI_VULKAN_BACKEND) 
    add_definitions(-DNANOGUI_VULKAN_BACKEND=1)
    list(APPEND BACKENDS_SELECTED "Vulkan") 
  endif()

  set(BACKENDS_SELECTED_SIZE "")
  list(LENGTH BACKENDS_SELECTED BACKENDS_SELECTED_SIZE)
  if (NOT ${BACKENDS_SELECTED_SIZE} EQUAL "1")
    message(FATAL_ERROR "Multiple backends selected! "
      "Select one of backends: opengl, dx11, dx12, vulkan"
      "Current selected: ${BACKENDS_SELECTED}"
    )	
  endif()    
else()
  message(FATAL_ERROR "No backends selected! "
     "Select one of backends: opengl, dx11, dx12, vulkan"
  )	  
endif()

if (NANOGUI_GLFW_BACKEND OR NANOGUI_VULKAN_BACKEND)  
  if (NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/src")
    message(FATAL_ERROR "The NanoGUI dependency repositories (GLFW, etc.) are missing! "
      "You probably did not clone the project with --recursive. It is possible to recover "
      "by calling \"git submodule update --init --recursive\"")
  endif()
endif()

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
include(CheckCXXSourceRuns)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

macro(CHECK_CXX_COMPILER_AND_LINKER_FLAGS _RESULT _CXX_FLAGS _LINKER_FLAGS)
  set(CMAKE_REQUIRED_FLAGS ${_CXX_FLAGS})
  set(CMAKE_REQUIRED_LIBRARIES ${_LINKER_FLAGS})
  set(CMAKE_REQUIRED_QUIET TRUE)
  check_cxx_source_runs("int main(int argc, char **argv) { return 0; }" ${_RESULT})
  set(CMAKE_REQUIRED_FLAGS "")
  set(CMAKE_REQUIRED_LIBRARIES "")
endmacro()

# Compile GLFW
if (NANOGUI_GLFW_BACKEND OR NANOGUI_VULKAN_BACKEND)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
  set(GLFW_BUILD_INSTALL OFF CACHE BOOL " " FORCE)
  set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
  set(GLFW_USE_CHDIR OFF CACHE BOOL " " FORCE)
  set(BUILD_SHARED_LIBS ${NANOGUI_BUILD_SHARED} CACHE BOOL " " FORCE)

  # Two targets have now been defined: `glfw_objects`, which will be merged into
  # NanoGUI at the end, and `glfw`.  The `glfw` target is the library itself
  # (e.g., libglfw.so), but can be skipped as we do not need to link against it
  # (because we merge `glfw_objects` into NanoGUI).  Skipping is required for
  # XCode, but preferable for all build systems (reduces build artifacts).
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw" "ext_build/glfw")

  set_target_properties(glfw PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
elseif (NANOGUI_DX11_BACKEND)

endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Quench annoying deprecation warnings when compiling GLFW on OSX
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
endif()


# Python support: add NANOGUI_PYTHON flag to all targets
if (NANOGUI_BUILD_PYTHON)
  list(APPEND NANOGUI_EXTRA_DEFS -DNANOGUI_PYTHON) 
endif()

# Shared library mode: add dllimport/dllexport flags to all symbols
if (NANOGUI_BUILD_SHARED)
  list(APPEND NANOGUI_EXTRA_DEFS -DNANOGUI_SHARED -DNVG_SHARED -DGLAD_GLAPI_EXPORT)
endif()

if (MSVC)
  # Disable annoying MSVC warnings (all targets)
  add_definitions("/D_CRT_SECURE_NO_WARNINGS")

  # Parallel build on MSVC (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")

  endif()
endif()

# Compile with compiler warnings turned on
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /W4505")
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
      if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(LTO_CXX_FLAGS "-flto=thin")
        set(LTO_LINKER_FLAGS "-flto=thin")
        if (NOT APPLE AND U_CMAKE_BUILD_TYPE MATCHES MINSIZEREL)
            # Clang Gold plugin does not support -Os
            set(LTO_CXX_FLAGS "${LTO_CXX_FLAGS} -O3")
        endif()
      else()
        set(LTO_CXX_FLAGS "-flto -fno-fat-lto-objects")
        set(LTO_LINKER_FLAGS "-flto")
      endif()

      CHECK_CXX_COMPILER_AND_LINKER_FLAGS(HAS_LTO ${LTO_CXX_FLAGS} ${LTO_LINKER_FLAGS})

      if (HAS_LTO)
        message(STATUS "NanoGUI: LTO support enabled.")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LTO_LINKER_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LTO_LINKER_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LTO_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LTO_CXX_FLAGS}")
      else()
        message(STATUS "NanoGUI: LTO not supported by the compiler.")
      endif()
    endif()
  elseif(MSVC)
    set(Configurations RELEASE RELWITHDEBINFO MINSIZEREL)
    set(LinkTypes EXE SHARED MODULE STATIC)
    foreach(Configuration ${Configurations})
      set("CMAKE_CXX_FLAGS_${Configuration}" "${CMAKE_CXX_FLAGS_${Configuration}} /GL")
      foreach(LinkType ${LinkTypes})
        set("CMAKE_${LinkType}_LINKER_FLAGS_${Configuration}" "${CMAKE_${LinkType}_LINKER_FLAGS_${Configuration}} /LTCG")
      endforeach()
    endforeach()
    message(STATUS "NanoGUI: LTO support enabled.")
  endif()
endif()

# Always use libc++ on Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  CHECK_CXX_COMPILER_AND_LINKER_FLAGS(HAS_LIBCPP "-stdlib=libc++" "-stdlib=libc++")
  if (HAS_LIBCPP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")
    CHECK_CXX_COMPILER_AND_LINKER_FLAGS(HAS_LIBCPPABI "-stdlib=libc++" "-stdlib=libc++ -lc++abi")
    if(HAS_LIBCPPABI)
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++abi")
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lc++abi")
      message(STATUS "NanoGUI: using libc++ and libc++abi.")
    else()
      message(STATUS "NanoGUI: using libc++.")
    endif()
  else()
    message(STATUS "NanoGUI: NOT using libc++.")
  endif()
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

list(APPEND NANOGUI_EXTRA_INCS "${CMAKE_CURRENT_SOURCE_DIR}/ext/nanovg/src")

if (NANOGUI_GLFW_BACKEND OR NANOGUI_VULKAN_BACKEND)
  list(APPEND NANOGUI_EXTRA_INCS "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/include")
elseif (NANOGUI_DX11_BACKEND)
  add_definitions(-DNANOVG_D3D11)  
elseif (NANOGUI_DX11_BACKEND)
  add_definitions(-DNANOVG_D3D12)  
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "BSD")
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
  if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
    include_directories(/usr/X11R6/include/)
    link_directories(/usr/X11R6/lib)
  endif()
endif()

# Required core libraries on various platforms
if (NANOGUI_GLFW_BACKEND)
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
    list(APPEND NANOGUI_EXTRA_LIBS GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 pthread )
    if (NOT CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
      list(APPEND NANOGUI_EXTRA_LIBS rt)
    endif()
    if(CMAKE_SYSTEM MATCHES "Linux")
      list(APPEND NANOGUI_EXTRA_LIBS dl)
    endif()
  endif()
elseif (NANOGUI_VULKAN_BACKEND)
  if (NANOGUI_VULKAN_NOSDK)
    set(Vulkan_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/vulkan/Include")
    set(Vulkan_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/blob/vulkan-1.lib")
  else()
    find_package(Vulkan REQUIRED)
    if(NOT Vulkan_FOUND)
      message("ERROR: Vulkan not found")
    endif(NOT Vulkan_FOUND)

  endif()
  message( "Vulkan include header found: ${Vulkan_INCLUDE_DIR}")
  include_directories(${Vulkan_INCLUDE_DIR})
  list(APPEND NANOGUI_EXTRA_LIBS ${Vulkan_LIBRARY}) 
elseif (NANOGUI_DX11_BACKEND)
  list(APPEND NANOGUI_EXTRA_LIBS d3d11 dxguid)
elseif (NANOGUI_DX12_BACKEND)
  list(APPEND NANOGUI_EXTRA_LIBS dxgi)
endif()

include_directories(ext/nanovg/src include ${CMAKE_CURRENT_BINARY_DIR})

if (NANOGUI_GLFW_BACKEND OR NANOGUI_VULKAN_BACKEND)
  include_directories(ext/glfw/include)
endif()

# Run simple cmake converter to put font files into the data segment

# Glob up resource files
file(GLOB resources "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.ttf")

# Concatenate resource files into a comma separated string
string (REGEX REPLACE "([^\\]|^);" "\\1," resources_string "${resources}")
string (REGEX REPLACE "[\\](.)" "\\1" resources_string "${resources_string}")

# Create command line for running bin2c cmake script
set(bin2c_cmdline
  -DOUTPUT_C=nanogui_resources.cpp
  -DOUTPUT_H=nanogui_resources.h
  "-DINPUT_FILES=${resources_string}"
  -P "${CMAKE_CURRENT_SOURCE_DIR}/resources/bin2c.cmake")

# Run bin2c on resource files
add_custom_command(
  OUTPUT nanogui_resources.cpp nanogui_resources.h
  COMMAND ${CMAKE_COMMAND} ARGS ${bin2c_cmdline}
  DEPENDS ${resources}
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

if (APPLE)
  # Use automatic reference counting for Objective-C portions
  add_compile_options(-fobjc-arc)
endif()

add_definitions(${NANOGUI_EXTRA_DEFS})
set(NANOGUI_BACKEND_SOURCES "")

if (NANOGUI_GLFW_BACKEND)
  list(APPEND NANOGUI_BACKEND_SOURCES  
    include/nanogui/glcanvas.h src/glfw/glcanvas_glfw.cpp
    include/nanogui/glutil.h src/glfw/glutil_glfw.cpp

    include/nanogui/opengl.h
    include/nanogui/serializer/opengl.h
    src/glfw/common_glfw.cpp
    src/glfw/gpu_glfw.cpp
  )
elseif (NANOGUI_VULKAN_BACKEND)
  list (APPEND NANOGUI_BACKEND_SOURCES 
    include/nanogui/nanovg_vk.h 
    src/vulkan/vulkan_util.h 
    src/vulkan/common_vk.cpp
    src/vulkan/shaders_vk.cpp
    src/vulkan/gpu_vk.cpp
  ) 
elseif (NANOGUI_DX11_BACKEND)
  list(APPEND NANOGUI_BACKEND_SOURCES
    src/dx11/common_dx11.h src/dx11/common_dx11.cpp 
    src/dx11/gpu_dx11.cpp
  )
elseif (NANOGUI_DX12_BACKEND)
  list(APPEND NANOGUI_BACKEND_SOURCES
    src/dx12/common_dx12.h src/dx12/common_dx12.cpp 
    src/dx12/gpu_dx12.cpp
	
  	include/nanogui/nanovg_d3d12.h  src/dx12/nanovg_d3d12.cpp
  	src/dx12/nanovg_dx12_utils.cpp
  	src/dx12/nanovg_dx12_render.cpp
  	src/dx12/nanovg_dx12_alloc.cpp
  	src/dx12/nanovg_dx12_res.cpp
      
  	src/dx12/dx12_dyn_imports.h src/dx12/dx12_dyn_imports.cpp
    src/dx12/dx12_subset.h src/dx12/dx12_subset.cpp
  	src/dx12/dx12_subset_resource.h src/dx12/dx12_subset_resource.cpp
  	src/dx12/dx12_subset_dheap.h src/dx12/dx12_subset_dheap.cpp
  	src/dx12/dx12_subset_pso_lib.cpp
  )
endif()

# Compile main NanoGUI library
add_library(nanogui-obj OBJECT
  # Merge NanoVG into the NanoGUI library
  ext/nanovg/src/nanovg.c
  # Merge GLAD into the NanoGUI library (only if needed)
  ${LIBNANOGUI_EXTRA_SOURCE}
  # Fonts etc.
  nanogui_resources.cpp
  include/nanogui/common.h src/common.cpp
  include/nanogui/keyboard.h src/keyboard.cpp
  include/nanogui/widget.h src/widget.cpp
  include/nanogui/theme.h src/theme.cpp
  include/nanogui/layout.h src/layout.cpp
  include/nanogui/screen.h src/screen.cpp
  include/nanogui/label.h src/label.cpp
  include/nanogui/window.h src/window.cpp
  include/nanogui/listbox.h src/listbox.cpp
  include/nanogui/popup.h src/popup.cpp
  include/nanogui/meter.h src/meter.cpp
  include/nanogui/checkbox.h src/checkbox.cpp
  include/nanogui/spinner.h src/spinner.cpp
  include/nanogui/separator.h src/separator.cpp
  include/nanogui/color.h src/color.cpp
  include/nanogui/switchbox.h src/switchbox.cpp
  include/nanogui/tolerancebar.h src/tolerancebar.cpp
  include/nanogui/dropdownbox.h src/dropdownbox.cpp
  include/nanogui/button.h src/button.cpp
  include/nanogui/popupbutton.h src/popupbutton.cpp
  include/nanogui/combobox.h src/combobox.cpp
  include/nanogui/progressbar.h src/progressbar.cpp
  include/nanogui/slider.h src/slider.cpp
  include/nanogui/dial.h src/dial.cpp
  include/nanogui/messagedialog.h src/messagedialog.cpp
  include/nanogui/textbox.h src/textbox.cpp
  include/nanogui/imagepanel.h src/imagepanel.cpp
  include/nanogui/imageview.h src/imageview.cpp
  include/nanogui/themebuilder.h src/themebuilder.cpp
  include/nanogui/treeview.h src/treeview.cpp
  include/nanogui/treeviewitem.h src/treeviewitem.cpp
  include/nanogui/vscrollpanel.h src/vscrollpanel.cpp
  include/nanogui/colorwheel.h src/colorwheel.cpp
  include/nanogui/colorpicker.h src/colorpicker.cpp
  include/nanogui/picflow.h src/picflow.cpp
  include/nanogui/graph.h src/graph.cpp
  include/nanogui/textarea.h src/textarea.cpp
  include/nanogui/stackedwidget.h src/stackedwidget.cpp
  include/nanogui/tabheader.h src/tabheader.cpp
  include/nanogui/tabwidget.h src/tabwidget.cpp
  include/nanogui/searchbox.h src/searchbox.cpp
  include/nanogui/contextmenu.h src/contextmenu.cpp
  include/nanogui/editworkspace.h src/editworkspace.cpp
  include/nanogui/editproperties.h src/editproperties.cpp
  include/nanogui/widgetsfactory.h src/widgetsfactory.cpp
  include/nanogui/scrollbar.h src/scrollbar.cpp
  include/nanogui/widgetsfactory.h src/widgetsfactory.cpp
  include/nanogui/foldout.h src/foldout.cpp
  include/nanogui/windowmenu.h src/windowmenu.cpp
  include/nanogui/ledmatrix.h src/ledmatrix.cpp
  include/nanogui/table.h src/table.cpp
  include/nanogui/perfchart.h src/perfchart.cpp
  include/nanogui/formhelper.h
  include/nanogui/toolbutton.h src/toolbutton.cpp
  include/nanogui/serializer/core.h    src/serializer.cpp

  include/nanogui/serializer/sparse.h
  include/nanogui/serializer/json.h

  ${NANOGUI_BACKEND_SOURCES}
)

# XCode has a serious bug where the XCode project produces an invalid target
# that will not get linked if it consists only of objects from object libraries,
# it will not generate any products (executables, libraries). The only work
# around is to add a dummy source file to the library definition. This is an
# XCode, not a CMake bug. See: https://itk.org/Bug/view.php?id=14044
if (CMAKE_GENERATOR STREQUAL Xcode)
  set(XCODE_DUMMY ${CMAKE_CURRENT_BINARY_DIR}/xcode_dummy.cpp)
  file(WRITE ${XCODE_DUMMY} "")
  add_library(nanogui ${NANOGUI_LIBRARY_TYPE}
    ${XCODE_DUMMY}
    $<TARGET_OBJECTS:nanogui-obj>
    $<TARGET_OBJECTS:glfw_objects>
  )
else()
  if (NANOGUI_GLFW_BACKEND OR NANOGUI_VULKAN_BACKEND)
    add_library(nanogui ${NANOGUI_LIBRARY_TYPE}
      $<TARGET_OBJECTS:nanogui-obj>
      $<TARGET_OBJECTS:glfw_objects>
    )
  else()
    add_library(nanogui ${NANOGUI_LIBRARY_TYPE}
      $<TARGET_OBJECTS:nanogui-obj>
    )
  endif()
endif()

if (NANOGUI_BUILD_SHARED)
  set_property(TARGET nanogui-obj PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

# Compile/link flags for NanoGUI
set_property(TARGET nanogui-obj APPEND PROPERTY COMPILE_DEFINITIONS "NANOGUI_BUILD;NVG_BUILD")

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

if (NANOGUI_BUILD_SHARED AND NANOGUI_GLFW_BACKEND)
  # When GLFW is merged into the NanoGUI library, this flag must be specified
  set_property(TARGET nanogui-obj APPEND PROPERTY COMPILE_DEFINITIONS "_GLFW_BUILD_DLL;NVG_SHARED")
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

# Build example application if desired
if(NANOGUI_BUILD_EDITOR)
  add_executable(editorl      
    editor/editor.cpp editor/widgetctor.cpp)
  target_link_libraries(editorl   nanogui ${NANOGUI_EXTRA_LIBS})

  file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

if(NANOGUI_BUILD_HONSU)
  include_directories("ext/cpp-httplib")
  set(HONSU_OPENSSL_LIBS "")

  if(MSVC)
    include_directories("c:/Program Files/OpenSSL-Win64/include")
    link_directories("c:/Program Files/OpenSSL-Win64/lib")

    list(APPEND HONSU_OPENSSL_LIBS libssl libcrypto)
  elseif (APPLE)  
    include_directories("/usr/local/opt/openssl/include")
    link_directories("/usr/local/opt/openssl/lib")
    list(APPEND HONSU_OPENSSL_LIBS ssl crypto)
  endif()
  add_executable(honsu 
    honsu/honsu_main.cpp )

  if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  endif()
  target_link_libraries(honsu nanogui ${NANOGUI_EXTRA_LIBS} ${HONSU_OPENSSL_LIBS})
  set_property(TARGET honsu PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  # file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY resources/ca-bundle.crt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

if(NANOGUI_BUILD_EXAMPLE)
  add_executable(example1      examples/example1.cpp)
  add_executable(example2      examples/example2.cpp)
  add_executable(example3      examples/example3.cpp)
  add_executable(example4      examples/example4.cpp)
  add_executable(example_icons examples/example_icons.cpp)
  target_link_libraries(example1      nanogui ${NANOGUI_EXTRA_LIBS})
  target_link_libraries(example2      nanogui ${NANOGUI_EXTRA_LIBS})
  target_link_libraries(example3      nanogui ${NANOGUI_EXTRA_LIBS})
  target_link_libraries(example4      nanogui ${NANOGUI_EXTRA_LIBS})
  target_link_libraries(example_icons nanogui ${NANOGUI_EXTRA_LIBS})

  # Copy icons for example application
  file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

if (NANOGUI_BUILD_PYTHON)
  message(STATUS "NanoGUI: building with Python plugin")
  # Detect Python

  if (NOT ${NANOGUI_PYTHON_VERSION} EQUAL "")
    message(STATUS "NanoGUI: building with Python plugin, manual options set")
  else()  
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11/tools")

    set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
    find_package(PythonLibsNew ${NANOGUI_PYTHON_VERSION})
  
    message(STATUS "PYTHON_VALUES: " "${_PYTHON_VALUES}")

    if (NOT PYTHONLIBS_FOUND)
      # Python not found -- disable the plugin
      # set(NANOGUI_BUILD_PYTHON OFF CACHE BOOL "Build a Python plugin for NanoGUI?" FORCE)
      message(WARNING "NanoGUI: not building the Python plugin!")
    else()
      set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIR})
    endif()    
  endif()
  # Try to autodetect Python (can be overridden manually if needed)

  include_directories(${PYTHON_INCLUDE_DIRS})
  message(STATUS "Python include dir: " ${PYTHON_INCLUDE_DIR})
  message(STATUS "Python library: " ${PYTHON_LIBRARIES})

  # Need PIC code in libnanogui even when compiled as a static library
  set_target_properties(nanogui-obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
  
  set(PYTHON_BACKEND_SOURCES "")  
  if (NANOGUI_VULKAN_BACKEND)
    set_target_properties(glfw_objects PROPERTIES POSITION_INDEPENDENT_CODE ON)
  endif()  

  if (NANOGUI_GLFW_BACKEND)  
    set_target_properties(glfw_objects PROPERTIES POSITION_INDEPENDENT_CODE ON)

    list(APPEND PYTHON_BACKEND_SOURCES 
         python/glcanvas.cpp 
         python/glutil.cpp)
  endif()
 

  include_directories("ext/pybind11/include" ${PYTHON_INCLUDE_DIRS})
  add_library(nanogui-python-obj OBJECT
    python/main.cpp
    python/constants_ui.cpp
    python/constants_entypo.cpp
    python/widget.cpp
    python/layout.cpp
    python/basics.cpp
    python/button.cpp
    python/tabs.cpp
    python/textbox.cpp
    python/theme.cpp
    
    python/formhelper.cpp
    python/misc.cpp

    python/nanovg.cpp
    python/python.h python/py_doc.h
    ${PYTHON_BACKEND_SOURCES}
    ${LIBNANOGUI_PYTHON_EXTRA_SOURCE})

  add_library(nanogui-python SHARED $<TARGET_OBJECTS:nanogui-python-obj>)
  set_property(TARGET nanogui-python-obj PROPERTY POSITION_INDEPENDENT_CODE ON)
  set_target_properties(nanogui-python PROPERTIES OUTPUT_NAME "nanogui")
  target_link_libraries(nanogui-python nanogui ${NANOGUI_EXTRA_LIBS} ${Python_LIBRARIES})

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

  # Make sure platform specific code gets kept in py_doc.h (specifically __doc_nanogui_chdir_to_bundle_parent)
  list(APPEND MKDOC_COMPILE_DEFINITIONS "DOXYGEN_DOCUMENTATION_BUILD")
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
