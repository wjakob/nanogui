#include "python.h"
#include <nanovg.h>

using namespace pybind11::literals;

void register_nanovg(py::module &m) {
    /* NanoVG */

    /* To be done: docstrings, functions taking memory arguments (fonts, images) */

    py::module nvg = m.def_submodule("nanovg", "NanoVG bindings");

    py::enum_<NVGwinding>(nvg, "NVGwinding")
        .value("CCW", NVG_CCW)
        .value("CW", NVG_CW)
        .export_values();

    py::enum_<NVGsolidity>(nvg, "NVGsolidity")
        .value("SOLID", NVG_SOLID)
        .value("HOLE", NVG_HOLE)
        .export_values();

    py::enum_<NVGlineCap>(nvg, "NVGlineCap")
        .value("BUTT", NVG_BUTT)
        .value("ROUND", NVG_ROUND)
        .value("SQUARE", NVG_SQUARE)
        .value("BEVEL", NVG_BEVEL)
        .value("MITER", NVG_MITER)
        .export_values();

    py::enum_<NVGalign>(nvg, "NVGalign")
        .value("ALIGN_LEFT", NVG_ALIGN_LEFT)
        .value("ALIGN_CENTER", NVG_ALIGN_CENTER)
        .value("ALIGN_RIGHT", NVG_ALIGN_RIGHT)
        .value("ALIGN_TOP", NVG_ALIGN_TOP)
        .value("ALIGN_MIDDLE", NVG_ALIGN_MIDDLE)
        .value("ALIGN_BOTTOM", NVG_ALIGN_BOTTOM)
        .value("ALIGN_BASELINE", NVG_ALIGN_BASELINE)
        .export_values();

    py::enum_<NVGblendFactor>(nvg, "NVGblendFactor")
        .value("ZERO", NVG_ZERO)
        .value("ONE", NVG_ONE)
        .value("SRC_COLOR", NVG_SRC_COLOR)
        .value("ONE_MINUS_SRC_COLOR", NVG_ONE_MINUS_SRC_COLOR)
        .value("DST_COLOR", NVG_DST_COLOR)
        .value("ONE_MINUS_DST_COLOR", NVG_ONE_MINUS_DST_COLOR)
        .value("SRC_ALPHA", NVG_SRC_ALPHA)
        .value("ONE_MINUS_SRC_ALPHA", NVG_ONE_MINUS_SRC_ALPHA)
        .value("DST_ALPHA", NVG_DST_ALPHA)
        .value("ONE_MINUS_DST_ALPHA", NVG_ONE_MINUS_DST_ALPHA)
        .value("SRC_ALPHA_SATURATE", NVG_SRC_ALPHA_SATURATE)
        .export_values();

    py::enum_<NVGcompositeOperation>(nvg, "NVGcompositeOperation")
        .value("SOURCE_OVER", NVG_SOURCE_OVER)
        .value("SOURCE_IN", NVG_SOURCE_IN)
        .value("SOURCE_OUT", NVG_SOURCE_OUT)
        .value("ATOP", NVG_ATOP)
        .value("DESTINATION_OVER", NVG_DESTINATION_OVER)
        .value("DESTINATION_IN", NVG_DESTINATION_IN)
        .value("DESTINATION_OUT", NVG_DESTINATION_OUT)
        .value("DESTINATION_ATOP", NVG_DESTINATION_ATOP)
        .value("LIGHTER", NVG_LIGHTER)
        .value("COPY", NVG_COPY)
        .value("XOR", NVG_XOR)
        .export_values();

    py::class_<NVGpaint>(nvg, "NVGpaint");

    py::class_<NVGcolor>(nvg, "NVGcolor")
        .def("__init__",
             [](NVGcolor &c, const Color &c2) { new (&c) NVGcolor(c2); }
        );

    py::implicitly_convertible<Color, NVGcolor>();

    nvg.def("RGB", &nvgRGB);
    nvg.def("RGBf", &nvgRGBf);
    nvg.def("RGBA", &nvgRGBA);
    nvg.def("RGBAf", &nvgRGBAf);

    nvg.def("LerpRGBA", &nvgLerpRGBA);
    nvg.def("TransRGBA", &nvgTransRGBA);
    nvg.def("TransRGBAf", &nvgTransRGBAf);

    nvg.def("HSL", &nvgHSL);
    nvg.def("HSLA", &nvgHSLA);

    //nvg.def("TextGlyphPositions", &nvgTextBounds);
    //nvg.def("TextGlyphMetrics", &nvgTextBounds);
    //nvg.def("TextTextBreakLines", &nvgTextBounds);

    py::class_<NVGcontext>(nvg, "NVGcontext")
        .def("GlobalCompositeOperation", &nvgGlobalCompositeOperation,
             "factor"_a)
        .def("GlobalCompositeBlendFunc", &nvgGlobalCompositeBlendFunc,
             "sfactor"_a, "dfactor"_a)
        .def("GlobalCompositeBlendFuncSeparate",
             &nvgGlobalCompositeBlendFuncSeparate, "srcRGB"_a, "dstRGB"_a,
             "srcAlpha"_a, "dstAlpha"_a)
        .def("Save", &nvgSave)
        .def("Restore", &nvgRestore)
        .def("Reset", &nvgReset)
        .def("StrokeColor", &nvgStrokeColor, "color"_a)
        .def("StrokePaint", &nvgStrokePaint, "paint"_a)
        .def("FillColor", &nvgFillColor, "color"_a)
        .def("FillPaint", &nvgFillPaint, "paint"_a)
        .def("MiterLimit", &nvgMiterLimit, "limit"_a)
        .def("StrokeWidth", &nvgStrokeWidth, "size"_a)
        .def("LineCap", &nvgLineCap, "cap"_a)
        .def("LineJoin", &nvgLineJoin, "join"_a)
        .def("GlobalAlpha", &nvgGlobalAlpha, "alpha"_a)
        .def("ResetTransform", &nvgResetTransform)
        .def("Transform", &nvgTransform, "a"_a, "b"_a, "c"_a, "d"_a, "e"_a,
             "f"_a)
        .def("Translate", (void(*)(NVGcontext*, float, float))&nvgTranslate, "x"_a, "y"_a)
        .def("Rotate", &nvgRotate, "angle"_a)
        .def("SkewX", &nvgSkewX, "angle"_a)
        .def("SkewY", &nvgSkewY, "angle"_a)
        .def("Scale", &nvgScale, "x"_a, "y"_a)
        .def("CreateImage", &nvgCreateImage, "filename"_a, "imageFlags"_a)
        .def("DeleteImage", &nvgDeleteImage, "image"_a)
        .def("LinearGradient", &nvgLinearGradient, "sx"_a, "sy"_a, "ex"_a,
             "ey"_a, "icol"_a, "ocol"_a)
        .def("BoxGradient", &nvgBoxGradient, "x"_a, "y"_a, "w"_a, "h"_a, "r"_a,
             "f"_a, "icol"_a, "ocol"_a)
        .def("RadialGradient", &nvgRadialGradient, "cx"_a, "cy"_a, "inr"_a,
             "outr"_a, "icol"_a, "ocol"_a)
        .def("ImagePattern", &nvgImagePattern, "ox"_a, "oy"_a, "ex"_a, "ey"_a,
             "angle"_a, "image"_a, "alpha"_a)
        .def("Scissor", &nvgScissor, "x"_a, "y"_a, "w"_a, "h"_a)
        .def("IntersectScissor", &nvgIntersectScissor, "x"_a, "y"_a, "w"_a,
             "h"_a)
        .def("ResetScissor", &nvgResetScissor)
        .def("BeginPath", &nvgBeginPath)
        .def("MoveTo", (void(*)(NVGcontext*, float, float))&nvgMoveTo, "x"_a, "y"_a)
        .def("LineTo", (void(*)(NVGcontext*, float, float))&nvgLineTo, "x"_a, "y"_a)
        .def("BezierTo", &nvgBezierTo, "c1x"_a, "c1y"_a, "c2x"_a, "c2y"_a,
             "x"_a, "y"_a)
        .def("QuadTo", &nvgQuadTo, "cx"_a, "cy"_a, "x"_a, "y"_a)
        .def("ArcTo", &nvgArcTo, "x1"_a, "y1"_a, "x2"_a, "y2"_a, "radius"_a)
        .def("ClosePath", &nvgClosePath)
        .def("PathWinding", &nvgPathWinding, "dir"_a)
        .def("Arc", (void(*)(NVGcontext*, float, float, float, float, float, int))&nvgArc, "cx"_a, "cy"_a, "r"_a, "a0"_a, "a1"_a, "dir"_a)
        .def("Rect", (void(*)(NVGcontext*,float,float,float,float))&nvgRect, "x"_a, "y"_a, "w"_a, "h"_a)
        .def("RoundedRect", (void(*)(NVGcontext*, float, float, float, float, float))&nvgRoundedRect, "x"_a, "y"_a, "w"_a, "h"_a, "r"_a)
        .def("RoundedRectVarying", &nvgRoundedRectVarying, "x"_a, "y"_a, "w"_a,
             "h"_a, "radTopLeft"_a, "radTopRight"_a, "radBottomRight"_a,
             "radBottomLeft"_a)
        .def("Ellipse", &nvgEllipse, "cx"_a, "cy"_a, "rx"_a, "ry"_a)
        .def("Circle", (void(*)(NVGcontext*, float, float, float))&nvgCircle, "cx"_a, "cy"_a, "r"_a)
        .def("Fill", &nvgFill)
        .def("Stroke", &nvgStroke)
        .def("CreateFont", &nvgCreateFont, "name"_a, "filename"_a)
        .def("FindFont", &nvgFindFont, "name"_a)
        .def("AddFallbackFontId", &nvgAddFallbackFontId, "baseFont"_a,
             "fallbackFont"_a)
        .def("AddFallbackFont", &nvgAddFallbackFont, "baseFont"_a,
             "fallbackFont"_a)
        .def("FontSize", &nvgFontSize, "size"_a)
        .def("FontBlur", &nvgFontBlur, "blur"_a)
        .def("TextLetterSpacing", &nvgTextLetterSpacing, "spacing"_a)
        .def("TextLineHeight", &nvgTextLineHeight, "lineHeight"_a)
        .def("TextAlign", &nvgTextAlign, "align"_a)
        .def("FontFaceId", &nvgFontFaceId, "font"_a)
        .def("FontFace", &nvgFontFace, "font"_a)
        .def("Text", [](NVGcontext *ctx, float x, float y,
                        const char *str) { nvgText(ctx, x, y, str, nullptr); },
             "x"_a, "y"_a, "string"_a)
        .def("TextBounds",
             [](NVGcontext *ctx, float x, float y, const char *str) {
                 float bounds[4];
                 nvgTextBounds(ctx, x, y, str, nullptr, bounds);
                 return std::make_tuple(bounds[0], bounds[1], bounds[2], bounds[3]);
             }, "x"_a, "y"_a, "string"_a)
        .def("TextBox",
             [](NVGcontext *ctx, float x, float y, float breakRowWidth,
                const char *str) {
                 nvgTextBox(ctx, x, y, breakRowWidth, str, nullptr);
             },
             "x"_a, "y"_a, "breakRowWidth"_a, "string"_a)
        .def("TextBoxBounds",
             [](NVGcontext *ctx, float x, float y, float breakRowWidth,
                const char *str) {
                 float bounds[4];
                 nvgTextBoxBounds(ctx, x, y, breakRowWidth, str, nullptr, bounds);
                 return std::make_tuple(bounds[0], bounds[1], bounds[2], bounds[3]);
             },
             "x"_a, "y"_a, "breakRowWidth"_a, "string"_a)
        .def("BeginFrame", &nvgBeginFrame, "windowWidth"_a, "windowHeight"_a,
             "devicePixelRatio"_a)
        .def("CancelFrame", &nvgCancelFrame)
        .def("EndFrame", &nvgEndFrame);
}
