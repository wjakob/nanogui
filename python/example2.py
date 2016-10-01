# python/example2.py -- Python version of an example application that shows
# how to use the form helper class. For a C++ implementation, see
# '../src/example2.cpp'.
#
# NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
# The widget drawing code is based on the NanoVG demo application
# by Mikko Mononen.
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE.txt file.

import nanogui
import math
import gc

from nanogui import Screen, FormHelper

bvar = True
ivar = 12345678
dvar = math.pi
strvar = "A string"
enumvar = 1
colvar = nanogui.Color(.5, .5, .7, 1)


def make_accessors(name):
    def setter(value):
        globals()[name] = value

    def getter():
        return globals()[name]
    return setter, getter

nanogui.init()

use_gl_4_1 = False # Set to True to create an OpenGL 4.1 context.
if use_gl_4_1:
    # NanoGUI presents many options for you to utilize at your discretion.
    # See include/nanogui/screen.h for what all of the options are.
    screen = Screen((500, 700), "NanoGUI test [GL 4.1]", glMajor=4, glMinor=1)
else:
    screen = Screen((500, 700), "NanoGUI test")

gui = FormHelper(screen)
window = gui.addWindow((10, 10), "Form helper example")

gui.addGroup("Basic types")
gui.addBoolVariable("bool", *make_accessors("bvar"))
gui.addStringVariable("string", *make_accessors("strvar"))

gui.addGroup("Validating fields")
gui.addIntVariable("int", *make_accessors("ivar"))
gui.addDoubleVariable("double", *make_accessors("dvar"))

gui.addGroup("Complex types")
gui.addEnumVariable("Enumeration", *make_accessors("enumvar")) \
   .setItems(["Item 1", "Item 2", "Item 3"])
gui.addColorVariable("Color", *make_accessors("colvar"))

gui.addGroup("Other widgets")


def cb():
    print("Button pressed.")
gui.addButton("A button", cb)

screen.setVisible(True)
screen.performLayout()
window.center()

nanogui.mainloop()
screen = gui = window = None
gc.collect()
nanogui.shutdown()
