/*
    nanogui/imageview.h -- Widget used to display images.

    The image view widget was contributed by Stefan Ivanov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/widget.h>
#include <nanogui/glutil.h>
#include <functional>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT ImageView : public Widget {
public:
    ImageView(Widget* parent, GLuint imageID);
    ~ImageView();

    void bindImage(GLuint imageId);

    GLShader& imageShader() { return mShader; }

    const Vector2i& imageSize() const { return mImageSize; }
    Vector2i scaledImageSize() const { return (mScale * mImageSize.cast<float>()).cast<int>(); }

    const Vector2i& offset() const { return mOffset; }
    void setOffset(const Vector2i& offset) { mOffset = offset; }
    float scale() const { return mScale; }
    void setScale(float scale) { mScale = scale > 0.01f ? scale : 0.01f; }

    bool fixedOffset() const { return mFixedOffset; }
    void setFixedOffset(bool fixedOffset) { mFixedOffset = fixedOffset; }
    bool fixedScale() const { return mFixedScale; }
    void setFixedScale(bool fixedScale) { mFixedScale = fixedScale; }

    float zoomSensitivity() const { return mZoomSensitivity; }
    void setZoomSensitivity(float zoomSensitivity) { mZoomSensitivity = zoomSensitivity; }

    float gridThreshold() const { return mGridThreshold; }
    void setGridThreshold(float gridThreshold) { mGridThreshold = gridThreshold; }

    float pixelInfoThreshold() const { return mPixelInfoThreshold; }
    void setPixelInfoThreshold(float pixelInfoThreshold) { mPixelInfoThreshold = pixelInfoThreshold; }

    void setPixelInfoCallback(const std::function<std::pair<std::string, Color>(const Vector2i&)>& callback) {
        mPixelInfoCallback = callback;
    }
    const std::function<std::pair<std::string, Color>(const Vector2i&)>& pixelInfoCallback() const {
        return mPixelInfoCallback;
    }

    void setFontScaleFactor(float fontScaleFactor) { mFontScaleFactor = fontScaleFactor; }
    float fontScaleFactor() const { return mFontScaleFactor; }

    /// Image transformation functions.

    /// Calculates the image coordinates of the given pixel position on the widget.
    Vector2f imageCoordinateAt(const Vector2i& position) const;

    /**
    *  Calculates the image coordinates of the given pixel position on the widget.
    *  If the position provided corresponds to a coordinate outside the range of
    *  the image, the coordinates are clamped to edges of the image.
    */
    Vector2f clampedImageCoordinateAt(const Vector2i& position) const;

    /// Calculates the position inside the widget for the given image coordinate.
    Vector2i positionForCoordinate(const Vector2f& imageCoordinate) const;

    /**
    *  Modifies the internal state of the image viewer widget so that the pixel at the provided
    *  position on the widget has the specified image coordinate.
    */
    void setImageCoordinateAt(const Vector2i& position, const Vector2f& imageCoordinate);

    /// Centers the image without affecting the scaling factor.
    void center();

    /// Centers and scales the image so that it fits inside the widgets.
    void fit();

    /// Set the scale while keeping the image centered
    void setScaleCentered(float scale);

    /// Moves the offset by the specified amount. Does bound checking.
    void moveOffset(const Vector2i& delta);

    /**
    *  Changes the scale factor by the provided amount modified by the zoom sensitivity member variable.
    *  The scaling occurs such that the image coordinate under the focused pixel remains in
    *  the same position before and after the scaling.
    */
    void zoom(int amount, const Vector2i& focusPixel);

    bool keyboardEvent(int key, int scancode, int action, int modifiers) override;
    bool keyboardCharacterEvent(unsigned int codepoint) override;
    bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;

    /// Function indicating whether the grid is currently visible.
    bool gridVisible() const;

    /// Function indicating whether the pixel information is currently visible.
    bool pixelInfoVisible() const;

    /// Function indicating whether any of the overlays are visible.
    bool helpersVisible() const;

    Vector2i preferredSize(NVGcontext* ctx) const override;
    void performLayout(NVGcontext* ctx) override;
    void draw(NVGcontext* ctx) override;

private:
    // Helper image methods.
    void updateImageParameters();

    // Helper drawing methods.
    void drawWidgetBorder(NVGcontext* ctx) const;
    void drawImageBorder(NVGcontext* ctx) const;
    void drawHelpers(NVGcontext* ctx) const;
    static void drawPixelGrid(NVGcontext* ctx, const Vector2f& upperLeftCorner,
                              const Vector2f& lowerRightCorner, const float stride);
    void drawPixelInfo(NVGcontext* ctx, const float stride) const;
    void writePixelInfo(NVGcontext* ctx, const Vector2f& cellPosition,
                        const Vector2i& pixel, const float stride) const;

    // Image parameters.
    GLShader mShader;
    GLuint mImageID;
    Vector2i mImageSize;

    // Image display parameters.
    float mScale;
    Vector2i mOffset;
    bool mFixedScale;
    bool mFixedOffset;

    // Fine-tuning parameters.
    float mZoomSensitivity = 1.1f;

    // Image info parameters.
    float mGridThreshold = -1;
    float mPixelInfoThreshold = -1;

    // Image pixel data display members.
    std::function<std::pair<std::string, Color>(const Vector2i&)> mPixelInfoCallback;
    float mFontScaleFactor = 0.2f;
};

NAMESPACE_END(nanogui)