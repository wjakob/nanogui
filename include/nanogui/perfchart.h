#pragma once

#include "nanogui/widget.h"

NAMESPACE_BEGIN(nanogui)

enum GraphrenderStyle {
    GRAPH_RENDER_FPS,
    GRAPH_RENDER_MS,
    GRAPH_RENDER_PERCENT,
};

#define GRAPH_HISTORY_COUNT 100
class NANOGUI_EXPORT PerfGraph : public Widget
{
public:
  PerfGraph(Widget* parent, int style, const std::string& name, const Vector2i& pos);
  void draw(NVGcontext* ctx) override;

  void update(float frameTime);

  virtual Vector2i preferredSize(NVGcontext *ctx) const;

private:
  float getGraphAverage();

  int mStyle;
  std::string mName;
  float mValues[GRAPH_HISTORY_COUNT] = { 0 };
  int mHead = 0;
};

#define GPU_QUERY_COUNT 5
struct NANOGUI_EXPORT GPUtimer {
  int supported;
  int cur, ret;
  unsigned int queries[GPU_QUERY_COUNT];
};

void NANOGUI_EXPORT initGPUTimer(GPUtimer* timer);
void NANOGUI_EXPORT startGPUTimer(GPUtimer* timer);
int NANOGUI_EXPORT stopGPUTimer(GPUtimer* timer, float* times, int maxTimes);

NAMESPACE_END(nanogui)