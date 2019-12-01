#include <nanovg.h>
#include <memory>

#if NANOGUI_VULKAN_BACKEND

#include "fill.vert.inc"
#include "fill.frag.inc"
#include "fill_edge_aa.frag.inc"

void vknvg_create_vertshader(uint32_t*& data, uint32_t& dataSize)
{
  dataSize = sizeof(g_VkVertexShader_Main);
  data = new uint32_t[dataSize];
  memcpy(data, g_VkVertexShader_Main, dataSize);
}

void vknvg_create_fragshader(uint32_t*& data, uint32_t& dataSize)
{
  dataSize = sizeof(g_VkPixelShader_Main);
  data = new uint32_t[dataSize];
  memcpy(data, g_VkPixelShader_Main, dataSize);
}

void vknvg_create_fragshader_aa(uint32_t*& data, uint32_t& dataSize)
{
  dataSize = sizeof(g_VkPixelShaderAA_Main);
  data = new uint32_t[dataSize];
  memcpy(data, g_VkPixelShaderAA_Main, dataSize);
}

void vknvg_destroy_vertshader(uint32_t* data) { delete [] data; }
void vknvg_destroy_fragshader(uint32_t* data) { delete [] data; }
void vknvg_destroy_fragshader_aa(uint32_t* data) { delete [] data; }

#endif //NANOGUI_VULKAN_BACKEND