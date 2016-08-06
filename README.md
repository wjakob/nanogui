# NanoGUI

[![Build Status](https://travis-ci.org/wjakob/nanogui.svg?branch=master)](https://travis-ci.org/wjakob/nanogui)
[![Build status](https://ci.appveyor.com/api/projects/status/m8h3uyvdb4ej2i02/branch/master?svg=true)](https://ci.appveyor.com/project/wjakob/nanogui/branch/master)

NanoGUI is a minimalistic cross-platform widget library for OpenGL 3.x.
It supports automatic layout generation, stateful C++11 lambdas callbacks,
a variety of useful widget types and Retina-capable rendering on Apple devices
thanks to [NanoVG](https://github.com/memononen/NanoVG) by Mikko Mononen.
Python bindings of all functionality are provided using
[pybind11](https://github.com/wjakob/pybind11).

## Example screenshot
![Screenshot](https://github.com/wjakob/nanogui/raw/master/resources/screenshot.png "Screenshot")

## Description
NanoGUI builds on [GLFW](http://www.glfw.org/) for cross-platform OpenGL context
creation and event handling, [GLAD](https://github.com/Dav1dde/glad) to use OpenGL
3.x Windows, [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) for
basic vector types, and [NanoVG](https://github.com/memononen/NanoVG) to draw
2D primitives.

Note that the dependency library NanoVG already includes some basic example
code to draw good-looking static widgets; what NanoGUI does is to flesh it
out into a complete GUI toolkit with event handling, layout generation, etc.

NanoGUI currently works on Mac OS X (Clang) Linux (GCC or Clang) and Windows
(Visual Studio ≥ 2015); it requires a recent C++11 capable compiler. All
dependencies are jointly built using a CMake-based build system.

## Creating widgets
NanoGUI makes it easy to instantiate widgets, set layout constraints, and
register event callbacks using high-level C++11 code. For instance, the
following two lines from the included example application add a new button to
an existing window `window` and register an event callback.
```C++
Button *b = new Button(window, "Plain button");
b->setCallback([] { cout << "pushed!" << endl; });
```

The following lines from the example application create the coupled
slider and text box on the bottom of the second window (see the screenshot).
```C++
/* Create an empty panel with a horizontal layout */
Widget *panel = new Widget(window);
panel->setLayout(new BoxLayout(BoxLayout::Horizontal, BoxLayout::Middle, 0, 20));

/* Add a slider and set defaults */
Slider *slider = new Slider(panel);
slider->setValue(0.5f);
slider->setFixedWidth(80);

/* Add a textbox and set defaults */
TextBox *textBox = new TextBox(panel);
textBox->setFixedSize(Vector2i(60, 25));
textBox->setValue("50");
textBox->setUnits("%");

/* Propagate slider changes to the text box */
slider->setCallback([textBox](float value) {
    textBox->setValue(std::to_string((int) (value * 100)));
});
```

The Python version of this same piece of code looks like this:
```Python
# Create an empty panel with a horizontal layout
panel = Widget(window)
panel.setLayout(BoxLayout(BoxLayout.Horizontal, BoxLayout.Middle, 0, 20))

# Add a slider and set defaults
slider = Slider(panel)
slider.setValue(0.5f)
slider.setFixedWidth(80)

# Add a textbox and set defaults
textBox = TextBox(panel)
textBox.setFixedSize(Vector2i(60, 25))
textBox.setValue("50")
textBox.setUnits("%")

# Propagate slider changes to the text box
def cb(value):
    textBox.setValue("%i" % int(value * 100))
slider.setCallback(cb)
```

## "Simple mode"

Christian Schüller contributed a convenience class that makes it possible to
create AntTweakBar-style variable manipulators using just a few lines of code.
For instance, the source code below was used to create the following example
application.

![Screenshot](https://github.com/wjakob/nanogui/raw/master/resources/screenshot2.png "Screenshot")


```C++
/// dvar, bar, strvar, etc. are double/bool/string/.. variables

FormHelper *gui = new FormHelper(screen);
ref<Window> window = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
gui->addGroup("Basic types");
gui->addVariable("bool", bvar);
gui->addVariable("string", strvar);

gui->addGroup("Validating fields");
gui->addVariable("int", ivar);
gui->addVariable("float", fvar);
gui->addVariable("double", dvar);

gui->addGroup("Complex types");
gui->addVariable("Enumeration", enumval, enabled)
   ->setItems({"Item 1", "Item 2", "Item 3"});
gui->addVariable("Color", colval);

gui->addGroup("Other widgets");
gui->addButton("A button", [](){ std::cout << "Button pressed." << std::endl; });

screen->setVisible(true);
screen->performLayout();
window->center();
```

## Compiling
Clone the repository and all dependencies (with `git clone --recursive`),
run CMake to generate Makefiles or CMake/Visual Studio project files, and
the rest should just work automatically.

On Debian/Ubuntu, make sure that you have installed the following packages
```bash
$ apt-get install cmake xorg-dev libglu1-mesa-dev
```
To also get the Python bindings, you'll need to run
```bash
$ apt-get install python-dev
```
### License

nanogui is provided under a BSD-style license that can be found in the
``LICENSE.txt`` file. By using, distributing, or contributing to this project,
you agree to the terms and conditions of this license.
