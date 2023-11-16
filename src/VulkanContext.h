#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "VulkanWindow.h"

class VulkanContext
{
  public:
    VulkanContext();
    ~VulkanContext();
    void init();

  public:
    const VkInstance &getInstance() const { return instance; };
    const VulkanWindow &getWindow() const { return window; };
    const VulkanSurface &getSurface() const { return surface; };
    const VulkanDevice &getDevice() const { return device; };
    const VulkanSwapChain &getSwapChain() const { return swapChain; };
    const VulkanRenderPass &getRenderPass() const { return renderPass; };
    const VulkanPipeline &getPipeline() const { return pipeline; };

  private:
    VkInstance instance;
    VulkanWindow window;
    VulkanSurface surface;
    VulkanDevice device;
    VulkanSwapChain swapChain;
    VulkanRenderPass renderPass;
    VulkanPipeline pipeline;
};
#endif // VULKAN_CONTEXT_H
