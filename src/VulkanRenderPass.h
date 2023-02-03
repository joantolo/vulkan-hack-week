#ifndef VULKAN_RENDER_PASS_H
#define VULKAN_RENDER_PASS_H

#include "Triangle.h"
#include "VulkanTypes.h"

class VulkanRenderPass
{
public:
    void init(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanPipeline* pipeline);
    void clear();
    VkCommandPool getCommandPool() const { return commandPool; }
    VkCommandBuffer getCommandBuffer() const { return commandBuffer; }
    void recordCommandBuffer(VkCommandBuffer commandBuffer, Triangle triangle, uint32_t imageIndex);
    operator VkRenderPass() const { return renderPass; }

private:
    void createRenderPass();

    void createCommandPool();
    void createCommandBuffer();

    void createFrameBuffers();

private:
    VkRenderPass renderPass;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VulkanDevice* device;
    VulkanSwapChain* swapChain;
    VulkanPipeline* pipeline;
};

#endif // VULKAN_RENDER_PASS_H
