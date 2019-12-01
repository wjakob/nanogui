/*

Port to dx12 by megai2

*/
#include <nanogui/perfchart.h>

#if NANOGUI_DX12_BACKEND

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

#endif //NANOGUI_DX12_BACKEND