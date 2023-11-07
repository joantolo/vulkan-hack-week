#ifndef VULKAN_SURFACE_H
#define VULKAN_SURFACE_H

#include <vulkan/vulkan.h>

#include "VulkanTypes.h"

class VulkanSurface
{
  public:
    void init(VkInstance *instance, VulkanWindow *window);
    void clear();

  private:
    void createSurface();

  public:
    operator VkSurfaceKHR() const { return surface; }

  private:
    VkSurfaceKHR surface;

    VkInstance *instance;
    VulkanWindow *window;
};
#endif // VULKAN_SURFACE_H
