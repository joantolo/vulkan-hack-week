#include <glm/glm.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <cstring>
#include <iostream>

#include "VulkanBuffer.h"
#include "VulkanDevice.h"

#include "Triangle.h"

void Triangle::init(VulkanDevice *device, VulkanBuffer *bufferCreator)
{
    this->device = device;
    this->bufferCreator = bufferCreator;

    createVertexBuffer();
}

void Triangle::clear()
{
    vkDestroyBuffer(*this->device, this->vertexBuffer, nullptr);
    vkFreeMemory(*this->device, this->vertexBufferMemory, nullptr);
}

void Triangle::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(this->vertices[0]) * this->vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    this->bufferCreator->createBuffer(bufferSize,
                                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      stagingBuffer,
                                      stagingBufferMemory);

    void *data;
    vkMapMemory(*this->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, this->vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(*this->device, stagingBufferMemory);

    this->bufferCreator->createBuffer(bufferSize,
                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                      this->vertexBuffer,
                                      this->vertexBufferMemory);

    this->bufferCreator->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(*this->device, stagingBuffer, nullptr);
    vkFreeMemory(*this->device, stagingBufferMemory, nullptr);
}
