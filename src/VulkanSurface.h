#ifndef VULKAN_SURFACE_H
#define VULKAN_SURFACE_H

#include "VulkanTypes.h"

class VulkanSurface
{
  public:
    VulkanSurface(VulkanContext *context);
    ~VulkanSurface();
    void init();

  public:
    operator VkSurfaceKHR() const { return surface; }

  private:
    VulkanContext *context;

    VkSurfaceKHR surface;
};
#endif // VULKAN_SURFACE_H
