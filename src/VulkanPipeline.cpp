#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <cstring>
#include <iostream>

#include "config.h"

#include "VulkanContext.h"
#include "utils.h"

#include "VulkanPipeline.h"

static const int MAX_FRAMES_IN_FLIGHT = 2;

static const std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR};

VulkanPipeline::VulkanPipeline(VulkanContext *context)
    : context(context),
      framesInFlight(MAX_FRAMES_IN_FLIGHT)
{
}

VulkanPipeline::~VulkanPipeline()
{
    VkDevice device = context->getDevice();

    for (const auto &frameInFlight : framesInFlight)
    {
        vkDestroySemaphore(
            device, frameInFlight.imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(
            device, frameInFlight.renderFinishedSemaphore, nullptr);
        vkDestroyFence(device, frameInFlight.inFlightFence, nullptr);

        vkDestroyBuffer(device, frameInFlight.uniformBuffers.buffer, nullptr);
        vkFreeMemory(device, frameInFlight.uniformBuffers.memory, nullptr);
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);

    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

    vkDestroyDescriptorPool(device, descriptor.pool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptor.setLayout, nullptr);
}

void VulkanPipeline::init()
{
    createDescriptor();

    createFramesInFlight();

    createPipeline();
}

void VulkanPipeline::drawFrame(const Triangle &triangle) const
{
    const VulkanDevice &device = context->getDevice();

    static uint32_t currentFrameIndex = 0;
    const FrameInFlight &currentFrame = framesInFlight[currentFrameIndex];

    vkWaitForFences(
        device, 1, &currentFrame.inFlightFence, VK_TRUE, UINT64_MAX);

    const VulkanSwapChain &swapChain = context->getSwapChain();

    uint32_t imageIndex;
    VkResult result =
        vkAcquireNextImageKHR(device,
                              swapChain,
                              UINT64_MAX,
                              currentFrame.imageAvailableSemaphore,
                              VK_NULL_HANDLE,
                              &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        const_cast<VulkanSwapChain &>(swapChain).recreate();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Only reset the fence if we are submitting work
    vkResetFences(device, 1, &currentFrame.inFlightFence);

    const VulkanRenderPass &renderPass = context->getRenderPass();

    vkResetCommandBuffer(currentFrame.commandBuffer, 0);
    renderPass.recordCommandBuffer(currentFrame.commandBuffer,
                                   &currentFrame.uniformBuffers.descriptorSet,
                                   triangle,
                                   imageIndex);

    UniformBufferObject ubo =
        updateUniform((float)swapChain.getExtent().width,
                      (float)swapChain.getExtent().height);
    memcpy(currentFrame.uniformBuffers.mapped, &ubo, sizeof(ubo));

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {currentFrame.imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentFrame.commandBuffer;

    VkSemaphore signalSemaphores[] = {currentFrame.renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    result = vkQueueSubmit(
        device.getGraphicsQueue(), 1, &submitInfo, currentFrame.inFlightFence);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to submit draw commant buffer: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        const_cast<VulkanSwapChain &>(swapChain).recreate();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrameIndex = (currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanPipeline::createDescriptor()
{
    createDescriptorPool();
    createDescriptorSetLayout();
}

void VulkanPipeline::createDescriptorPool()
{
    descriptor.setsCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = descriptor.setsCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = descriptor.setsCount;

    if (vkCreateDescriptorPool(
            context->getDevice(), &poolInfo, nullptr, &descriptor.pool) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void VulkanPipeline::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;
    if (vkCreateDescriptorSetLayout(context->getDevice(),
                                    &layoutInfo,
                                    nullptr,
                                    &descriptor.setLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void VulkanPipeline::createDescriptorSets(VkDescriptorSet *descriptorSets)
{
    std::vector<VkDescriptorSetLayout> layouts(descriptor.setsCount,
                                               descriptor.setLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptor.pool;
    allocInfo.descriptorSetCount = descriptor.setsCount;
    allocInfo.pSetLayouts = layouts.data();

    if (vkAllocateDescriptorSets(
            context->getDevice(), &allocInfo, descriptorSets) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

void VulkanPipeline::updateDescriptorSet(VkDescriptorSet descriptorSet,
                                         VkBuffer uniformBuffer)
{
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;       // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional

    vkUpdateDescriptorSets(
        context->getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void VulkanPipeline::createFramesInFlight()
{
    std::vector<VkCommandBuffer> commandBuffers(MAX_FRAMES_IN_FLIGHT);
    context->getBufferCreator().createCommandBuffers(commandBuffers.data(),
                                                     MAX_FRAMES_IN_FLIGHT);

    std::vector<VkDescriptorSet> descriptorSets(MAX_FRAMES_IN_FLIGHT);
    createDescriptorSets(descriptorSets.data());

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        framesInFlight[i].commandBuffer = commandBuffers[i];

        createSyncObjects(framesInFlight[i].imageAvailableSemaphore,
                          framesInFlight[i].renderFinishedSemaphore,
                          framesInFlight[i].inFlightFence);

        createUniformBuffers(framesInFlight[i].uniformBuffers.buffer,
                             framesInFlight[i].uniformBuffers.memory,
                             &framesInFlight[i].uniformBuffers.mapped);

        framesInFlight[i].uniformBuffers.descriptorSet = descriptorSets[i];
        updateDescriptorSet(framesInFlight[i].uniformBuffers.descriptorSet,
                            framesInFlight[i].uniformBuffers.buffer);
    }
}

void VulkanPipeline::createSyncObjects(VkSemaphore &imageAvailableSemaphore,
                                       VkSemaphore &renderFinishedSemaphore,
                                       VkFence &inFlightFence)
{
    VkDevice device = context->getDevice();

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(
            device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) !=
            VK_SUCCESS ||
        vkCreateSemaphore(
            device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) !=
            VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) !=
            VK_SUCCESS)
    {
        throw std::runtime_error("failed to create semaphores!");
    }
}

void VulkanPipeline::createUniformBuffers(VkBuffer &uniformBuffers,
                                          VkDeviceMemory &uniformBuffersMemory,
                                          void **uniformBuffersMapped)
{

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    context->getBufferCreator().createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        uniformBuffers,
        uniformBuffersMemory);

    vkMapMemory(context->getDevice(),
                uniformBuffersMemory,
                0,
                bufferSize,
                0,
                uniformBuffersMapped);
}

void VulkanPipeline::createPipeline()
{
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaders();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.stageCount = 2;

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo =
        createVertexInputInfo(bindingDescription, attributeDescriptions);
    pipelineInfo.pVertexInputState = &vertexInputInfo;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly =
        createInputAssembly();
    pipelineInfo.pInputAssemblyState = &inputAssembly;

    VkPipelineViewportStateCreateInfo viewportState = createViewportState();
    pipelineInfo.pViewportState = &viewportState;

    VkPipelineRasterizationStateCreateInfo rasterizer = createRasterizer();
    pipelineInfo.pRasterizationState = &rasterizer;

    VkPipelineMultisampleStateCreateInfo multisampling = createMultisampling();
    pipelineInfo.pMultisampleState = &multisampling;

    VkPipelineColorBlendAttachmentState colorBlendAttachment =
        createColorBlendAttachment();
    VkPipelineColorBlendStateCreateInfo colorBlending =
        createColorBlending(&colorBlendAttachment);
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;

    VkPipelineDynamicStateCreateInfo dynamicState = createDynamicState();
    pipelineInfo.pDynamicState = &dynamicState;

    createPipelineLayout();
    pipelineInfo.layout = pipelineLayout;

    pipelineInfo.renderPass = context->getRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    VkResult result = vkCreateGraphicsPipelines(context->getDevice(),
                                                VK_NULL_HANDLE,
                                                1,
                                                &pipelineInfo,
                                                nullptr,
                                                &graphicsPipeline);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to create graphicsPipeline: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }
}

void VulkanPipeline::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptor.setLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    VkResult result = vkCreatePipelineLayout(
        context->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to create pipeline layout: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }
}

std::vector<VkPipelineShaderStageCreateInfo> VulkanPipeline::createShaders()
{
    auto vertShaderCode = readFile(SHADERS_DIR "/vert.spv");
    vertShaderModule = createShaderModule(vertShaderCode);
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    auto fragShaderCode = readFile(SHADERS_DIR "/frag.spv");
    fragShaderModule = createShaderModule(fragShaderCode);
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    return std::vector<VkPipelineShaderStageCreateInfo>{vertShaderStageInfo,
                                                        fragShaderStageInfo};
}

VkShaderModule VulkanPipeline::createShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(
        context->getDevice(), &createInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to create shader module: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }

    return shaderModule;
}

VkPipelineVertexInputStateCreateInfo VulkanPipeline::createVertexInputInfo(
    const VkVertexInputBindingDescription &bindingDescription,
    const std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

    vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo VulkanPipeline::createInputAssembly()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    return inputAssembly;
}

VkPipelineViewportStateCreateInfo VulkanPipeline::createViewportState()
{
    const VkExtent2D &swapChainExtent = context->getSwapChain().getExtent();
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    return viewportState;
}

VkPipelineRasterizationStateCreateInfo VulkanPipeline::createRasterizer()
{
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f;          // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional
    return rasterizer;
}

VkPipelineMultisampleStateCreateInfo VulkanPipeline::createMultisampling()
{
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;          // Optional
    multisampling.pSampleMask = nullptr;            // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;      // Optional
    return multisampling;
}

VkPipelineColorBlendAttachmentState VulkanPipeline::createColorBlendAttachment()
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo VulkanPipeline::createColorBlending(
    VkPipelineColorBlendAttachmentState *colorBlendAttachment)
{
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional
    return colorBlending;
}

VkPipelineDynamicStateCreateInfo VulkanPipeline::createDynamicState()
{
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount =
        static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    return dynamicState;
}
