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

from nanogui import Color, Screen, Window, GroupLayout, BoxLayout, \
                    ToolButton, Vector2i, Label, Button, Widget, \
                    PopupButton, CheckBox, MessageDialog, VScrollPanel, \
                    ImagePanel, ImageView, ComboBox, ProgressBar, Slider, \
                    TextBox, ColorWheel, Graph, VectorXf, GridLayout, \
                    Alignment, Orientation

from nanogui import glfw, entypo


class TestApp(Screen):
    def __init__(self):
        super(TestApp, self).__init__(Vector2i(1024, 768), "NanoGUI Test")

        window = Window(self, "Button demo")
        window.setPosition(Vector2i(15, 15))
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
        ToolButton(tools, entypo.ICON_FF)
        ToolButton(tools, entypo.ICON_COMPASS)
        ToolButton(tools, entypo.ICON_INSTALL)

        Label(window, "Popup buttons", "sans-bold")
        popupBtn = PopupButton(window, "Popup", entypo.ICON_EXPORT)
        popup = popupBtn.popup()
        popup.setLayout(GroupLayout())
        Label(popup, "Arbitrary widgets can be placed here")
        CheckBox(popup, "A check box")
        popupBtn = PopupButton(popup, "Recursive popup", entypo.ICON_FLASH)
        popup = popupBtn.popup()
        popup.setLayout(GroupLayout())
        CheckBox(popup, "Another check box")

        window = Window(self, "Basic widgets")
        window.setPosition(Vector2i(200, 15))
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
            icons = nanogui.loadImageDirectory(self.nvgContext(), "../icons")

        Label(window, "Image panel & scroll panel", "sans-bold")
        imagePanelBtn = PopupButton(window, "Image Panel")
        imagePanelBtn.setIcon(entypo.ICON_FOLDER)
        popup = imagePanelBtn.popup()
        vscroll = VScrollPanel(popup)
        imgPanel = ImagePanel(vscroll)
        imgPanel.setImages(icons)
        popup.setFixedSize(Vector2i(245, 150))

        img_window = Window(self, "Selected image")
        img_window.setPosition(Vector2i(675, 15))
        img_window.setLayout(GroupLayout());

        img = ImageView(img_window)
        img.setPolicy(ImageView.SizePolicy.Expand)
        img.setFixedSize(Vector2i(300, 300))
        img.setImage(icons[0][0])

        def cb(i):
            print("Selected item %i" % i)
            img.setImage(icons[i][0])
        imgPanel.setCallback(cb)

        def cb(s):
            if s:
                img.setPolicy(ImageView.SizePolicy.Expand)
            else:
                img.setPolicy(ImageView.SizePolicy.Fixed)
        img_cb = CheckBox(img_window, "Expand", cb)
        img_cb.setChecked(True)

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
        textBox.setFixedSize(Vector2i(60, 25))
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
        window.setPosition(Vector2i(425, 15))
        window.setLayout(GroupLayout())
        Label(window, "Color wheel", "sans-bold")
        ColorWheel(window)
        Label(window, "Function graph", "sans-bold")
        graph = Graph(window, "Some function")
        graph.setHeader("E = 2.35e-3")
        graph.setFooter("Iteration 89")
        values = VectorXf(100)
        for i in range(100):
            values[i] = 0.5 * (0.5 * math.sin(i / 10.0) +
                               0.5 * math.cos(i / 23.0) + 1)
        graph.setValues(values)

        window = Window(self, "Grid of small widgets")
        window.setPosition(Vector2i(425, 288))
        layout = GridLayout(Orientation.Horizontal, 2,
                            Alignment.Middle, 15, 5)
        layout.setColAlignment(
            [Alignment.Maximum, Alignment.Fill])
        layout.setSpacing(0, 10)
        window.setLayout(layout)

        Label(window, "Floating point :", "sans-bold")
        floatBox = TextBox(window)
        floatBox.setEditable(True)
        floatBox.setFixedSize(Vector2i(100, 20))
        floatBox.setValue("50")
        floatBox.setUnits("GiB")
        floatBox.setDefaultValue("0.0")
        floatBox.setFontSize(16)
        floatBox.setFormat("[-]?[0-9]*\\.?[0-9]+")

        Label(window, "Positive integer :", "sans-bold")
        intBox = TextBox(window)
        intBox.setEditable(True)
        intBox.setFixedSize(Vector2i(100, 20))
        intBox.setValue("50")
        intBox.setUnits("Mhz")
        intBox.setDefaultValue("0.0")
        intBox.setFontSize(16)
        intBox.setFormat("[1-9][0-9]*")

        Label(window, "Checkbox :", "sans-bold")

        cb = CheckBox(window, "Check me")
        cb.setFontSize(16)
        cb.setChecked(True)

        Label(window, "Combo box :", "sans-bold")
        cobo = ComboBox(window, ["Item 1", "Item 2", "Item 3"])
        cobo.setFontSize(16)
        cobo.setFixedSize(Vector2i(100, 20))

        Label(window, "Color button :", "sans-bold")
        popupBtn = PopupButton(window, "", 0)
        popupBtn.setBackgroundColor(Color(1, 0.47, 0, 1))
        popupBtn.setFontSize(16)
        popupBtn.setFixedSize(Vector2i(100, 20))
        popup = popupBtn.popup()
        popup.setLayout(GroupLayout())

        colorwheel = ColorWheel(popup)
        colorwheel.setColor(popupBtn.backgroundColor())

        colorBtn = Button(popup, "Pick")
        colorBtn.setFixedSize(Vector2i(100, 25))
        c = colorwheel.color()
        colorBtn.setBackgroundColor(c)

        def cb(value):
            colorBtn.setBackgroundColor(value)

        colorwheel.setCallback(cb)

        def cb(pushed):
            if (pushed):
                popupBtn.setBackgroundColor(colorBtn.backgroundColor())
                popupBtn.setPushed(False)
        colorBtn.setChangeCallback(cb)

        self.performLayout()

    def draw(self, ctx):
        self.progress.setValue(math.fmod(time.time() / 10, 1))
        super(TestApp, self).draw(ctx)

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
    gc.collect()
    nanogui.shutdown()
