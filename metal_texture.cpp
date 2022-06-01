#include "metal_texture.h"

#define MTL_PRIVATE_IMPLEMENTATION
#define NS_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>

MTL::Device *metal_device = nullptr;
MTL::Texture *metal_texture = nullptr;
uint32_t g_frame = 0;

void* createTexture() {
  metal_device = MTL::CreateSystemDefaultDevice();
  MTL::TextureDescriptor *metal_texture_desc = MTL::TextureDescriptor::alloc()->init();
  metal_texture_desc->setWidth(2);
  metal_texture_desc->setHeight(2);
  metal_texture_desc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
  metal_texture_desc->setTextureType(MTL::TextureType2D);
  metal_texture_desc->setStorageMode(MTL::StorageModeManaged);
  metal_texture_desc->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);

  metal_texture = metal_device->newTexture(metal_texture_desc);
  metal_texture_desc->release();
  return metal_texture->retain();
}

void updateTexture() {
  ++g_frame;
  uint32_t data[4] = {
        g_frame,
        g_frame,
        g_frame,
        g_frame,
    };
    metal_texture->replaceRegion(MTL::Region(0, 0, 0, 2, 2, 1), 0, data, 2 * 4);
}