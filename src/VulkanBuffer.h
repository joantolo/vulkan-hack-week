#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H

#include <vulkan/vulkan.h>

#include "VulkanTypes.h"

class VulkanBuffer
{
  public:
    void init(VulkanDevice *device, VulkanRenderPass *renderPass);
    void clear();
    void createBuffer(VkDeviceSize size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  private:
    VulkanDevice *device;
    VulkanRenderPass *renderPass;
};
#endif // VULKAN_SURFACE_H
