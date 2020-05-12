#include <nanogui/perfchart.h>

NAMESPACE_BEGIN(nanogui)

void initGPUTimer(GPUtimer* timer)
{
  memset(timer, 0, sizeof(*timer));
}

void startGPUTimer(GPUtimer* timer)
{
  if (!timer->supported)
    return;
}

int stopGPUTimer(GPUtimer*, float*, int)
{
  return 0;
}

NAMESPACE_END(nanogui)
