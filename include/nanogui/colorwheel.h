/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/colorwheel.h
 *
 * \brief Fancy analog widget to select a color value.  This widget was
 *        contributed by Dmitriy Morozov.
 */

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class ColorWheel colorwheel.h nanogui/colorwheel.h
 *
 * \brief Fancy analog widget to select a color value.  This widget was
 *        contributed by Dmitriy Morozov.
 */
class NANOGUI_EXPORT ColorWheel : public Widget {
public:
    RTTI_CLASS_UID("CLWH")
    RTTI_DECLARE_INFO(ColorWheel)

    /**
     * Adds a ColorWheel to the specified parent.
     *
     * \param parent
     *     The Widget to add this ColorWheel to.
     *
     * \param color
     *     The initial color of the ColorWheel (default: Red).
     */
    ColorWheel(Widget *parent, const Color& color = Color(1.0f, 0.0f, 0.0f, 1.0f));

    /// The callback to execute when a user changes the ColorWheel value.
    std::function<void(const Color &)> callback() const { return mCallback; }

    /// Sets the callback to execute when a user changes the ColorWheel value.
    void setCallback(const std::function<void(const Color &)> &callback) { mCallback = callback; }

    /// The current Color this ColorWheel has selected.
    Color color() const;

    /// Sets the current Color this ColorWheel has selected.
    void setColor(const Color& color);

    /// The preferred size of this ColorWheel.
    virtual Vector2i preferredSize(NVGcontext *ctx) const override;

    /// Draws the ColorWheel.
    virtual void draw(NVGcontext *ctx) override;

    /// Handles mouse button click events for the ColorWheel.
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

    /// Handles mouse drag events for the ColorWheel.
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;

    /// Saves the current state of this ColorWheel to the specified Serializer.
    virtual void save(Serializer &s) const override;

    /// Sets the state of this ColorWheel using the specified Serializer.
    virtual bool load(Serializer &s) override;

private:
    // Used to describe where the mouse is interacting
    enum Region {
        None = 0,
        InnerTriangle = 1,
        OuterCircle = 2,
        Both = 3
    };

    // Converts a specified hue (with saturation = value = 1) to RGB space.
    Color hue2rgb(float h) const;

    // Manipulates the positioning of the different regions of the ColorWheel.
    Region adjustPosition(const Vector2i &p, Region consideredRegions = Both);

protected:
    /// The current Hue in the HSV color model.
    float mHue;

    /**
     * The implicit Value component of the HSV color model.  See implementation
     * \ref nanogui::ColorWheel::color for its usage.  Valid values are in the
     * range ``[0, 1]``.
     */
    float mWhite;

    /**
     * The implicit Saturation component of the HSV color model.  See implementation
     * \ref nanogui::ColorWheel::color for its usage.  Valid values are in the
     * range ``[0, 1]``.
     */
    float mBlack;

    /// The current region the mouse is interacting with.
    Region mDragRegion;

    /// The current callback to execute when the color value has changed.
    std::function<void(const Color &)> mCallback;

public:
};

NAMESPACE_END(nanogui)
