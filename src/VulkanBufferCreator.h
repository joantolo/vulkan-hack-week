#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H

#include "VulkanTypes.h"

class VulkanBufferCreator
{
  public:
    VulkanBufferCreator(VulkanContext *contenxt);
    static void createBuffer(VkDeviceSize size,
                             VkBufferUsageFlags usage,
                             VkMemoryPropertyFlags properties,
                             VkBuffer &buffer,
                             VkDeviceMemory &bufferMemory);
    static uint32_t findMemoryType(uint32_t typeFilter,
                                   VkMemoryPropertyFlags properties);
    static void copyBuffer(VkBuffer srcBuffer,
                           VkBuffer dstBuffer,
                           VkDeviceSize size);

  private:
    static VulkanContext *context;
};

#endif // VULKAN_SURFACE_H
