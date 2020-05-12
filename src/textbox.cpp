/*
    src/textbox.cpp -- Fancy text box with builtin regular
    expression-based validation

    The text box widget was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/window.h>
#include <nanogui/screen.h>
#include <nanogui/textbox.h>
#include <nanovg.h>
#include <nanogui/theme.h>
#include <nanogui/saveload.h>
#include <regex>
#include <iostream>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(TextBox, Widget)

TextBox::TextBox(Widget *parent,const std::string &value)
    : Widget(parent),
      mEditable(false),
      mSpinnable(false),
      mCommitted(true),
      mValue(value),
      mDefaultValue(""),
      mAlignment(TextAlignment::Center),
      mUnits(""),
      mFormat(""),
      mUnitsImage(-1),
      mValidFormat(true),
      mValueTemp(value),
      mCursorPos(-1),
      mSelectionPos(-1),
      mMousePos(Vector2i(-1,-1)),
      mMouseDownPos(Vector2i(-1,-1)),
      mMouseDragPos(Vector2i(-1,-1)),
      mMouseDownModifier(0),
      mTextOffset(0),
      mLastClick(0)
{
    mIconExtraScale = 0.8f;// widget override
    mBorderSize = 0;
}

void TextBox::setEditable(bool editable) {
    mEditable = editable;
    setCursor(editable ? Cursor::IBeam : Cursor::Arrow);
}

void TextBox::setTheme(Theme *theme) {  Widget::setTheme(theme); }

int TextBox::fontSize() const
{
  return mFontSize > 0 ? mFontSize : mTheme->mTextBoxFontSize;
}

Vector2i TextBox::preferredSize(NVGcontext *ctx) const 
{
    Vector2i size(0, fontSize() * 1.4f);

    float uw = 0;
    if (mUnitsImage > 0) {
        int w, h;
        nvgImageSize(ctx, mUnitsImage, &w, &h);
        float uh = size.y() * 0.4f;
        uw = w * uh / h;
    } else if (!mUnits.empty()) {
        uw = nvgTextBounds(ctx, 0, 0, mUnits.c_str(), nullptr, nullptr);
    }
    float sw = 0;
    if (mSpinnable) {
        sw = 14.f;
    }

    float ts = nvgTextBounds(ctx, 0, 0, mValue.c_str(), nullptr, nullptr);
    size.x() = size.y() + ts + uw + sw;
    if (mFixedSize.x() > 0)
      size.x() = mFixedSize.x();
    if (mFixedSize.y() > 0)
      size.y() = mFixedSize.y();
    return size;
}

int TextBox::getCornerRadius() const { return mTheme->mTextBoxCornerRadius; }

void TextBox::afterDraw(NVGcontext* ctx)
{
  Widget::afterDraw(ctx);
  if (mUpdateCallback)
    mUpdateCallback(this);
}

void TextBox::createBoxGradient(NVGcontext* ctx, NVGpaint& fg, const Color& c1, const Color& c2, int cr)
{
  fg = nvgBoxGradient(ctx, mPos.x() + 1, mPos.y() + 1 + 1.0f,
                           mSize.x() - 2, mSize.y() - 2,
                           cr, 4, c1, c2);
}

void TextBox::draw(NVGcontext* ctx) 
{
    int cornerRadius = getCornerRadius();
    NVGpaint fg1;
    createBoxGradient(ctx, fg1,
                      mBackgroundHoverColor.notW(theme()->textBoxFocusedColorIn), 
                      mBackgroundHoverColor.notW(theme()->textBoxFocusedColorOut), 
                      cornerRadius);

    //background fill
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1 + 1.0f,
                        mSize.x() - 2, mSize.y() - 2, cornerRadius);

    if (mEditable && focused())
    {
      if (!mValidFormat)
        createBoxGradient(ctx, fg1, 
                          theme()->textBoxInvalidFormatColorIn, theme()->textBoxInvalidFormatColorOut,
                          cornerRadius);
    }
    else if (mSpinnable && mMouseDownPos.x() != -1)
    {
      ;
    }
    else
    {
      createBoxGradient(ctx, fg1, 
                        mBackgrodunColor.notW(theme()->textBoxUnfocusedColorIn), 
                        mBackgrodunColor.notW(theme()->textBoxUnfocusedColorOut), 
                        cornerRadius);
    }

    nvgFillPaint(ctx, fg1);
    nvgFill(ctx);

    //background line
    nvgSave(ctx);
    nvgResetScissor(ctx);
    nvgBeginPath(ctx);
    float bs = mBorderSize > 0 ? mBorderSize : theme()->textBoxBorderSize;
    nvgStrokeWidth(ctx, bs );
    nvgRoundedRect(ctx, mPos.x() + (-bs/2+0.5f), mPos.y() + (-bs/2+0.5f), 
                        mSize.x() + (bs/2+0.5f), mSize.y() + (bs/2+0.5f), 
                        cornerRadius - 0.5f);
    nvgStrokeColor(ctx, mBorderColor.notW(theme()->textBoxBorderColor));
    nvgStroke(ctx);
    nvgRestore(ctx);

    nvgFontFaceSize(ctx, "sans", fontSize());
    Vector2i drawPos(mPos.x(), mPos.y() + mSize.y() * 0.5f + 1);

    float xSpacing = mSize.y() * 0.3f;

    float unitWidth = 0;

    if (mUnitsImage > 0) {
        int w, h;
        nvgImageSize(ctx, mUnitsImage, &w, &h);
        float unitHeight = mSize.y() * 0.4f;
        unitWidth = w * unitHeight / h;
        NVGpaint imgPaint = nvgImagePattern(
            ctx, mPos.x() + mSize.x() - xSpacing - unitWidth,
            drawPos.y() - unitHeight * 0.5f, unitWidth, unitHeight, 0,
            mUnitsImage, mEnabled ? 0.7f : 0.35f);
        nvgBeginPath(ctx);
        nvgRect(ctx, mPos.x() + mSize.x() - xSpacing - unitWidth,
                drawPos.y() - unitHeight * 0.5f, unitWidth, unitHeight);
        nvgFillPaint(ctx, imgPaint);
        nvgFill(ctx);
        unitWidth += 2;
    } else if (!mUnits.empty()) {
        unitWidth = nvgTextBounds(ctx, 0, 0, mUnits.c_str(), nullptr, nullptr);
        nvgFillColor(ctx, Color(255, mEnabled ? 64 : 32));
        nvgTextAlign(ctx, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        nvgText(ctx, mPos.x() + mSize.x() - xSpacing, drawPos.y(),
                mUnits.c_str(), nullptr);
        unitWidth += 2;
    }

    float spinArrowsWidth = 0.f;

    if (mSpinnable && !focused()) {
        spinArrowsWidth = 14.f;

        nvgFontFace(ctx, "icons");
        nvgFontSize(ctx, (mFontSize > 0 ? mFontSize : mTheme->mButtonFontSize) * icon_scale());

        bool spinning = mMouseDownPos.x() != -1;

        /* up button */ {
            bool hover = mMouseFocus && spinArea(mMousePos) == SpinArea::Top;
            nvgFillColor(ctx, (mEnabled && (hover || spinning)) ? mTheme->mTextColor : mTheme->mDisabledTextColor);
            auto icon = utf8(mTheme->mTextBoxUpIcon);
            nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
            Vector2f iconPos(mPos.x() + 4.f,
                             mPos.y() + mSize.y()/2.f - xSpacing/2.f);
            nvgText(ctx, iconPos.x(), iconPos.y(), icon.data(), nullptr);
        }

        /* down button */ {
            bool hover = mMouseFocus && spinArea(mMousePos) == SpinArea::Bottom;
            nvgFillColor(ctx, (mEnabled && (hover || spinning)) ? mTheme->mTextColor : mTheme->mDisabledTextColor);
            auto icon = utf8(mTheme->mTextBoxDownIcon);
            nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
            Vector2f iconPos(mPos.x() + 4.f,
                             mPos.y() + mSize.y()/2.f + xSpacing/2.f + 1.5f);
            nvgText(ctx, iconPos.x(), iconPos.y(), icon.data(), nullptr);
        }

        nvgFontSize(ctx, fontSize());
        nvgFontFace(ctx, "sans");
    }

    switch (mAlignment) {
        case TextAlignment::Left:
            nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
            drawPos.x() += xSpacing + spinArrowsWidth;
            break;
        case TextAlignment::Right:
            nvgTextAlign(ctx, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
            drawPos.x() += mSize.x() - unitWidth - xSpacing;
            break;
        case TextAlignment::Center:
            nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            drawPos.x() += mSize.x() * 0.5f;
            break;
        default: break;
    }

    nvgFontSize(ctx, fontSize());
    nvgFillColor(ctx, mEnabled && (!mCommitted || !mValue.empty()) ?
        mTheme->mTextColor :
        mTheme->mDisabledTextColor);

    // clip visible text area
    float clipX = mPos.x() + xSpacing + spinArrowsWidth - 1.0f;
    float clipY = mPos.y() + 1.0f;
    float clipWidth = mSize.x() - unitWidth - spinArrowsWidth - 2 * xSpacing + 2.0f;
    float clipHeight = mSize.y() - 3.0f;

    nvgSave(ctx);
    nvgIntersectScissor(ctx, clipX, clipY, clipWidth, clipHeight);

    Vector2i oldDrawPos(drawPos);
    drawPos.x() += mTextOffset;

    if (mCommitted) {
        nvgText(ctx, drawPos.x(), drawPos.y(),
            mValue.empty() ? mPlaceholder.c_str() : mValue.c_str(), nullptr);
    } else {
        const int maxGlyphs = 1024;
        NVGglyphPosition glyphs[maxGlyphs];
        float textBound[4];
        nvgTextBounds(ctx, drawPos.x(), drawPos.y(), mValueTemp.c_str(),
                      nullptr, textBound);
        float lineh = textBound[3] - textBound[1];

        // find cursor positions
        int nglyphs =
            nvgTextGlyphPositions(ctx, drawPos.x(), drawPos.y(),
                                  mValueTemp.c_str(), nullptr, glyphs, maxGlyphs);
        updateCursor(ctx, textBound[2], glyphs, nglyphs);

        // compute text offset
        int prevCPos = mCursorPos > 0 ? mCursorPos - 1 : 0;
        int nextCPos = mCursorPos < nglyphs ? mCursorPos + 1 : nglyphs;
        float prevCX = cursorIndex2Position(prevCPos, textBound[2], glyphs, nglyphs);
        float nextCX = cursorIndex2Position(nextCPos, textBound[2], glyphs, nglyphs);

        if (nextCX > clipX + clipWidth)
            mTextOffset -= nextCX - (clipX + clipWidth) + 1;
        if (prevCX < clipX)
            mTextOffset += clipX - prevCX + 1;

        drawPos.x() = oldDrawPos.x() + mTextOffset;

        // draw text with offset
        nvgText(ctx, drawPos.x(), drawPos.y(), mValueTemp.c_str(), nullptr);
        nvgTextBounds(ctx, drawPos.x(), drawPos.y(), mValueTemp.c_str(),
                      nullptr, textBound);

        // recompute cursor positions
        nglyphs = nvgTextGlyphPositions(ctx, drawPos.x(), drawPos.y(),
                mValueTemp.c_str(), nullptr, glyphs, maxGlyphs);

        if (mCursorPos > -1) {
            if (mSelectionPos > -1) {
                float caretx = cursorIndex2Position(mCursorPos, textBound[2],
                                                    glyphs, nglyphs);
                float selx = cursorIndex2Position(mSelectionPos, textBound[2],
                                                  glyphs, nglyphs);

                if (caretx > selx)
                    std::swap(caretx, selx);

                // draw selection
                nvgBeginPath(ctx);
                nvgFillColor(ctx, nvgRGBA(255, 255, 255, 80));
                nvgRect(ctx, caretx, drawPos.y() - lineh * 0.5f, selx - caretx,
                        lineh);
                nvgFill(ctx);
            }

            float caretx = cursorIndex2Position(mCursorPos, textBound[2], glyphs, nglyphs);

            // draw cursor
            nvgBeginPath(ctx);
            nvgMoveTo(ctx, caretx, drawPos.y() - lineh * 0.5f);
            nvgLineTo(ctx, caretx, drawPos.y() + lineh * 0.5f);
            nvgStrokeColor(ctx, nvgRGBA(255, 192, 0, 255));
            nvgStrokeWidth(ctx, 1.0f);
            nvgStroke(ctx);
        }
    }
    nvgRestore(ctx);

    Widget::draw(ctx);
}

bool TextBox::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) 
{
    if (isMouseButtonLeft(button) && down && !mFocused) {
        if (!mSpinnable || spinArea(p) == SpinArea::None) /* not on scrolling arrows */
            requestFocus();
    }

    if (mEditable && focused()) {
        if (down) {
            mMouseDownPos = p;
            mMouseDownModifier = modifiers;

            double time = getTimeFromStart();
            if (time - mLastClick < 0.25) {
                /* Double-click: select all text */
                mSelectionPos = 0;
                mCursorPos = (int) mValueTemp.size();
                mMouseDownPos = Vector2i(-1, -1);
            }
            mLastClick = time;
        } else {
            mMouseDownPos = Vector2i(-1, -1);
            mMouseDragPos = Vector2i(-1, -1);
        }
        return true;
    } else if (mSpinnable && !focused()) {
        if (down) {
            if (spinArea(p) == SpinArea::None) {
                mMouseDownPos = p;
                mMouseDownModifier = modifiers;

                double time = getTimeFromStart();
                if (time - mLastClick < 0.25) {
                    /* Double-click: reset to default value */
                    mValue = mDefaultValue;
                    if (mCallback)
                        mCallback(mValue);

                    mMouseDownPos = Vector2i(-1, -1);
                }
                mLastClick = time;
            } else {
                mMouseDownPos = Vector2i(-1, -1);
                mMouseDragPos = Vector2i(-1, -1);
            }
        } else {
            mMouseDownPos = Vector2i(-1, -1);
            mMouseDragPos = Vector2i(-1, -1);
        }
        return true;
    }

    return false;
}

bool TextBox::mouseMotionEvent(const Vector2i &p, const Vector2i & /* rel */,
                               int /* button */, int /* modifiers */) {
    mMousePos = p;

    if (!mEditable)
        setCursor(Cursor::Arrow);
    else if (mSpinnable && !focused() && spinArea(mMousePos) != SpinArea::None) /* scrolling arrows */
        setCursor(Cursor::Hand);
    else
        setCursor(Cursor::IBeam);

    if (mEditable && focused()) {
        return true;
    }
    return false;
}

bool TextBox::mouseDragEvent(const Vector2i &p, const Vector2i &/* rel */,
                             int /* button */, int /* modifiers */) {
    mMousePos = p;
    mMouseDragPos = p;

    if (mEditable && focused()) {
        return true;
    }
    return false;
}

bool TextBox::focusEvent(bool focused) {
    Widget::focusEvent(focused);

    std::string backup = mValue;

    if (mEditable) {
        if (focused) {
            mValueTemp = mValue;
            mCommitted = false;
            mCursorPos = 0;
        } else {
            if (mValidFormat) {
                if (mValueTemp == "")
                    mValue = mDefaultValue;
                else
                    mValue = mValueTemp;
            }

            if (mCallback && !mCallback(mValue))
                mValue = backup;

            mValidFormat = true;
            mCommitted = true;
            mCursorPos = -1;
            mSelectionPos = -1;
            mTextOffset = 0;
        }

        mValidFormat = (mValueTemp == "") || checkFormat(mValueTemp, mFormat);
    }

    return true;
}

bool TextBox::keyboardEvent(int key, int scancode, int action, int modifiers) 
{
  if (!focused())
    return false;

  if (mEditable) 
  {
    if (isKeyboardActionPress(action) || isKeyboardActionRepeat(action)) {
        if (isKeyboardKey(key, "LEFT")) 
        {
            if (isKeyboardModifierShift(modifiers)) {
                if (mSelectionPos == -1)
                    mSelectionPos = mCursorPos;
            } else {
                mSelectionPos = -1;
            }

            if (mCursorPos > 0)
                mCursorPos--;
        }
        else if (isKeyboardKey(key, "RGHT")) 
        {
            if (isKeyboardModifierShift(modifiers)) {
                if (mSelectionPos == -1)
                    mSelectionPos = mCursorPos;
            } else {
                mSelectionPos = -1;
            }

            if (mCursorPos < (int) mValueTemp.length())
                mCursorPos++;
        } 
        else if (isKeyboardKey(key, "HOME")) 
        {
            if (isKeyboardModifierShift(modifiers)) {
                if (mSelectionPos == -1)
                    mSelectionPos = mCursorPos;
            } else {
                mSelectionPos = -1;
            }

            mCursorPos = 0;
        } 
        else if (isKeyboardKey(key, "KEND")) 
        {
            if (isKeyboardModifierShift(modifiers)) {
                if (mSelectionPos == -1)
                    mSelectionPos = mCursorPos;
            } else {
                mSelectionPos = -1;
            }

            mCursorPos = (int) mValueTemp.size();
        } 
        else if (isKeyboardKey(key, "BACK")) 
        {
            if (!deleteSelection()) {
                if (mCursorPos > 0) {
                    mValueTemp.erase(mValueTemp.begin() + mCursorPos - 1);
                    mCursorPos--;

                    if (mEditCallback)
                      mEditCallback(mValueTemp, true);
                }
            }
        } 
        else if (isKeyboardKey(key, "KDEL")) 
        {
            if (!deleteSelection()) {
              if (mCursorPos < (int)mValueTemp.length())
              {
                mValueTemp.erase(mValueTemp.begin() + mCursorPos);
                if (mEditCallback)
                  mEditCallback(mValueTemp, true);
              }
            }
        } 
        else if (isKeyboardKey(key, "ENTR")) 
        {
            if (!mCommitted)
                focusEvent(false);
            if (mComitCallback)
            {
              mComitCallback(this);
              return true;
            }
        } 
        else if (isKeyboardKey(key, "KEYA") && isKeyboardModifierCtrl(modifiers)) 
        {
            mCursorPos = (int) mValueTemp.length();
            mSelectionPos = 0;
        } 
        else if (isKeyboardKey(key, "KEYX") && isKeyboardModifierCtrl(modifiers)) 
        {
            copySelection();
            deleteSelection();
        } 
        else if (isKeyboardKey(key, "KEYC") && isKeyboardModifierCtrl(modifiers)) 
        {
            copySelection();
        } 
        else if (isKeyboardKey(key, "KEYV") && isKeyboardModifierCtrl(modifiers)) 
        {
            deleteSelection();
            pasteFromClipboard();
        }

        mValidFormat = (mValueTemp == "") || checkFormat(mValueTemp, mFormat);
    }

    return true;
  }

  return Widget::keyboardEvent(key, scancode, action, modifiers);
}

bool TextBox::keyboardCharacterEvent(unsigned int codepoint) {
    if (mEditable && focused()) {
        std::ostringstream convert;
        convert << (char) codepoint;

        deleteSelection();
        mValueTemp.insert(mCursorPos, convert.str());
        mCursorPos++;

        mValidFormat = (mValueTemp == "") || checkFormat(mValueTemp, mFormat);

        if (mEditCallback)
          mEditCallback(mValueTemp, mValidFormat);

        return true;
    }

    return false;
}

bool TextBox::checkFormat(const std::string &input, const std::string &format) {
    if (format.empty())
        return true;
    try {
        std::regex regex(format);
        return regex_match(input, regex);
    } catch (const std::regex_error &) {
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9)
        std::cerr << "Warning: cannot validate text field due to lacking regular expression support. please compile with GCC >= 4.9" << std::endl;
        return true;
#else
        throw;
#endif
    }
}

bool TextBox::copySelection() {
    if (mSelectionPos > -1) {
        Screen *sc = Screen::cast(window()->parent());
        if (!sc)
            return false;

        int begin = mCursorPos;
        int end = mSelectionPos;

        if (begin > end)
            std::swap(begin, end);

        sc->setClipboardString(mValueTemp.substr(begin, end));
        return true;
    }

    return false;
}

void TextBox::pasteFromClipboard() {
    Screen *sc = Screen::cast(window()->parent());
    if (!sc)
        return;
    std::string cbstr = sc->getClipboardString();
    if (!cbstr.empty())
    {
      mValueTemp.insert(mCursorPos, cbstr);
      if (mEditCallback)
        mEditCallback(mValueTemp, true);
    }
}

bool TextBox::deleteSelection() {
    if (mSelectionPos > -1) {
        int begin = mCursorPos;
        int end = mSelectionPos;

        if (begin > end)
            std::swap(begin, end);

        if (begin == end - 1)
            mValueTemp.erase(mValueTemp.begin() + begin);
        else
            mValueTemp.erase(mValueTemp.begin() + begin,
                             mValueTemp.begin() + end);

        if (mEditCallback)
          mEditCallback(mValueTemp, true);

        mCursorPos = begin;
        mSelectionPos = -1;
        return true;
    }

    return false;
}

void TextBox::updateCursor(NVGcontext *, float lastx,
                           const NVGglyphPosition *glyphs, int size) {
    // handle mouse cursor events
    if (mMouseDownPos.x() != -1) {
        if (isKeyboardModifierShift(mMouseDownModifier)) {
            if (mSelectionPos == -1)
                mSelectionPos = mCursorPos;
        } else
            mSelectionPos = -1;

        mCursorPos =  position2CursorIndex(mMouseDownPos.x(), lastx, glyphs, size);
        mMouseDownPos = Vector2i(-1, -1);
    } else if (mMouseDragPos.x() != -1) {
        if (mSelectionPos == -1)
            mSelectionPos = mCursorPos;

        mCursorPos = position2CursorIndex(mMouseDragPos.x(), lastx, glyphs, size);
    } else {
        // set cursor to last character
        if (mCursorPos == -2)
            mCursorPos = size;
    }

    if (mCursorPos == mSelectionPos)
        mSelectionPos = -1;
}

float TextBox::cursorIndex2Position(int index, float lastx,
                                    const NVGglyphPosition *glyphs, int size) {
    float pos = 0;
    if (index == size)
        pos = lastx; // last character
    else
        pos = glyphs[index].x;

    return pos;
}

int TextBox::position2CursorIndex(float posx, float lastx,
                                  const NVGglyphPosition *glyphs, int size) {
    int mCursorId = 0;
    float caretx = glyphs[mCursorId].x;
    for (int j = 1; j < size; j++) {
        if (std::abs(caretx - posx) > std::abs(glyphs[j].x - posx)) {
            mCursorId = j;
            caretx = glyphs[mCursorId].x;
        }
    }
    if (std::abs(caretx - posx) > std::abs(lastx - posx))
        mCursorId = size;

    return mCursorId;
}

TextBox::SpinArea TextBox::spinArea(const Vector2i & pos) {
    if (0 <= pos.x() - mPos.x() && pos.x() - mPos.x() < 14.f) { /* on scrolling arrows */
        if (mSize.y() >= pos.y() - mPos.y() && pos.y() - mPos.y() <= mSize.y() / 2.f) { /* top part */
            return SpinArea::Top;
        } else if (0.f <= pos.y() - mPos.y() && pos.y() - mPos.y() > mSize.y() / 2.f) { /* bottom part */
            return SpinArea::Bottom;
        }
    }
    return SpinArea::None;
}

void TextBox::save(Json::value &s) const
{
  Widget::save(s);
  auto obj = s.get_obj();

  obj["editable"] = json().set(mEditable).name("Editable");
  obj["spinnable"] = json().set(mSpinnable).name("Spinnable");
  obj["value"] = json().set(mValue).name("Value");
  obj["defaultValue"] = json().set(mDefaultValue).name("Default value");
  obj["alignment"] = json().set((int)mAlignment).name("Alignment");
  obj["units"] = json().set(mUnits).name("Units");
  obj["format"] = json().set(mFormat).name("Format");
  obj["unitsImage"] = json().set(mUnitsImage).name("Units image");
  obj["validFormat"] = json().set(mValidFormat).name("Valid format");
  obj["valueTemp"] = json().set(mValueTemp).name("Value temp");

  s = Json::value(obj);
}

bool TextBox::load(Json::value &save) 
{
  Widget::load(save);
  json s{ save.get_obj() };

  mEditable = s.get<bool>("editable");
  mSpinnable = s.get<bool>("spinnable");
  mValue = s.get<float>("value");
  mDefaultValue = s.get<float>("defaultValue");
  mAlignment = (TextAlignment)s.get<int>("alignment");
  mUnits = s.get<std::string>("units");
  mFormat = s.get<std::string>("format");
  mUnitsImage = s.get<int>("unitsImage");
  mValidFormat = s.get<bool>("validFormat");
  mMousePos = mMouseDownPos = mMouseDragPos = Vector2i::Constant(-1);
  mMouseDownModifier = mTextOffset = 0;
  return true;
}

NAMESPACE_END(nanogui)
