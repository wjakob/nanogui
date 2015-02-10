#include <nanogui/progressbar.h>
#include <nanogui/opengl.h>

NANOGUI_NAMESPACE_BEGIN

ProgressBar::ProgressBar(Widget *parent)
	: Widget(parent), mValue(0.0f) {}

Vector2i ProgressBar::preferredSize(NVGcontext *ctx) const {
	return Vector2i(70, 12);
}

void ProgressBar::draw(NVGcontext* ctx) {
	Widget::draw(ctx);

	NVGpaint paint = nvgBoxGradient(
		ctx, mPos.x() + 1, mPos.y() + 1,
		mSize.x()-2, mSize.y(), 3, 4, Color(0, 32), Color(0, 92));
	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), 3);
	nvgFillPaint(ctx, paint);
	nvgFill(ctx);

	float value = std::min(std::max(0.0f, mValue), 1.0f);
	int barPos = (int) std::round((mSize.x() - 2) * value);

	paint = nvgBoxGradient(
		ctx, mPos.x(), mPos.y(),
		barPos+1.5f, mSize.y()-1, 3, 4,
		Color(220, 100), Color(128, 100));

	nvgBeginPath(ctx);
	nvgRoundedRect(
		ctx, mPos.x()+1, mPos.y()+1,
		barPos, mSize.y()-2, 3);
	nvgFillPaint(ctx, paint);
	nvgFill(ctx);
}

NANOGUI_NAMESPACE_END
