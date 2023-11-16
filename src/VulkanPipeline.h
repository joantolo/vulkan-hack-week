#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include "VulkanTypes.h"

class VulkanPipeline
{
  public:
    VulkanPipeline(VulkanContext *context);
    ~VulkanPipeline();
    void init();
    void drawFrame(const Triangle &triangle) const;

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
    operator VkPipeline() const { return graphicsPipeline; }

  private:
    VulkanContext *context;

    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
};
#endif // VULKAN_PIPELINE_H
