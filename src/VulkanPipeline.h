#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include "VulkanTypes.h"
#include <vector>

struct FrameInFlight
{
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
};

class VulkanPipeline
{
  public:
    VulkanPipeline(VulkanContext *context);
    ~VulkanPipeline();
    void init();
    void drawFrame(const Triangle &triangle) const;

  private:
    void createPipeline();
    void createFramesInFlight();
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

    std::vector<FrameInFlight> framesInFlight;
};
#endif // VULKAN_PIPELINE_H
