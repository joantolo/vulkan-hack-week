#include <vulkan/vulkan.h>

#include "VulkanContext.h"

#include "Triangle.h"

Triangle::Triangle(VulkanContext *context) : context(context) {}

Triangle::~Triangle()
{
    VkDevice device = context->getDevice();

    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);

    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);
}

void Triangle::init()
{
    createVertexBuffer();
    createIndexBuffer();
}

void Triangle::createVertexBuffer()
{
    context->getBufferCreator().createStagingBuffer(
        vertices.data(),
        sizeof(vertices[0]) * vertices.size(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        vertexBuffer,
        vertexBufferMemory);
}

void Triangle::createIndexBuffer()
{
    context->getBufferCreator().createStagingBuffer(
        indices.data(),
        sizeof(indices[0]) * indices.size(),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        indexBuffer,
        indexBufferMemory);
}
