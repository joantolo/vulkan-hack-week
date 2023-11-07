#ifndef VULKAN_APP_H
#define VULKAN_APP_H

#include <vulkan/vulkan.h>

#include <vector>

#include "Triangle.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "VulkanWindow.h"

class VulkanApp
{
  public:
    void run();

  private:
    void init();
    void mainLoop();
    void clear();

  private:
    VkInstance instance;

    VulkanWindow window;
    VulkanSurface surface;
    VulkanDevice device;
    VulkanSwapChain swapChain;
    VulkanPipeline pipeline;

    Triangle triangle;
};
#endif // VULKAN_APP_H
