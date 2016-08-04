/*
    nanogui/imageview.cpp -- Widget used to display images.

    The image view widget was contributed by Stefan Ivanov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/imageview.h>
#include <nanogui/window.h>
#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <string>
#include <vector>

NAMESPACE_BEGIN(nanogui)

namespace
{

    std::vector<std::string> splitString(const std::string& text, const std::string& delimiter) {
        using std::string; using std::vector;
        vector<string> strings;
        string::size_type current = 0;
        string::size_type previous = 0;
        while ((current = text.find(delimiter, previous)) != string::npos) {
            strings.push_back(text.substr(previous, current - previous));
            previous = current + 1;
        }
        strings.push_back(text.substr(previous));
        return strings;
    }

    constexpr char const *const defaultImageViewVertexShader =
        R"(#version 330
        uniform vec2 scaleFactor;
        uniform vec2 position;
        in vec2 vertex;
        out vec2 uv;
        void main() {
            uv = vertex;
            vec2 scaledVertex = (vertex * scaleFactor) + position; 
            gl_Position  = vec4(2.0*scaledVertex.x - 1.0f,
                                1.0 - 2.0*scaledVertex.y,
                                0.0, 1.0);

        })";

    constexpr char const *const defaultImageViewFragmentShader =
        R"(#version 330
        uniform sampler2D texture;
        out vec4 color;
        in vec2 uv;
        void main() {
            color= texture2D(texture, uv);
        })";

}

ImageView::ImageView(Widget* parent, GLuint imageID)
    : Widget(parent), mImageID(imageID), mScale(1.0f), mOffset(Vector2i::Zero()),
    mFixedScale(false), mFixedOffset(false), mPixelInfoCallback(nullptr) {
    updateImageParameters();
    mShader.init("Default_Image_View_Shader", defaultImageViewVertexShader,
                 defaultImageViewFragmentShader);

    MatrixXu indices(3, 2);
    indices.col(0) << 0, 1, 2;
    indices.col(1) << 2, 3, 1;

    MatrixXf vertices(2, 4);
    vertices.col(0) << 0, 0;
    vertices.col(1) << 1, 0;
    vertices.col(2) << 0, 1;
    vertices.col(3) << 1, 1;

    mShader.bind();
    mShader.uploadIndices(indices);
    mShader.uploadAttrib("vertex", vertices);
    mShader.setUniform("texture", 0);
}

ImageView::~ImageView() {
    mShader.free();
}

void ImageView::bindImage(GLuint imageId) {
    mImageID = imageId;
    updateImageParameters();
    fit();
}

Vector2f ImageView::imageCoordinateAt(const Vector2i& position) const {
    Vector2i imagePosition = position - mOffset;
    return (imagePosition.cast<float>() / mScale);
}

Vector2f ImageView::clampedImageCoordinateAt(const Vector2i& position) const {
    Vector2f imageCoordinate = imageCoordinateAt(position);
    return imageCoordinate.array().
        max(Vector2f::Zero().array()).
        min(mImageSize.cast<float>().array());
}

Vector2i ImageView::positionForCoordinate(const Vector2f& imageCoordinate) const {
    return (imageCoordinate*mScale).cast<int>() + mOffset;
}

void ImageView::setImageCoordinateAt(const Vector2i& position, const Vector2f& imageCoordinate) {
    mOffset = position - (imageCoordinate * mScale).cast<int>();
}

void ImageView::center() {
    // In order to center the image we need to fulfill the following equation
    // mOffset + scalledImageSize() / 2 == mSize / 2
    mOffset = (mSize - scaledImageSize()) / 2;
}

void ImageView::fit() {
    // Calculate the appropriate scaling factor.
    mScale = (mSize.cast<float>().cwiseQuotient(mImageSize.cast<float>())).minCoeff();
    center();
}

void ImageView::setScaleCentered(float scale) {
    Vector2i centerPosition = mSize / 2;
    auto p = imageCoordinateAt(centerPosition);
    mScale = scale;
    setImageCoordinateAt(centerPosition, p);
}

void ImageView::moveOffset(const Vector2i& delta) {
    // Apply the delta to the offset.
    mOffset += delta;
    // Prevent the image from going too much out of bounds.
    Vector2i scaledSize = scaledImageSize();
    if (mOffset.x() + scaledSize.x() < 0)
        mOffset.x() = -scaledSize.x();
    if (mOffset.x() > size().x())
        mOffset.x() = size().x();
    if (mOffset.y() + scaledSize.y() < 0)
        mOffset.y() = -scaledSize.y();
    if (mOffset.y() > size().y())
        mOffset.y() = size().y();
}

void ImageView::zoom(int amount, const Vector2i& focusPixel) {
    Vector2f focusedCoordinate = imageCoordinateAt(focusPixel);
    auto scaleFactor = std::pow(mZoomSensitivity, amount);
    mScale *= scaleFactor;
    if (mScale < 0.01f)
        mScale = 0.01f;
    setImageCoordinateAt(focusPixel, focusedCoordinate);
}

bool ImageView::mouseDragEvent(const Vector2i& p, const Vector2i& rel, int button, int /*modifiers*/) {
    // The bellow macro, for the left button, corresponds to, confusingly enough, 
    // the right button on my computer. /*GLFW_MOUSE_BUTTON_LEFT*/
    if (button == GLFW_MOUSE_BUTTON_2 && !mFixedOffset) {
        setImageCoordinateAt(p + rel, imageCoordinateAt(p));
        return true;
    }
    return false;
}

bool ImageView::gridVisible() const {
    return (mGridThreshold != -1) && (mScale > mGridThreshold);
}

bool ImageView::pixelInfoVisible() const {
    return mPixelInfoCallback && (mPixelInfoThreshold != -1) && (mScale > mPixelInfoThreshold);
}

bool ImageView::helpersVisible() const {
    return gridVisible() || pixelInfoVisible();
}

bool ImageView::scrollEvent(const Vector2i& p, const Vector2f& rel) {
    if (mFixedScale) {
        return false;
    }
    zoom(rel.y(), p);
    return true;
}

bool ImageView::keyboardEvent(int key, int /*scancode*/, int action, int modifiers) {
    if (action) {
        switch (key) {
        case GLFW_KEY_LEFT:
            if (!mFixedOffset) {
                if (GLFW_MOD_CONTROL & modifiers)
                    moveOffset(Vector2i(30, 0));
                else
                    moveOffset(Vector2i(10, 0));
                return true;
            }
            break;
        case GLFW_KEY_RIGHT:
            if (!mFixedOffset) {
                if (GLFW_MOD_CONTROL & modifiers)
                    moveOffset(Vector2i(-30, 0));
                else
                    moveOffset(Vector2i(-10, 0));
                return true;
            }
            break;
        case GLFW_KEY_DOWN:
            if (!mFixedOffset) {
                if (GLFW_MOD_CONTROL & modifiers)
                    moveOffset(Vector2i(0, -30));
                else
                    moveOffset(Vector2i(0, -10));
                return true;
            }
            break;
        case GLFW_KEY_UP:
            if (!mFixedOffset) {
                if (GLFW_MOD_CONTROL & modifiers)
                    moveOffset(Vector2i(0, 30));
                else
                    moveOffset(Vector2i(0, 10));
                return true;
            }
            break;
        }
    }
    return false;
}

bool ImageView::keyboardCharacterEvent(unsigned int codepoint) {
    switch (codepoint) {
    case '-':
        if (!mFixedScale) {
            zoom(-1, mSize / 2);
            return true;
        }
        break;
    case '+':
        if (!mFixedScale) {
            zoom(1, mSize / 2);
            return true;
        }
        break;
    case 'c':
        if (!mFixedOffset) {
            center();
            return true;
        }
        break;
    case 'f':
        if (!mFixedOffset && !mFixedScale) {
            fit();
            return true;
        }
        break;
    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
        if (!mFixedScale) {
            setScaleCentered(codepoint - '0');
            return true;
        }
        break;
    default:
        return false;
    }
    return false;
}

Vector2i ImageView::preferredSize(NVGcontext* /*ctx*/) const {
    return mImageSize;
}

void ImageView::performLayout(NVGcontext* ctx) {
    Widget::performLayout(ctx);
    center();
}

void ImageView::draw(NVGcontext* ctx) {
    Widget::draw(ctx);
    nvgEndFrame(ctx); // Flush the NanoVG draw stack, not necessary to call nvgBeginFrame afterwards.

    drawWidgetBorder(ctx);
    drawImageBorder(ctx);

    // Calculate several variables that need to be send to OpenGL in order for the image to be
    // properly displayed inside the widget.
    Screen* containingScreen = dynamic_cast<Screen*>(this->window()->parent());
    assert(containingScreen);
    Vector2i screenSize = containingScreen->size();
    Vector2f scaleFactor = mScale * imageSize().cast<float>().cwiseQuotient(screenSize.cast<float>());
    Vector2i positionInScreen = absolutePosition();
    Vector2i positionAfterOffset = positionInScreen + mOffset;
    Vector2f imagePosition = positionAfterOffset.cast<float>().cwiseQuotient(screenSize.cast<float>());
    glEnable(GL_SCISSOR_TEST);
    glScissor(positionInScreen.x(), screenSize.y() - positionInScreen.y() - size().y(),
              size().x(), size().y());
    mShader.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mImageID);
    mShader.setUniform("texture", 0);
    mShader.setUniform("scaleFactor", scaleFactor);
    mShader.setUniform("position", imagePosition);
    mShader.drawIndexed(GL_TRIANGLES, 0, 2);
    glDisable(GL_SCISSOR_TEST);

    if (helpersVisible())
        drawHelpers(ctx);
}


void ImageView::updateImageParameters() {
    // Query the width of the OpenGL texture.
    glBindTexture(GL_TEXTURE_2D, mImageID);
    GLint w, h;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
    mImageSize = Vector2i(w, h);
}

void ImageView::drawWidgetBorder(NVGcontext* ctx) const {
    nvgBeginPath(ctx);
    nvgStrokeWidth(ctx, 1.0f);
    nvgRoundedRect(ctx, mPos.x() - 0.5f, mPos.y() - 0.5f,
                   mSize.x() + 1, mSize.y() + 1, mTheme->mWindowCornerRadius);
    nvgStrokeColor(ctx, mTheme->mBorderLight);
    nvgRoundedRect(ctx, mPos.x() - 1.0f, mPos.y() - 1.0f,
                   mSize.x() + 2, mSize.y() + 2, mTheme->mWindowCornerRadius);
    nvgStrokeColor(ctx, mTheme->mBorderDark);
    nvgStroke(ctx);
}

void ImageView::drawImageBorder(NVGcontext* ctx) const {
    nvgSave(ctx);
    nvgBeginPath(ctx);
    nvgScissor(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y());
    nvgStrokeWidth(ctx, 1.0f);
    Vector2i borderPosition = mPos + mOffset;
    Vector2i borderSize = scaledImageSize();
    nvgRect(ctx, borderPosition.x() - 0.5f, borderPosition.y() - 0.5f,
            borderSize.x() + 1, borderSize.y() + 1);
    nvgStrokeColor(ctx, Color(1.0f, 1.0f, 1.0f, 1.0f));
    nvgStroke(ctx);
    nvgResetScissor(ctx);
    nvgRestore(ctx);
}

void ImageView::drawHelpers(NVGcontext* ctx) const {
    // We need to apply mPos after the transformation to account for the position of the widget
    // relative to the parent.
    Vector2f upperLeftCorner = positionForCoordinate(Vector2f(0, 0)).cast<float>() + mPos.cast<float>();
    Vector2f lowerRightCorner = positionForCoordinate(mImageSize.cast<float>()).cast<float>() + mPos.cast<float>();
    // Use the scissor method in NanoVG to display only the correct part of the grid.
    Vector2f scissorPosition = upperLeftCorner.array().max(mPos.cast<float>().array());
    Vector2f sizeOffsetDifference = (mSize - mOffset).cast<float>();
    Vector2f scissorSize = sizeOffsetDifference.array().min(mSize.cast<float>().array());
    nvgSave(ctx);
    nvgScissor(ctx, scissorPosition.x(), scissorPosition.y(), scissorSize.x(), scissorSize.y());
    if (gridVisible())
        drawPixelGrid(ctx, upperLeftCorner, lowerRightCorner, mScale);
    if (pixelInfoVisible())
        drawPixelInfo(ctx, mScale);
    nvgResetScissor(ctx);
    nvgRestore(ctx);
}

void ImageView::drawPixelGrid(NVGcontext* ctx, const Vector2f& upperLeftCorner,
                              const Vector2f& lowerRightCorner, const float stride) {
    nvgBeginPath(ctx);
    // Draw the vertical lines for the grid
    float currentX = std::floor(upperLeftCorner.x());
    while (currentX <= lowerRightCorner.x()) {
        nvgMoveTo(ctx, std::floor(currentX), std::floor(upperLeftCorner.y()));
        nvgLineTo(ctx, std::floor(currentX), std::floor(lowerRightCorner.y()));
        currentX += stride;
    }
    // Draw the horizontal lines for the grid.
    float currentY = std::floor(upperLeftCorner.y());
    while (currentY <= lowerRightCorner.y()) {
        nvgMoveTo(ctx, std::floor(upperLeftCorner.x()), std::floor(currentY));
        nvgLineTo(ctx, std::floor(lowerRightCorner.x()), std::floor(currentY));
        currentY += stride;
    }
    nvgStrokeWidth(ctx, 1.0f);
    nvgStrokeColor(ctx, Color(1.0f, 1.0f, 1.0f, 1.0f));
    nvgStroke(ctx);
}

void ImageView::drawPixelInfo(NVGcontext* ctx, const float stride) const {
    // Extract the image coordinates at the two corners of the widget.
    Vector2f currentPixelF = clampedImageCoordinateAt(Vector2i::Zero());
    Vector2f lastPixelF = clampedImageCoordinateAt(mSize);
    // Round the top left coordinates down and bottom down coordinates up.
    // This is done so that the edge information does not pop up suddenly when it gets in range.
    currentPixelF = currentPixelF.unaryExpr([](float x) { return std::floor(x); });
    lastPixelF = lastPixelF.unaryExpr([](float x) { return std::ceil(x); });
    Vector2i currentPixel = currentPixelF.cast<int>();
    Vector2i lastPixel = lastPixelF.cast<int>();

    // Extract the positions for where to draw the text.
    Vector2f currentCellPosition = (mPos + positionForCoordinate(currentPixelF)).cast<float>();
    float xInitialPosition = currentCellPosition.x();
    int xInitialIndex = currentPixel.x();


    // Properly scale the pixel information for the given stride.
    auto fontSize = stride * mFontScaleFactor;
    static constexpr float maxFontSize = 30.0f;
    fontSize = fontSize > maxFontSize ? maxFontSize : fontSize;
    nvgSave(ctx);
    nvgBeginPath(ctx);
    nvgFontSize(ctx, fontSize);
    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
    // TODO: The font face can disappear without the line below.
    nvgFontFace(ctx, "sans");
    while (currentPixel.y() != lastPixel.y()) {
        while (currentPixel.x() != lastPixel.x()) {
            writePixelInfo(ctx, currentCellPosition, currentPixel, stride);
            currentCellPosition.x() += stride;
            ++currentPixel.x();
        }
        currentCellPosition.x() = xInitialPosition;
        currentCellPosition.y() += stride;
        ++currentPixel.y();
        currentPixel.x() = xInitialIndex;
    }
    nvgRestore(ctx);
}

void ImageView::writePixelInfo(NVGcontext* ctx, const Vector2f& cellPosition,
                               const Vector2i& pixel, const float stride) const {
    auto pixelData = mPixelInfoCallback(pixel);
    auto pixelDataRows = splitString(pixelData.first, "\n");
    // If no data is provided for this pixel then simply return.
    if (pixelDataRows.empty())
        return;
    nvgFillColor(ctx, pixelData.second);
    auto padding = stride / 10;
    auto maxSize = stride - 2 * padding;
    // Measure the size of a single line of text.
    float bounds[4];
    nvgTextBoxBounds(ctx, 0.0f, 0.0f, maxSize, pixelDataRows.front().data(), nullptr, bounds);
    auto rowHeight = bounds[3] - bounds[1];
    auto totalRowsHeight = rowHeight * pixelDataRows.size();
    // Choose the initial y offset and the index for the past the last visible row.
    auto yOffset = 0.0f;
    auto lastIndex = 0;
    // Structured bindings would have been so nice here...
    if (totalRowsHeight > maxSize) {
        yOffset = padding;
        lastIndex = static_cast<int>(maxSize / rowHeight);
    } else {
        yOffset = (stride - totalRowsHeight) / 2;
        lastIndex = static_cast<int>(pixelDataRows.size());
    }
    for (int i = 0; i != lastIndex; ++i) {
        // TODO: Limit the width of the rows - probably like in the tab header.
        nvgText(ctx, cellPosition.x() + stride / 2, cellPosition.y() + yOffset,
                pixelDataRows[i].data(), nullptr);
        yOffset += rowHeight;
    }
}

NAMESPACE_END(nanogui)