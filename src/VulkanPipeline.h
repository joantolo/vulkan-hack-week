#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include <vulkan/vulkan.h>

#include <vector>

#include "VulkanBuffer.h"
#include "VulkanRenderPass.h"

#include "VulkanTypes.h"

class VulkanPipeline
{
  public:
    void init(VulkanDevice *device,
              VulkanSwapChain *swapChain,
              Triangle *triangle);
    void clear();
    void drawFrame();

  private:
    void createPipeline();
    void createSyncObjects();
    std::vector<VkPipelineShaderStageCreateInfo> createShaders();
    VkShaderModule createShaderModule(const std::vector<char> &code);
    VkPipelineVertexInputStateCreateInfo createVertexInputInfo(
        const VkVertexInputBindingDescription &bindingDescription,
        const std::vector<VkVertexInputAttributeDescription>
            &attributeDescriptions);
    VkPipelineInputAssemblyStateCreateInfo createInputAssembly();
    VkPipelineViewportStateCreateInfo createViewportState();
    VkPipelineRasterizationStateCreateInfo createRasterizer();
    VkPipelineMultisampleStateCreateInfo createMultisampling();
    VkPipelineColorBlendAttachmentState createColorBlendAttachment();
    VkPipelineColorBlendStateCreateInfo createColorBlending(
        VkPipelineColorBlendAttachmentState *colorBlendAttachment);
    VkPipelineDynamicStateCreateInfo createDynamicState();
    VkPipelineLayout createPipelineLayout();

  public:
    VulkanBuffer &getBufferCreator() { return bufferCreator; }
    operator VkPipeline() const { return graphicsPipeline; }

  private:
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

    VulkanRenderPass renderPass;
    VulkanBuffer bufferCreator;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    VulkanDevice *device;
    VulkanSwapChain *swapChain;

    Triangle *triangle;
};
#endif // VULKAN_PIPELINE_H
