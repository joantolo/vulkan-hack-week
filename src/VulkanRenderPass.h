#ifndef VULKAN_RENDER_PASS_H
#define VULKAN_RENDER_PASS_H

#include "VulkanTypes.h"

class VulkanRenderPass
{
  public:
    VulkanRenderPass(VulkanContext *context);
    ~VulkanRenderPass();
    void init();
    void recordCommandBuffer(VkCommandBuffer commandBuffer,
                             const Triangle &triangle,
                             uint32_t imageIndex) const;

  private:
    void createRenderPass();

  public:
    operator VkRenderPass() const { return renderPass; }

  private:
    VulkanContext *context;

    VkRenderPass renderPass;
};

#endif // VULKAN_RENDER_PASS_H
