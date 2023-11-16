#ifndef VULKAN_RENDER_PASS_H
#define VULKAN_RENDER_PASS_H

#include "VulkanTypes.h"

class VulkanRenderPass
{
  public:
    VulkanRenderPass(VulkanContext *context);
    ~VulkanRenderPass();
    void init();
    void recordCommandBuffer(const VkCommandBuffer &commandBuffer,
                             const Triangle &triangle,
                             uint32_t imageIndex) const;

  private:
    void createRenderPass();
    void createCommandPool();
    void createCommandBuffer();

  public:
    const VkCommandPool &getCommandPool() const { return commandPool; }
    const VkCommandBuffer &getCommandBuffer() const { return commandBuffer; }
    operator VkRenderPass() const { return renderPass; }

  private:
    VulkanContext *context;

    VkRenderPass renderPass;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
};

#endif // VULKAN_RENDER_PASS_H
