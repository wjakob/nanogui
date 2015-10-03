# This is the Python version of the example application
# For a C++ implementation, see 'src/example.cpp'

import nanogui
import math

from nanogui import Screen, FormHelper, Vector2i

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

screen = Screen(Vector2i(500, 700), "NanoGUI test")
gui = FormHelper(screen)
window = gui.addWindow(Vector2i(10, 10), "Form helper example")

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
nanogui.shutdown()
