# python/example_icons.py -- Python version of an example application that shows
# all available Entypo icons as they would appear in NanoGUI itself.  For a C++
# implementation, see '../src/example_icons.cpp'.
#
# NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
# The widget drawing code is based on the NanoVG demo application
# by Mikko Mononen.
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE.txt file.

# Developer note: need to make a change to this file?
# Please raise an Issue on GitHub describing what needs to change.  This file
# was generated, so the scripts that generated it need to update as well.

import gc

import nanogui
from nanogui import Screen, Window, Widget, GridLayout, VScrollPanel, Button, TextBox, BoxLayout, Orientation
from nanogui import entypo

# Return false essentially makes it not possible to actually edit this text
# box, but keeping it editable=true allows selection for copy-paste.  If the
# text box is not editable, then the user cannot highlight it.
def textbox_callback(val):
    return False

if __name__ == "__main__":
    nanogui.init()

    width      = 1000
    half_width = width // 2
    height     = 800

    # create a fixed size screen with one window
    screen = Screen((width, height), "NanoGUI Icons", False)
    window = Window(screen, "")
    window.setPosition((0, 0))
    window.setFixedSize((width, height))

    # attach a vertical scroll panel
    vscroll = VScrollPanel(window)
    vscroll.setFixedSize((width, height))

    # vscroll should only have *ONE* child. this is what `wrapper` is for
    wrapper = Widget(vscroll)
    wrapper.setFixedSize((width, height))
    wrapper.setLayout(GridLayout())  # defaults: 2 columns

    # NOTE: don't __dict__ crawl in real code!
    # this is just because it's more convenient to do this for enumerating all
    # of the icons -- see cpp example for alternative...
    for key in entypo.__dict__.keys():
        if key.startswith("ICON_"):
            wrapper_icon = Widget(wrapper)
            wrapper_icon.setLayout(BoxLayout(Orientation.Horizontal))
            b = Button(wrapper_icon, "", entypo.__dict__[key])
            b.setFixedWidth(40)
            text = TextBox(wrapper_icon, "entypo.{0}".format(key))
            text.setDefaultValue("entypo.{0}".format(key))
            text.setEditable(True) # allow copy-paste
            text.setCallback(textbox_callback) # disable changes
            text.setFont("mono-bold")
            text.setFixedWidth(half_width - 40)

    screen.performLayout()
    screen.drawAll()
    screen.setVisible(True)

    nanogui.mainloop()

    del screen
    gc.collect()

    nanogui.shutdown()
