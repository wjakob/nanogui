/*

Port to dx12 by megai2

*/
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

int stopGPUTimer(GPUtimer* timer, float* times, int maxTimes)
{
  return 0;
}

NAMESPACE_END(nanogui)
