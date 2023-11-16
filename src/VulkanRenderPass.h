#ifndef VULKAN_RENDER_PASS_H
#define VULKAN_RENDER_PASS_H

#include "VulkanTypes.h"

class VulkanRenderPass
{
  public:
    VulkanRenderPass(VulkanContext *context);
    ~VulkanRenderPass();
    void init();
    void recordCommandBuffer(const Triangle &triangle,
                             uint32_t imageIndex) const;
    void resetCommandBuffer() const;

  private:
    void createRenderPass();
    void createCommandBuffer();

  public:
    const VkCommandBuffer &getCommandBuffer() const { return commandBuffer; }
    operator VkRenderPass() const { return renderPass; }

  private:
    VulkanContext *context;

    VkRenderPass renderPass;
    VkCommandBuffer commandBuffer;
};

#endif // VULKAN_RENDER_PASS_H
