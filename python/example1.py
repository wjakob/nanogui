# python/example1.py -- Python version of an example application that shows
# how to use the various widget classes. For a C++ implementation, see
# '../src/example1.cpp'.
#
# NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
# The widget drawing code is based on the NanoVG demo application
# by Mikko Mononen.
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE.txt file.

import nanogui
import math
import time
import gc

from nanogui import Color, ColorPicker, Screen, Window, GroupLayout, BoxLayout, \
                    ToolButton, Label, Button, Widget, \
                    Popup, PopupButton, CheckBox, MessageDialog, VScrollPanel, \
                    ImagePanel, ImageView, ComboBox, ProgressBar, Slider, \
                    TextBox, ColorWheel, Graph, GridLayout, \
                    Alignment, Orientation, TabWidget, IntBox, GLShader

from nanogui import gl, glfw, entypo

# A simple counter, used for dynamic tab creation with TabWidget callback
counter = 1

class TestApp(Screen):
    def __init__(self):
        super(TestApp, self).__init__((1024, 768), "NanoGUI Test")

        window = Window(self, "Button demo")
        window.setPosition((15, 15))
        window.setLayout(GroupLayout())

        Label(window, "Push buttons", "sans-bold")
        b = Button(window, "Plain button")

        def cb():
            print("pushed!")
        b.setCallback(cb)

        b = Button(window, "Styled", entypo.ICON_ROCKET)
        b.setBackgroundColor(Color(0, 0, 1.0, 0.1))
        b.setCallback(cb)

        Label(window, "Toggle buttons", "sans-bold")
        b = Button(window, "Toggle me")
        b.setFlags(Button.Flags.ToggleButton)

        def change_cb(state):
            print("Toggle button state: %s" % str(state))
        b.setChangeCallback(change_cb)

        Label(window, "Radio buttons", "sans-bold")
        b = Button(window, "Radio button 1")
        b.setFlags(Button.Flags.RadioButton)
        b = Button(window, "Radio button 2")
        b.setFlags(Button.Flags.RadioButton)

        Label(window, "A tool palette", "sans-bold")
        tools = Widget(window)
        tools.setLayout(BoxLayout(Orientation.Horizontal,
                                  Alignment.Middle, 0, 6))

        ToolButton(tools, entypo.ICON_CLOUD)
        ToolButton(tools, entypo.ICON_CONTROLLER_FAST_FORWARD)
        ToolButton(tools, entypo.ICON_COMPASS)
        ToolButton(tools, entypo.ICON_INSTALL)

        Label(window, "Popup buttons", "sans-bold")
        popupBtn = PopupButton(window, "Popup", entypo.ICON_EXPORT)
        popup = popupBtn.popup()
        popup.setLayout(GroupLayout())
        Label(popup, "Arbitrary widgets can be placed here")
        CheckBox(popup, "A check box")
        # popup right
        popupBtn = PopupButton(popup, "Recursive popup", entypo.ICON_FLASH)
        popupRight = popupBtn.popup()
        popupRight.setLayout(GroupLayout())
        CheckBox(popupRight, "Another check box")
        # popup left
        popupBtn = PopupButton(popup, "Recursive popup", entypo.ICON_FLASH)
        popupBtn.setSide(Popup.Side.Left)
        popupLeft = popupBtn.popup()
        popupLeft.setLayout(GroupLayout())
        CheckBox(popupLeft, "Another check box");

        window = Window(self, "Basic widgets")
        window.setPosition((200, 15))
        window.setLayout(GroupLayout())

        Label(window, "Message dialog", "sans-bold")
        tools = Widget(window)
        tools.setLayout(BoxLayout(Orientation.Horizontal,
                                  Alignment.Middle, 0, 6))

        def cb2(result):
            print("Dialog result: %i" % result)

        b = Button(tools, "Info")

        def cb():
            dlg = MessageDialog(self, MessageDialog.Type.Information, "Title",
                                "This is an information message")
            dlg.setCallback(cb2)
        b.setCallback(cb)

        b = Button(tools, "Warn")

        def cb():
            dlg = MessageDialog(self, MessageDialog.Type.Warning, "Title",
                                "This is a warning message")
            dlg.setCallback(cb2)
        b.setCallback(cb)

        b = Button(tools, "Ask")

        def cb():
            dlg = MessageDialog(self, MessageDialog.Type.Warning, "Title",
                                "This is a question message", "Yes", "No",
                                True)
            dlg.setCallback(cb2)
        b.setCallback(cb)

        import os
        import sys
        os.chdir(sys.path[0])
        try:
            icons = nanogui.loadImageDirectory(self.nvgContext(), "icons")
        except:
            try:
                icons = nanogui.loadImageDirectory(self.nvgContext(), "../icons")
            except:
                icons = nanogui.loadImageDirectory(self.nvgContext(), "../resources/icons")


        Label(window, "Image panel & scroll panel", "sans-bold")
        imagePanelBtn = PopupButton(window, "Image Panel")
        imagePanelBtn.setIcon(entypo.ICON_FOLDER)
        popup = imagePanelBtn.popup()
        vscroll = VScrollPanel(popup)
        imgPanel = ImagePanel(vscroll)
        imgPanel.setImages(icons)
        popup.setFixedSize((245, 150))

        img_window = Window(self, "Selected image")
        img_window.setPosition((710, 15))
        img_window.setLayout(GroupLayout())

        imgView = ImageView(img_window, icons[0][0])

        def cb(i):
            if i < len(icons):
                print("Selected item %i" % i)
                imgView.bindImage(icons[i][0])
        imgPanel.setCallback(cb)

        imgView.setGridThreshold(3)

        Label(window, "File dialog", "sans-bold")
        tools = Widget(window)
        tools.setLayout(BoxLayout(Orientation.Horizontal,
                                  Alignment.Middle, 0, 6))
        b = Button(tools, "Open")
        valid = [("png", "Portable Network Graphics"), ("txt", "Text file")]

        def cb():
            result = nanogui.file_dialog(valid, False)
            print("File dialog result = %s" % result)

        b.setCallback(cb)
        b = Button(tools, "Save")

        def cb():
            result = nanogui.file_dialog(valid, True)
            print("File dialog result = %s" % result)

        b.setCallback(cb)

        Label(window, "Combo box", "sans-bold")
        ComboBox(window, ["Combo box item 1", "Combo box item 2",
                          "Combo box item 3"])
        Label(window, "Check box", "sans-bold")

        def cb(state):
            print("Check box 1 state: %s" % state)
        chb = CheckBox(window, "Flag 1", cb)
        chb.setChecked(True)

        def cb(state):
            print("Check box 2 state: %s" % state)
        CheckBox(window, "Flag 2", cb)

        Label(window, "Progress bar", "sans-bold")
        self.progress = ProgressBar(window)

        Label(window, "Slider and text box", "sans-bold")

        panel = Widget(window)
        panel.setLayout(BoxLayout(Orientation.Horizontal,
                                  Alignment.Middle, 0, 20))

        slider = Slider(panel)
        slider.setValue(0.5)
        slider.setFixedWidth(80)

        textBox = TextBox(panel)
        textBox.setFixedSize((60, 25))
        textBox.setValue("50")
        textBox.setUnits("%")
        textBox.setFontSize(20)
        textBox.setAlignment(TextBox.Alignment.Right)

        def cb(value):
            textBox.setValue("%i" % int(value * 100))
        slider.setCallback(cb)

        def cb(value):
            print("Final slider value: %i" % int(value * 100))
        slider.setFinalCallback(cb)

        window = Window(self, "Misc. widgets")
        window.setPosition((425, 15))
        window.setLayout(GroupLayout())

        tabWidget = TabWidget(window)
        layer = tabWidget.createTab("Color Wheel")
        layer.setLayout(GroupLayout())

        Label(layer, "Color wheel widget", "sans-bold")
        ColorWheel(layer)

        layer = tabWidget.createTab("Function Graph")
        layer.setLayout(GroupLayout())
        Label(layer, "Function graph widget", "sans-bold")

        graph = Graph(layer, "Some function")
        graph.setHeader("E = 2.35e-3")
        graph.setFooter("Iteration 89")
        values = [0.5 * (0.5 * math.sin(i / 10.0) +
                         0.5 * math.cos(i / 23.0) + 1)
                  for i in range(100)]
        graph.setValues(values)
        tabWidget.setActiveTab(0)

        # Dummy tab used to represent the last tab button.
        tabWidget.createTab("+")

        def tab_cb(index):
            if index == (tabWidget.tabCount()-1):
                global counter
                # When the "+" tab has been clicked, simply add a new tab.
                tabName  = "Dynamic {0}".format(counter)
                layerDyn = tabWidget.createTab(index, tabName)
                layerDyn.setLayout(GroupLayout())
                Label(layerDyn, "Function graph widget", "sans-bold")
                graphDyn = Graph(layerDyn, "Dynamic function")

                graphDyn.setHeader("E = 2.35e-3")
                graphDyn.setFooter("Iteration {0}".format(index*counter))
                valuesDyn = [0.5 * abs((0.5 * math.sin(i / 10.0 + counter)) +
                                       (0.5 * math.cos(i / 23.0 + 1 + counter)))
                             for i in range(100)]
                graphDyn.setValues(valuesDyn)
                counter += 1
                # We must invoke perform layout from the screen instance to keep everything in order.
                # This is essential when creating tabs dynamically.
                self.performLayout()
                # Ensure that the newly added header is visible on screen
                tabWidget.ensureTabVisible(index)

        tabWidget.setCallback(tab_cb)
        tabWidget.setActiveTab(0);

        window = Window(self, "Grid of small widgets")
        window.setPosition((425, 300))
        layout = GridLayout(Orientation.Horizontal, 2,
                            Alignment.Middle, 15, 5)
        layout.setColAlignment(
            [Alignment.Maximum, Alignment.Fill])
        layout.setSpacing(0, 10)
        window.setLayout(layout)

        Label(window, "Floating point :", "sans-bold")
        floatBox = TextBox(window)
        floatBox.setEditable(True)
        floatBox.setFixedSize((100, 20))
        floatBox.setValue("50")
        floatBox.setUnits("GiB")
        floatBox.setDefaultValue("0.0")
        floatBox.setFontSize(16)
        floatBox.setFormat("[-]?[0-9]*\\.?[0-9]+")

        Label(window, "Positive integer :", "sans-bold")
        intBox = IntBox(window)
        intBox.setEditable(True)
        intBox.setFixedSize((100, 20))
        intBox.setValue(50)
        intBox.setUnits("Mhz")
        intBox.setDefaultValue("0")
        intBox.setFontSize(16)
        intBox.setFormat("[1-9][0-9]*")
        intBox.setSpinnable(True)
        intBox.setMinValue(1)
        intBox.setValueIncrement(2)

        Label(window, "Checkbox :", "sans-bold")

        cb = CheckBox(window, "Check me")
        cb.setFontSize(16)
        cb.setChecked(True)

        Label(window, "Combo box :", "sans-bold")
        cobo = ComboBox(window, ["Item 1", "Item 2", "Item 3"])
        cobo.setFontSize(16)
        cobo.setFixedSize((100, 20))

        Label(window, "Color picker :", "sans-bold");
        cp = ColorPicker(window, Color(255, 120, 0, 255));
        cp.setFixedSize((100, 20));

        def cp_final_cb(color):
            print(
                "ColorPicker Final Callback: [{0}, {1}, {2}, {3}]".format(color.r,
                                                                          color.g,
                                                                          color.b,
                                                                          color.w)
            )

        cp.setFinalCallback(cp_final_cb)

        # setup a fast callback for the color picker widget on a new window
        # for demonstrative purposes
        window = Window(self, "Color Picker Fast Callback")
        window.setPosition((425, 300))
        layout = GridLayout(Orientation.Horizontal, 2,
                            Alignment.Middle, 15, 5)
        layout.setColAlignment(
            [Alignment.Maximum, Alignment.Fill])
        layout.setSpacing(0, 10)
        window.setLayout(layout)
        window.setPosition((425, 500))
        Label(window, "Combined: ");
        b = Button(window, "ColorWheel", entypo.ICON_500PX)
        Label(window, "Red: ")
        redIntBox = IntBox(window)
        redIntBox.setEditable(False)
        Label(window, "Green: ")
        greenIntBox = IntBox(window)
        greenIntBox.setEditable(False)
        Label(window, "Blue: ")
        blueIntBox = IntBox(window)
        blueIntBox.setEditable(False)
        Label(window, "Alpha: ")
        alphaIntBox = IntBox(window)

        def cp_fast_cb(color):
            b.setBackgroundColor(color)
            b.setTextColor(color.contrastingColor())
            red = int(color.r * 255.0)
            redIntBox.setValue(red)
            green = int(color.g * 255.0)
            greenIntBox.setValue(green)
            blue = int(color.b * 255.0)
            blueIntBox.setValue(blue)
            alpha = int(color.w * 255.0)
            alphaIntBox.setValue(alpha)

        cp.setCallback(cp_fast_cb)

        self.performLayout()

        try:
            import numpy as np

            self.shader = GLShader()
            self.shader.init(
                # An identifying name
                "a_simple_shader",

                # Vertex shader
                """#version 330
                uniform mat4 modelViewProj;
                in vec3 position;
                void main() {
                    gl_Position = modelViewProj * vec4(position, 1.0);
                }""",

                """#version 330
                out vec4 color;
                uniform float intensity;
                void main() {
                    color = vec4(vec3(intensity), 1.0);
                }"""
            )

            # Draw 2 triangles
            indices = np.array(
                [[0, 2], [1, 3], [2, 0]],
                dtype=np.int32)

            positions = np.array(
                [[-1, 1, 1, -1],
                 [-1, -1, 1, 1],
                 [0, 0, 0, 0]],
                dtype=np.float32)

            self.shader.bind()
            self.shader.uploadIndices(indices)
            self.shader.uploadAttrib("position", positions)
            self.shader.setUniform("intensity", 0.5)
        except ImportError:
            self.shader = None
            pass

    def draw(self, ctx):
        self.progress.setValue(math.fmod(time.time() / 10, 1))
        super(TestApp, self).draw(ctx)

    def drawContents(self):
        if self.shader is not None:
            import numpy as np
            self.shader.bind()

            angle = time.time()

            mvp = np.array(
                [[np.cos(angle), -np.sin(angle), 0, 0],
                 [np.sin(angle), np.cos(angle), 0, 0],
                 [0, 0, 1, 0],
                 [0, 0, 0, 1]],
                dtype=np.float32
            )

            mvp[0:3, 0:3] *= 0.25
            mvp[0, :] *= self.size()[1] / self.size()[0]

            self.shader.setUniform("modelViewProj", mvp)
            self.shader.drawIndexed(gl.TRIANGLES, 0, 2)
        super(TestApp, self).drawContents()

    def keyboardEvent(self, key, scancode, action, modifiers):
        if super(TestApp, self).keyboardEvent(key, scancode,
                                              action, modifiers):
            return True
        if key == glfw.KEY_ESCAPE and action == glfw.PRESS:
            self.setVisible(False)
            return True
        return False

if __name__ == "__main__":
    nanogui.init()
    test = TestApp()
    test.drawAll()
    test.setVisible(True)
    nanogui.mainloop()
    del test
    gc.collect()
    nanogui.shutdown()
