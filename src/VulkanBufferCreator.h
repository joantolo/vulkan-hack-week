#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H

#include "VulkanTypes.h"

class VulkanBufferCreator
{
  public:
    VulkanBufferCreator(VulkanContext *contenxt);
    ~VulkanBufferCreator();
    void init();
    void createStagingBuffer(const void *bytes,
                             VkDeviceSize size,
                             VkBufferUsageFlags usage,
                             VkBuffer &buffer,
                             VkDeviceMemory &bufferMemory) const;
    void createCommandBuffers(VkCommandBuffer *commandBuffers,
                              uint32_t count) const;

  private:
    void createCommandPool();
    void createBuffer(VkDeviceSize size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory) const;
    void copyBuffer(VkBuffer srcBuffer,
                    VkBuffer dstBuffer,
                    VkDeviceSize size) const;
    uint32_t findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties) const;

  private:
    VulkanContext *context;

    VkCommandPool commandPool;
};

#endif // VULKAN_SURFACE_H
