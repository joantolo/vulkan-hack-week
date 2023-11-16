#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H

#include "VulkanTypes.h"

class VulkanBufferCreator
{
  public:
    VulkanBufferCreator(VulkanContext *contenxt);
    ~VulkanBufferCreator();
    void init();
    void createBuffer(VkDeviceSize size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory) const;
    void copyBuffer(VkBuffer srcBuffer,
                    VkBuffer dstBuffer,
                    VkDeviceSize size) const;
    VkCommandBuffer createCommandBuffer() const;

  private:
    void createCommandPool();
    uint32_t findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties) const;

  private:
    VulkanContext *context;

    VkCommandPool commandPool;
};

#endif // VULKAN_SURFACE_H
