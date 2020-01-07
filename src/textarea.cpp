/*
    src/textarea.cpp -- Multi-line read-only text widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/textarea.h>
#include <nanogui/theme.h>
#include <nanogui/screen.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/entypo.h>
#include <nanovg.h>
#include <algorithm>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(TextArea, Widget)

TextArea::TextArea(Widget *parent) : Widget(parent),
  m_foreground_color(Color(0, 0)), m_background_color(Color(0, 0)),
  m_selection_color(.5f, 1.f), m_font("sans"), m_offset(0),
  m_max_size(0), m_padding(0), m_selectable(true),
  m_selection_start(-1), m_selection_end(-1) { }

void TextArea::setText(const std::string &text)
{
  clear();
  append(text);
}

TextArea& TextArea::header(const std::string &text)
{ 
  append(text + "\n"); 
  return *this;
}

TextArea& TextArea::bulletLine(const std::string &text)
{
  appendIcon(ENTYPO_ICON_MINUS);
  append(std::string(" ") + text + "\n");
  return *this;
}

TextArea& TextArea::separator(const std::string &text)
{
  m_blocks.push_back(Block{ m_offset, 0, std::string(""), m_foreground_color, true, false });
  m_offset = Vector2i(0, m_offset.y() + 2);
  m_max_size = m_max_size.cwiseMax(m_offset);
  return *this;
}

void TextArea::appendIcon(int icon)
{
  NVGcontext *ctx = screen()->nvgContext();

  nvgFontSize(ctx, fontSize());
  nvgFontFace(ctx, "icons");

  std::string str = utf8(icon).data();
  int width = nvgTextBounds(ctx, 0, 0, str.c_str(), nullptr, nullptr);
  m_blocks.push_back(Block{ m_offset, width, str, m_foreground_color, false, true });

  m_offset.x() += width;
  m_max_size = m_max_size.cwiseMax(m_offset);

  mNeedUpdate = true;
}

void TextArea::append(const std::string &text) 
{
    NVGcontext *ctx = screen()->nvgContext();

    nvgFontSize(ctx, fontSize());
    nvgFontFace(ctx, m_font.c_str());

    const char *str = text.c_str();
    do {
        const char *begin = str;

        while (*str != 0 && *str != '\n')
            str++;

        std::string line(begin, str);
        if (line.empty())
            continue;
        int width = nvgTextBounds(ctx, 0, 0, line.c_str(), nullptr, nullptr);
        m_blocks.push_back(Block { m_offset, width, line, m_foreground_color, false, false });

        m_offset.x() += width;
        m_max_size = std::max(m_max_size, m_offset);
        if (*str == '\n') 
        {
            m_offset = Vector2i(mIndentWidth, m_offset.y() + fontSize());
            m_max_size = m_max_size.cwiseMax(m_offset);
        }
    } while (*str++ != 0);

    mNeedUpdate = true;
}

void TextArea::clear() 
{
    m_blocks.clear();
    m_offset = m_max_size = Vector2i(0, 0);
    m_selection_start = m_selection_end = Vector2i(-1, -1);
}

bool TextArea::keyboardEvent(int key, int /* scancode */, int action, int modifiers) 
{
  const Vector2i nonePos(-1, -1);
    if (m_selectable && focused()) 
    {
        if (isKeyboardKey(key, "KEYC") 
            && isKeyboardModifierCtrl(modifiers) 
            && isKeyboardActionPress(action) 
            && m_selection_start != nonePos && m_selection_end != nonePos)
        {
            Vector2i start = m_selection_start, end = m_selection_end;
            if (start.x() > end.x() || (start.x() == end.x() && start.y() > end.y()))
                std::swap(start, end);

            std::string str;
            const int max_glyphs = 1024;
            NVGglyphPosition glyphs[max_glyphs + 1];
            for (int i = start.x(); i <= end.x(); ++i) 
            {
                if (i > start.x() && m_blocks[i].offset.y() != m_blocks[i-1].offset.y())
                    str += '\n';

                const Block &block = m_blocks[i];
                NVGcontext *ctx = screen()->nvgContext();
                int nglyphs = nvgTextGlyphPositions(ctx, block.offset.x(), block.offset.y(),
                                                    block.text.c_str(), nullptr, glyphs, max_glyphs);
                glyphs[nglyphs].str = block.text.c_str() + block.text.length();

                if (i == start.x() && i == end.x())
                    str += std::string(glyphs[start.y()].str, glyphs[end.y()].str);
                else if (i == start.x())
                    str += std::string(glyphs[start.y()].str, glyphs[nglyphs].str);
                else if (i == end.x())
                    str += std::string(glyphs[0].str, glyphs[end.y()].str);
                else
                    str += m_blocks[i].text;
            }
            screen()->setClipboardString(str.c_str());
            return true;
        }
    }
    return false;
}

Vector2i TextArea::preferredSize(NVGcontext *) const { return m_max_size + m_padding * 2; }

void TextArea::draw(NVGcontext *ctx) 
{
    VScrollPanel *vscroll = VScrollPanel::cast(mParent);

    auto start_it = m_blocks.begin();
    auto end_it = m_blocks.end();
    if (vscroll) 
    {
      int window_offset = -position().y();
      int window_size = vscroll->size().y();

      start_it = std::lower_bound(
          m_blocks.begin(),
          m_blocks.end(),
          window_offset,
          [&](const Block &block, int value) { return block.offset.y() + fontSize() < value; }
      );

      end_it = std::upper_bound(
          m_blocks.begin(),
          m_blocks.end(),
          window_offset + window_size,
          [](int value, const Block &block) { return value < block.offset.y(); }
      );
    }
    
    if (mNeedUpdate)
    {
      auto scr = screen();
      if (scr) scr->needPerformLayout(mParent);
      mNeedUpdate = false;
    }

    if (m_background_color.w() != 0.f) 
    {
        nvgFillColor(ctx, m_background_color);
        nvgBeginPath(ctx);
        nvgRect(ctx, mPos, mSize);
        nvgFill(ctx);
    }

    Vector2i selection_end = blockToPosition(m_selection_end);
    selection_end += mPos + m_padding;
    if (m_selection_end != Vector2i(-1)) 
    {
      int transp = 255;
      if (theme()->textAreaBlinkCursor)
        transp = std::sinf(std::fmod((float)getTimeFromStart()*3, 3.14f)) * 255;

      nvgBeginPath(ctx);
      nvgMoveTo(ctx, selection_end.x(), selection_end.y());
      nvgLineTo(ctx, selection_end.x(), selection_end.y() + fontSize());
      nvgStrokeColor(ctx, nvgRGBA(255, 192, 0, transp));
      nvgStrokeWidth(ctx, 2.0f);
      nvgStroke(ctx);
    }

    Vector2i selection_start = blockToPosition(m_selection_start);
    selection_start += mPos + m_padding;
    bool flip = false;
    if (selection_start.y() > selection_end.y() ||
        (selection_start.y() == selection_end.y() && selection_start.x() > selection_end.x())) 
    {
        std::swap(selection_start, selection_end);
        flip = true;
    }
    
    if (m_selection_end != Vector2i(-1) && m_selection_end != Vector2i(-1)) 
    {
        nvgBeginPath(ctx);
        nvgFillColor(ctx, m_selection_color);
        if (selection_end.y() == selection_start.y()) 
        {
            nvgRect(ctx, selection_start.x(), selection_start.y(),
                    selection_end.x() - selection_start.x(),
                    fontSize());
        } 
        else 
        {
            nvgRect(ctx, selection_start.x(), selection_start.y(),
                    m_blocks[flip ? m_selection_end.x() : m_selection_start.x()].width -
                    (selection_start.x() - mPos.x() - m_padding),
                    fontSize());
            nvgRect(ctx, mPos.x() + m_padding, selection_end.y(),
                    selection_end.x() - mPos.x() - m_padding, fontSize());
        }
        nvgFill(ctx);
    }

    nvgFontFace(ctx, m_font.c_str());
    nvgFontSize(ctx, fontSize());
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

    for (auto it = start_it; it != end_it; ++it) 
    {
        const Block &block = *it;
        Color color = block.color;
        if (color.w() == 0)
            color = mTheme->mTextColor;

        Vector2i offset = block.offset + mPos + m_padding;

        if (m_selection_end != Vector2i(-1) && m_selection_end != Vector2i(-1) &&
            offset.y() > selection_start.y() && offset.y() < selection_end.y()) 
        {
            nvgFillColor(ctx, m_selection_color);
            nvgBeginPath(ctx);
            nvgRect(ctx, offset.x(), offset.y(), block.width, fontSize());
            nvgFill(ctx);
        }

        if (block.isSeparator)
        {
          nvgBeginPath(ctx);
          nvgMoveTo(ctx, offset.x(), offset.y());
          nvgLineTo(ctx, mPos.x() + mSize.x(), offset.y());
          nvgStrokeColor(ctx, color);
          nvgStroke(ctx);
        }
        else if (block.isIcon)
        {
          nvgFontFace(ctx, "icons");
          nvgFillColor(ctx, color);
          nvgText(ctx, offset.x(), offset.y(), block.text.c_str(), nullptr);
          nvgFontFace(ctx, m_font.c_str());
        }
        else
        {
          nvgFillColor(ctx, color);
          nvgText(ctx, offset.x(), offset.y(), block.text.c_str(), nullptr);
        }
    }
}

bool TextArea::mouseButtonEvent(const Vector2i &p, int button, bool down, int /* modifiers */) 
{
    if (down && isMouseButtonLeft(button) && m_selectable)
    {
        m_selection_start = m_selection_end = positionToBlock(p - mPos - m_padding);
        requestFocus();
        return true;
    }

    return false;
}

bool TextArea::mouseDragEvent(const Vector2i &p, const Vector2i &/* rel */, int /* button */, int /* modifiers */) 
{
    if (m_selection_start != Vector2i(-1) && m_selectable) 
    {
        m_selection_end = positionToBlock(p - mPos - m_padding);
        return true;
    }
    return false;
}

Vector2i TextArea::positionToBlock(const Vector2i &pos) const 
{
    NVGcontext *ctx = const_cast<TextArea*>(this)->screen()->nvgContext();
    auto it = std::lower_bound(
        m_blocks.begin(),
        m_blocks.end(),
        pos.y(),
        [&](const Block &block, int value) { return block.offset.y() + fontSize() < value; });

    const int max_glyphs = 1024;
    NVGglyphPosition glyphs[max_glyphs];
    int selection = 0;

    if (it == m_blocks.end()) 
    {
        if (m_blocks.empty())
            return Vector2i(-1, 1);
        it = m_blocks.end() - 1;
        const Block &block = *it;
        selection = nvgTextGlyphPositions(ctx, block.offset.x(), block.offset.y(),
                              block.text.c_str(), nullptr, glyphs, max_glyphs);
    } 
    else 
    {
        for (auto it2 = it; it2 != m_blocks.end() && it2->offset.y() == it->offset.y(); ++it2) {
            const Block &block = *it2;
            nvgFontSize(ctx, fontSize());
            nvgFontFace(ctx, m_font.c_str());
            int nglyphs =
                nvgTextGlyphPositions(ctx, block.offset.x(), block.offset.y(),
                                      block.text.c_str(), nullptr, glyphs, max_glyphs);

            for (int i = 0; i < nglyphs; ++i) {
                if (glyphs[i].minx + glyphs[i].maxx < pos.x() * 2)
                    selection = i + 1;
            }
        }
    }

    return Vector2i( it - m_blocks.begin(), selection );
}

Vector2i TextArea::blockToPosition(const Vector2i &pos) const 
{
    if (pos.x() < 0 || pos.x() >= (int) m_blocks.size())
        return Vector2i(-1, -1);

    NVGcontext *ctx = const_cast<TextArea*>(this)->screen()->nvgContext();
    const Block &block = m_blocks[pos.x()];
    const int max_glyphs = 1024;
    NVGglyphPosition glyphs[max_glyphs];
    nvgFontSize(ctx, fontSize());
    nvgFontFace(ctx, m_font.c_str());
    int nglyphs = nvgTextGlyphPositions(ctx, block.offset.x(), block.offset.y(),
                                        block.text.c_str(), nullptr, glyphs, max_glyphs);
    if (pos.y() == nglyphs)
        return block.offset + Vector2i(glyphs[pos.y() - 1].maxx + 1, 0);
    else if (pos.y() > nglyphs)
        return Vector2i(-1, -1);

    return block.offset + Vector2i(glyphs[pos.y()].x, 0);
}

NAMESPACE_END(nanogui)
