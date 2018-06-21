/*
    nanogui/graph.h -- Simple graph widget for showing a function plot

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Graph graph.h nanogui/graph.h
 *
 * \brief Simple graph widget for showing a function plot.
 */
class NANOGUI_EXPORT Graph : public Widget {
public:
    /**
     * \brief Creates a new Graph with the specified parent.
     *
     * \rst
     * .. tip::
     *
     *    See :ref:`nanogui_example_1` for how to set the values of the graph
     *    using the :func:`Graph::values` or :func:`Graph::setValues` methods.
     *
     * \endrst
     *
     * \param parent
     *     The parent of this Graph.
     *
     * \param caption
     *     The caption to use for this Graph (default: ``Untitled``).
     *
     * \param font
     *     The font face to use (default ``""`` implies \ref Theme::defaultFont,
     *     which will typically be ``"sans"``).
     */
    Graph(Widget *parent, const std::string &caption = "Untitled", const std::string &font = "");

    const std::string &caption() const { return mCaption; }
    void setCaption(const std::string &caption) { mCaption = caption; }

    const std::string &header() const { return mHeader; }
    void setHeader(const std::string &header) { mHeader = header; }

    const std::string &footer() const { return mFooter; }
    void setFooter(const std::string &footer) { mFooter = footer; }

    const Color &backgroundColor() const { return mBackgroundColor; }
    void setBackgroundColor(const Color &backgroundColor) { mBackgroundColor = backgroundColor; }

    const Color &foregroundColor() const { return mForegroundColor; }
    void setForegroundColor(const Color &foregroundColor) { mForegroundColor = foregroundColor; }

    const Color &textColor() const { return mTextColor; }
    void setTextColor(const Color &textColor) { mTextColor = textColor; }

    const VectorXf &values() const { return mValues; }
    VectorXf &values() { return mValues; }
    void setValues(const VectorXf &values) { mValues = values; }

    virtual Vector2i preferredSize(NVGcontext *ctx) const override;
    virtual void draw(NVGcontext *ctx) override;

    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;
protected:
    std::string mCaption, mHeader, mFooter;
    Color mBackgroundColor, mForegroundColor, mTextColor;
    VectorXf mValues;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
