#include "nanogui/perfchart.h"
#include "nanogui/opengl.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <glad/glad.h>

#ifndef glGetQueryObjectuiv
#define GL_TIME_ELAPSED                   0x88BF
typedef void (APIENTRY *pfnGLGETQUERYOBJECTUI64V)(GLuint id, GLenum pname, GLuint64* params);
pfnGLGETQUERYOBJECTUI64V glGetQueryObjectui64v = 0;
#endif

NAMESPACE_BEGIN(nanogui)

void initGPUTimer(GPUtimer* timer)
{
	memset(timer, 0, sizeof(*timer));

  timer->supported = glfwExtensionSupported("GL_ARB_timer_query");
  if (timer->supported) {
#ifndef glGetQueryObjectuiv
    glGetQueryObjectui64v = (pfnGLGETQUERYOBJECTUI64V)glfwGetProcAddress("glGetQueryObjectui64v");
#endif
    printf("glGetQueryObjectui64v=%p\n", glGetQueryObjectui64v);
    if (!glGetQueryObjectui64v) {
      timer->supported = GL_FALSE;
      return;
    }
    glGenQueries(GPU_QUERY_COUNT, timer->queries);
  }
}

void startGPUTimer(GPUtimer* timer)
{
	if (!timer->supported)
		return;
	glBeginQuery(GL_TIME_ELAPSED, timer->queries[timer->cur % GPU_QUERY_COUNT] );
	timer->cur++;
}

int stopGPUTimer(GPUtimer* timer, float* times, int maxTimes)
{
	NVG_NOTUSED(times);
	NVG_NOTUSED(maxTimes);
	GLint available = 1;
	int n = 0;
	if (!timer->supported)
		return 0;

	glEndQuery(GL_TIME_ELAPSED);
	while (available && timer->ret <= timer->cur) {
		// check for results if there are any
		glGetQueryObjectiv(timer->queries[timer->ret % GPU_QUERY_COUNT], GL_QUERY_RESULT_AVAILABLE, &available);
    if (available) {
      GLuint64 timeElapsed = 0;
      glGetQueryObjectui64v(timer->queries[timer->ret % GPU_QUERY_COUNT], GL_QUERY_RESULT, &timeElapsed);
      timer->ret++;
      if (n < maxTimes) {
        times[n] = (float)((double)timeElapsed * 1e-9);
        n++;
      }
    }
	}
	return n;
}

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