#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <cstring>
#include <iostream>

#include "VulkanContext.h"

#include "VulkanBufferCreator.h"

VulkanBufferCreator::VulkanBufferCreator(VulkanContext *context)
    : context(context)
{
}

VulkanBufferCreator::~VulkanBufferCreator()
{
    vkDestroyCommandPool(context->getDevice(), commandPool, nullptr);
}

void VulkanBufferCreator::init()
{
    createCommandPool();
}

void VulkanBufferCreator::createStagingBuffer(
    const void *bytes,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkBuffer &buffer,
    VkDeviceMemory &bufferMemory) const
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(size,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer,
                 stagingBufferMemory);

    VkDevice device = context->getDevice();
    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, size, 0, &data);
    memcpy(data, bytes, (size_t)size);
    vkUnmapMemory(device, stagingBufferMemory);

    createBuffer(size,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 buffer,
                 bufferMemory);

    copyBuffer(stagingBuffer, buffer, size);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void VulkanBufferCreator::createBuffer(VkDeviceSize size,
                                       VkBufferUsageFlags usage,
                                       VkMemoryPropertyFlags properties,
                                       VkBuffer &buffer,
                                       VkDeviceMemory &bufferMemory) const
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    const VulkanDevice &device = context->getDevice();
    VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to create buffer: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to allocate buffer memory: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

uint32_t VulkanBufferCreator::findMemoryType(
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(
        context->getDevice().getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) ==
                properties)
        {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

void VulkanBufferCreator::copyBuffer(VkBuffer srcBuffer,
                                     VkBuffer dstBuffer,
                                     VkDeviceSize size) const
{
    VkCommandBuffer commandBuffer;
    createCommandBuffers(&commandBuffer, 1);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    const VulkanDevice &device = context->getDevice();
    vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.getGraphicsQueue());

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void VulkanBufferCreator::createCommandPool()
{
    const VulkanDevice &device = context->getDevice();

    const QueueFamilyIndices &queueFamilyIndices =
        device.getQueueFamiyIndices();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VkResult result =
        vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to create command pool: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }
}

void VulkanBufferCreator::createCommandBuffers(VkCommandBuffer *commandBuffers,
                                               uint32_t count) const
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    VkResult result = vkAllocateCommandBuffers(
        context->getDevice(), &allocInfo, commandBuffers);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to allocate command buffer: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }
}
