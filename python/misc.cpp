#ifdef NANOGUI_PYTHON

#include "python.h"

DECLARE_WIDGET(ColorWheel);
DECLARE_WIDGET(ColorPicker);
DECLARE_WIDGET(Graph);
DECLARE_WIDGET(ImageView);
DECLARE_WIDGET(ImagePanel);

void register_misc(py::module &m) {
    py::class_<ColorWheel, Widget, ref<ColorWheel>, PyColorWheel>(m, "ColorWheel", D(ColorWheel))
        .def(py::init<Widget *>(), py::arg("parent"), D(ColorWheel, ColorWheel))
        .def(py::init<Widget *, const Color &>(), py::arg("parent"), py::arg("Color"))
        .def("color", &ColorWheel::color, D(ColorWheel, color))
        .def("setColor", &ColorWheel::setColor, D(ColorWheel, setColor))
        .def("callback", &ColorWheel::callback, D(ColorWheel, callback))
        .def("setCallback", &ColorWheel::setCallback, D(ColorWheel, setCallback));

    py::class_<ColorPicker, PopupButton, ref<ColorPicker>, PyColorPicker>(m, "ColorPicker", D(ColorPicker))
        .def(py::init<Widget *>(), py::arg("parent"), D(ColorPicker, ColorPicker))
        .def(py::init<Widget *, const Color &>(), py::arg("parent"), py::arg("Color"))
        .def("color", &ColorPicker::color, D(ColorPicker, color))
        .def("setColor", &ColorPicker::setColor, D(ColorPicker, setColor))
        .def("callback", &ColorPicker::callback, D(ColorPicker, callback))
        .def("setCallback", &ColorPicker::setCallback, D(ColorPicker, setCallback))
        .def("finalCallback", &ColorPicker::finalCallback, D(ColorPicker, finalCallback))
        .def("setFinalCallback", &ColorPicker::setFinalCallback, D(ColorPicker, setFinalCallback));

    py::class_<Graph, FontWidget, ref<Graph>, PyGraph>(m, "Graph", D(Graph))
        .def(py::init<Widget *, const std::string &, const std::string &>(), py::arg("parent"),
             py::arg("caption") = std::string("Untitled"), py::arg("font") = std::string(""),
             D(Graph, Graph))
        .def("caption", &Graph::caption, D(Graph, caption))
        .def("setCaption", &Graph::setCaption, D(Graph, setCaption))
        .def("header", &Graph::header, D(Graph, header))
        .def("setHeader", &Graph::setHeader, D(Graph, setHeader))
        .def("footer", &Graph::footer, D(Graph, footer))
        .def("setFooter", &Graph::setFooter, D(Graph, setFooter))
        .def("backgroundColor", &Graph::backgroundColor, D(Graph, backgroundColor))
        .def("setBackgroundColor", &Graph::setBackgroundColor, D(Graph, setBackgroundColor))
        .def("foregroundColor", &Graph::foregroundColor, D(Graph, foregroundColor))
        .def("setForegroundColor", &Graph::setForegroundColor, D(Graph, setForegroundColor))
        .def("textColor", &Graph::textColor, D(Graph, textColor))
        .def("setTextColor", &Graph::setTextColor, D(Graph, setTextColor))
        .def("values", (VectorXf &(Graph::*)(void)) &Graph::values, D(Graph, values))
        .def("setValues", &Graph::setValues, D(Graph, setValues));

    py::class_<ImageView, FontWidget, ref<ImageView>, PyImageView>(m, "ImageView", D(ImageView))
        .def(py::init<Widget *, GLuint>(), D(ImageView, ImageView))
        .def("bindImage", &ImageView::bindImage, D(ImageView, bindImage))
        .def("imageShader", &ImageView::imageShader, D(ImageView, imageShader))
        .def("imageSize", &ImageView::imageSize, D(ImageView, imageSize))
        .def("scaledImageSize", &ImageView::scaledImageSize, D(ImageView, scaledImageSize))
        .def("offset", &ImageView::offset, D(ImageView, offset))
        .def("setOffset", &ImageView::setOffset, D(ImageView, setOffset))
        .def("scale", &ImageView::scale, D(ImageView, scale))
        .def("fixedOffset", &ImageView::fixedOffset, D(ImageView, fixedOffset))
        .def("setFixedOffset", &ImageView::setFixedOffset, D(ImageView, setFixedOffset))
        .def("fixedScale", &ImageView::fixedScale, D(ImageView, fixedScale))
        .def("setFixedScale", &ImageView::setFixedScale, D(ImageView, setFixedScale))
        .def("zoomSensitivity", &ImageView::zoomSensitivity, D(ImageView, zoomSensitivity))
        .def("setZoomSensitivity", &ImageView::setZoomSensitivity, D(ImageView, setZoomSensitivity))
        .def("gridThreshold", &ImageView::gridThreshold, D(ImageView, gridThreshold))
        .def("setGridThreshold", &ImageView::setGridThreshold, D(ImageView, setGridThreshold))
        .def("pixelInfoThreshold", &ImageView::pixelInfoThreshold, D(ImageView, pixelInfoThreshold))
        .def("setPixelInfoThreshold", &ImageView::setPixelInfoThreshold, D(ImageView, setPixelInfoThreshold))
        .def("setPixelInfoCallback", &ImageView::setPixelInfoCallback, D(ImageView, setPixelInfoCallback))
        .def("pixelInfoCallback", &ImageView::pixelInfoCallback, D(ImageView, pixelInfoCallback))
        .def("setFontScaleFactor", &ImageView::setFontScaleFactor, D(ImageView, setFontScaleFactor))
        .def("fontScaleFactor", &ImageView::fontScaleFactor, D(ImageView, fontScaleFactor))
        .def("imageCoordinateAt", &ImageView::imageCoordinateAt, D(ImageView, imageCoordinateAt))
        .def("clampedImageCoordinateAt", &ImageView::clampedImageCoordinateAt, D(ImageView, clampedImageCoordinateAt))
        .def("positionForCoordinate", &ImageView::positionForCoordinate, D(ImageView, positionForCoordinate))
        .def("setImageCoordinateAt", &ImageView::setImageCoordinateAt, D(ImageView, setImageCoordinateAt))
        .def("center",&ImageView::center, D(ImageView, center))
        .def("fit", &ImageView::fit, D(ImageView, fit))
        .def("setScaleCentered", &ImageView::setScaleCentered, D(ImageView, setScaleCentered))
        .def("moveOffset", &ImageView::moveOffset, D(ImageView, moveOffset))
        .def("zoom", &ImageView::zoom, D(ImageView, zoom))
        .def("gridVisible", &ImageView::gridVisible, D(ImageView, gridVisible))
        .def("pixelInfoVisible", &ImageView::pixelInfoVisible, D(ImageView, pixelInfoVisible))
        .def("helpersVisible", &ImageView::helpersVisible, D(ImageView, helpersVisible));

    py::class_<ImagePanel, Widget, ref<ImagePanel>, PyImagePanel>(m, "ImagePanel", D(ImagePanel))
        .def(py::init<Widget *>(), py::arg("parent"), D(ImagePanel, ImagePanel))
        .def("images", &ImagePanel::images, D(ImagePanel, images))
        .def("setImages", &ImagePanel::setImages, D(ImagePanel, setImages))
        .def("callback", &ImagePanel::callback, D(ImagePanel, callback))
        .def("setCallback", &ImagePanel::setCallback, D(ImagePanel, setCallback));
}

#endif
