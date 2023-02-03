#ifndef VULKAN_SURFACE_H
#define VULKAN_SURFACE_H

#include <vulkan/vulkan.h>

#include "VulkanTypes.h"

class VulkanSurface
{
public:
    void init(VkInstance* instance, GlfwWindow* window);
    void clear();
    operator VkSurfaceKHR() const { return surface; }

private:
    void createSurface();

private:
    VkSurfaceKHR surface;

    VkInstance* instance;
    GlfwWindow* window;
};
#endif // VULKAN_SURFACE_H
