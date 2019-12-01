#include <nanogui/perfchart.h>

#if NANOGUI_VULKAN_BACKEND

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

int stopGPUTimer(GPUtimer* timer, float* times, int maxTimes)
{
  return 0;
}

NAMESPACE_END(nanogui)

#endif //NANOGUI_VULKAN_BACKEND