# python/example3.py -- running NanoGUI in detached mode
# (contributed by Dmitriy Morozov)
#
# NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
# The widget drawing code is based on the NanoVG demo application
# by Mikko Mononen.
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE.txt file.

import nanogui
from nanogui import *
import time


class TestApp(Screen):
    def __init__(self):
        super(TestApp, self).__init__((190, 170), "NanoGUI Test")

        window = Window(self, "Detached mode")
        window.setPosition((15, 15))
        window.setLayout(GroupLayout())

        Label(window, "Push buttons", "sans-bold")
        b = Button(window, "Plain button")

        def cb():
            print("pushed!")
        b.setCallback(cb)

        b = Button(window, "Quit")

        def cb2():
            self.setVisible(False)
        b.setCallback(cb2)

        self.performLayout()


if __name__ == "__main__":
    nanogui.init()
    test = TestApp()
    test.drawAll()
    test.setVisible(True)

    print("Launching detached mainloop")
    h = nanogui.mainloop(detach=test)

    print("Back in Python context")
    for i in range(10):
        print(i)
        time.sleep(1)
        if not nanogui.active():
            break

    h.join()
    nanogui.shutdown()
