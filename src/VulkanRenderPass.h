#ifndef VULKAN_RENDER_PASS_H
#define VULKAN_RENDER_PASS_H

#include "Triangle.h"
#include "VulkanTypes.h"

class VulkanRenderPass
{
  public:
    void init(VulkanDevice *device,
              VulkanSwapChain *swapChain,
              VulkanPipeline *pipeline);
    void clear();
    void recordCommandBuffer(VkCommandBuffer commandBuffer,
                             Triangle triangle,
                             uint32_t imageIndex);

  private:
    void createRenderPass();
    void createCommandPool();
    void createCommandBuffer();
    void createFrameBuffers();

  public:
    VkCommandPool getCommandPool() const { return commandPool; }
    VkCommandBuffer getCommandBuffer() const { return commandBuffer; }
    operator VkRenderPass() const { return renderPass; }

  private:
    VkRenderPass renderPass;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VulkanDevice *device;
    VulkanSwapChain *swapChain;
    VulkanPipeline *pipeline;
};

#endif // VULKAN_RENDER_PASS_H
