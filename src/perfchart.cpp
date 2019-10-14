#include "nanogui/perfchart.h"
#include <nanovg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

NAMESPACE_BEGIN(nanogui)

PerfGraph::PerfGraph(Widget* parent, int style, const std::string& name, const Vector2i& pos)
  : Widget(parent)
{
	mStyle = style;
	mName = name;

  setPosition(pos);
}

void PerfGraph::update(float frameTime)
{
	mHead = (mHead+1) % GRAPH_HISTORY_COUNT;
	mValues[mHead] = frameTime;
}

Vector2i PerfGraph::preferredSize(NVGcontext *ctx) const
{
  return Vector2i(200, 35);
}

float PerfGraph::getGraphAverage()
{
	int i;
	float avg = 0;
	for (i = 0; i < GRAPH_HISTORY_COUNT; i++) {
		avg += mValues[i];
	}
	return avg / (float)GRAPH_HISTORY_COUNT;
}

void PerfGraph::draw(NVGcontext* vg)
{
	int i;
	float avg, w, h;
	char str[64];

	avg = getGraphAverage();

	w = 200;
	h = 35;

	nvgBeginPath(vg);
	nvgRect(vg, mPos.x(), mPos.y(), w, h);
	nvgFillColor(vg, nvgRGBA(0,0,0,128));
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgMoveTo(vg, mPos.x(), mPos.y()+h);
	if (mStyle == GRAPH_RENDER_FPS) {
		for (i = 0; i < GRAPH_HISTORY_COUNT; i++) {
			float v = 1.0f / (0.00001f + mValues[(mHead+i) % GRAPH_HISTORY_COUNT]);
			float vx, vy;
			if (v > 80.0f) v = 80.0f;
			vx = mPos.x() + ((float)i/(GRAPH_HISTORY_COUNT-1)) * w;
			vy = mPos.y() + h - ((v / 80.0f) * h);
			nvgLineTo(vg, vx, vy);
		}
	} else if (mStyle == GRAPH_RENDER_PERCENT) {
		for (i = 0; i < GRAPH_HISTORY_COUNT; i++) {
			float v = mValues[(mHead+i) % GRAPH_HISTORY_COUNT] * 1.0f;
			float vx, vy;
			if (v > 100.0f) v = 100.0f;
			vx = mPos.x() + ((float)i/(GRAPH_HISTORY_COUNT-1)) * w;
			vy = mPos.y() + h - ((v / 100.0f) * h);
			nvgLineTo(vg, vx, vy);
		}
	} else {
		for (i = 0; i < GRAPH_HISTORY_COUNT; i++) {
			float v = mValues[(mHead+i) % GRAPH_HISTORY_COUNT] * 1000.0f;
			float vx, vy;
			if (v > 20.0f) v = 20.0f;
			vx = mPos.x() + ((float)i/(GRAPH_HISTORY_COUNT-1)) * w;
			vy = mPos.y() + h - ((v / 20.0f) * h);
			nvgLineTo(vg, vx, vy);
		}
	}
	nvgLineTo(vg, mPos.x()+w, mPos.y()+h);
	nvgFillColor(vg, nvgRGBA(255,192,0,128));
	nvgFill(vg);

	nvgFontFace(vg, "sans");

	if (!mName.empty()) {
		nvgFontSize(vg, 14.0f);
		nvgTextAlign(vg, NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
		nvgFillColor(vg, nvgRGBA(240,240,240,192));
		nvgText(vg, mPos.x()+3, mPos.y()+1, mName.c_str(), NULL);
	}

	if (mStyle == GRAPH_RENDER_FPS) {
		nvgFontSize(vg, 18.0f);
		nvgTextAlign(vg,NVG_ALIGN_RIGHT|NVG_ALIGN_TOP);
		nvgFillColor(vg, nvgRGBA(240,240,240,255));
		sprintf(str, "%.2f FPS", 1.0f / avg);
		nvgText(vg, mPos.x()+w-3, mPos.y()+1, str, NULL);

		nvgFontSize(vg, 15.0f);
		nvgTextAlign(vg,NVG_ALIGN_RIGHT|NVG_ALIGN_BOTTOM);
		nvgFillColor(vg, nvgRGBA(240,240,240,160));
		sprintf(str, "%.2f ms", avg * 1000.0f);
		nvgText(vg, mPos.x()+w-3, mPos.y()+h-1, str, NULL);
	}
	else if (mStyle == GRAPH_RENDER_PERCENT) {
		nvgFontSize(vg, 18.0f);
		nvgTextAlign(vg,NVG_ALIGN_RIGHT|NVG_ALIGN_TOP);
		nvgFillColor(vg, nvgRGBA(240,240,240,255));
		sprintf(str, "%.1f %%", avg * 1.0f);
		nvgText(vg, mPos.x()+w-3, mPos.y()+1, str, NULL);
	} else {
		nvgFontSize(vg, 18.0f);
		nvgTextAlign(vg,NVG_ALIGN_RIGHT|NVG_ALIGN_TOP);
		nvgFillColor(vg, nvgRGBA(240,240,240,255));
		sprintf(str, "%.2f ms", avg * 1000.0f);
		nvgText(vg, mPos.x()+w-3, mPos.y()+1, str, NULL);
	}
}

NAMESPACE_END(nanogui)