#include <vulkan/vulkan.h>

#include <cstring>

#include "VulkanContext.h"

#include "Triangle.h"

Triangle::Triangle(VulkanContext *context) : context(context) {}

Triangle::~Triangle()
{
    VkDevice device = context->getDevice();

    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);
}

void Triangle::init()
{
    createVertexBuffer();
}

void Triangle::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    const VulkanBufferCreator &bufferCreator = context->getBufferCreator();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    bufferCreator.createBuffer(bufferSize,
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                               stagingBuffer,
                               stagingBufferMemory);

    VkDevice device = context->getDevice();
    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    bufferCreator.createBuffer(bufferSize,
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               vertexBuffer,
                               vertexBufferMemory);

    bufferCreator.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}
