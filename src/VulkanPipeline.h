#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include "VulkanTypes.h"
#include <vector>
#include <vulkan/vulkan_core.h>

struct Descriptor
{
    VkDescriptorPool pool;
    VkDescriptorSetLayout setLayout;
    uint32_t setsCount;
};

struct UniformBuffers
{
    VkBuffer buffer;
    VkDeviceMemory memory;
    void *mapped;
    VkDescriptorSet descriptorSet;
};

struct FrameInFlight
{
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    UniformBuffers uniformBuffers;
};

class VulkanPipeline
{
  public:
    VulkanPipeline(VulkanContext *context);
    ~VulkanPipeline();
    void init();
    void drawFrame(const Triangle &triangle) const;

  private:
    void createDescriptor();
    void createDescriptorPool();
    void createDescriptorSetLayout();
    void createDescriptorSets(VkDescriptorSet *descriptorSets);
    void updateDescriptorSet(VkDescriptorSet descriptorSet,
                             VkBuffer uniformBuffer);

    void createFramesInFlight();
    void createSyncObjects(VkSemaphore &imageAvailableSemaphore,
                           VkSemaphore &renderFinishedSemaphore,
                           VkFence &inFlightFence);
    void createUniformBuffers(VkBuffer &uniformBuffers,
                              VkDeviceMemory &uniformBuffersMemory,
                              void **uniformBuffersMapped);

    void createPipeline();
    void createPipelineLayout();

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

  public:
    VkPipelineLayout getLayout() const { return pipelineLayout; }
    operator VkPipeline() const { return graphicsPipeline; }

  private:
    VulkanContext *context;

    VkPipeline graphicsPipeline;

    VkPipelineLayout pipelineLayout;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    Descriptor descriptor;

    std::vector<FrameInFlight> framesInFlight;
};
#endif // VULKAN_PIPELINE_H
