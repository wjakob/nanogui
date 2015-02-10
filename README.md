# NanoGUI

NanoGUI is a a minimalistic cross-platform widget library for OpenGL 3.x.
It supports automatic layout generation, stateful C++11 lambdas callbacks,
a variety of useful widget types and Retina-capable rendering on Apple devices
thanks to [NanoVG](https://github.com/memononen/NanoVG) by Mikko Mononen.

## Example screenshot
![Screenshot](https://github.com/wjakob/nanogui/raw/master/resources/screenshot.png "Screenshot")

## Description
NanoGUI builds on [GLFW](http://www.glfw.org/) for cross-platform OpenGL context
creation and event handling, [GLEW](http://glew.sourceforge.net/) to use OpenGL
3.x Windows, [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) for
basic vector types, and [NanoVG](https://github.com/memononen/NanoVG) to draw
2D primitives.

Note that the depencency library NanoVG already includes some basic example
code to draw good-looking static widgets; what NanoGUI does is to flesh it
out into a complete GUI toolkit with event handling, layout generation, etc.

NanoGUI currently works on Mac OS X (Clang) Linux (GCC or Clang) and Windows
(Visual Studio ≥ 2013); it requires a recent C++11 capable compiler. All
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
textBox->setFixedSize(Vector2i(90, 25));
textBox->setValue("50");
textBox->setUnits("%");
textBox->setFixedWidth(60);

/* Propagate slider changes to the text box */
slider->setCallback([textBox](float value) {
	textBox->setValue(std::to_string((int) (value * 100)));
});
```

## Compiling
Clone the repository and all dependencies (with `git clone --recursive`),
run CMake to generate Makefiles or CMake/Visual Studio project files, and
the rest should just work automatically.

