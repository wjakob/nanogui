/*
    src/graph.cpp -- Simple graph widget for showing a function plot

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/graph.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/saveload.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Graph, Widget)

Graph::Graph(Widget *parent, const std::string &caption)
    : Widget(parent), mCaption(caption) {
    mBackgroundColor = Color(20, 128);
    mForegroundColor = Color(255, 192, 0, 128);
    mTextColor = Color(240, 192);
}

Vector2i Graph::preferredSize(NVGcontext *) const {
    return Vector2i(180, 45);
}

void Graph::draw(NVGcontext *ctx) {
    Widget::draw(ctx);

    nvgBeginPath(ctx);
    nvgRect(ctx, mPos, mSize);
    nvgFillColor(ctx, mBackgroundColor);
    nvgFill(ctx);

    if (mValues.size() < 2)
        return;

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, mPos.x(), mPos.y()+mSize.y());
    for (size_t i = 0; i < (size_t) mValues.size(); i++) {
        float value = mValues[i];
        float vx = mPos.x() + i * mSize.x() / (float) (mValues.size() - 1);
        float vy = mPos.y() + (1-value) * mSize.y();
        nvgLineTo(ctx, vx, vy);
    }

    nvgLineTo(ctx, mPos.x() + mSize.x(), mPos.y() + mSize.y());
    nvgStrokeColor(ctx, Color(100, 255));
    nvgStroke(ctx);
    nvgFillColor(ctx, mForegroundColor);
    nvgFill(ctx);

    nvgFontFace(ctx, "sans");

    if (!mCaption.empty()) {
        nvgFontSize(ctx, 14.0f);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        nvgFillColor(ctx, mTextColor);
        nvgText(ctx, mPos.x() + 3, mPos.y() + 1, mCaption.c_str(), NULL);
    }

    if (!mHeader.empty()) {
        nvgFontSize(ctx, 18.0f);
        nvgTextAlign(ctx, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
        nvgFillColor(ctx, mTextColor);
        nvgText(ctx, mPos.x() + mSize.x() - 3, mPos.y() + 1, mHeader.c_str(), NULL);
    }

    if (!mFooter.empty()) {
        nvgFontSize(ctx, 15.0f);
        nvgTextAlign(ctx, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);
        nvgFillColor(ctx, mTextColor);
        nvgText(ctx, mPos.x() + mSize.x() - 3, mPos.y() + mSize.y() - 1, mFooter.c_str(), NULL);
    }

    nvgBeginPath(ctx);
    nvgRect(ctx, mPos, mSize);
    nvgStrokeColor(ctx, Color(100, 255));
    nvgStroke(ctx);
}

void Graph::save(Json::value &s) const 
{
    Widget::save(s);
    auto obj = s.get_obj();

    obj["caption"] = json().set(mCaption).name("Caption");
    obj["header"] = json().set(mHeader).name("Header");
    obj["footer"] = json().set(mFooter).name("Footer");
    obj["backgroundColor"] = json().set(mBackgroundColor).name("Backgrodun color");
    obj["foregroundColor"] = json().set(mForegroundColor).name("Foreground color");
    obj["textColor"] = json().set(mTextColor).name("Text color");
    obj["values"] = json().set(mValues).name("Values");

    s = Json::value(obj);
}

bool Graph::load(Json::value &save) 
{
    Widget::load(save);
    json s{ save.get_obj() };
    
    mCaption = s.get<std::string>("caption");
    mHeader = s.get<std::string>("header");
    mFooter = s.get<std::string>("footer");
    mBackgroundColor = s.get<Color>("backgroundColor");
    mForegroundColor = s.get<Color>("foregroundColor");
    mTextColor = s.get<Color>("textColor");
    mValues = s.get<decltype(mValues)>("values");
    
    return true;
}

NAMESPACE_END(nanogui)
