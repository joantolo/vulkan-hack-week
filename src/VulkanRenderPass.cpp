#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"

#include "VulkanRenderPass.h"

void VulkanRenderPass::init(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanPipeline* pipeline)
{
    this->device = device;
    this->swapChain = swapChain;
    this->pipeline = pipeline;

    createRenderPass();
    createCommandPool();
    createCommandBuffer();
    createFrameBuffers();
}

void VulkanRenderPass::clear()
{
    vkDestroyCommandPool(*this->device, this->commandPool, nullptr);

    for (auto framebuffer : this->swapChainFramebuffers) {
        vkDestroyFramebuffer(*this->device, framebuffer, nullptr);
    }

    vkDestroyRenderPass(*this->device, this->renderPass, nullptr);
}

void VulkanRenderPass::createRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = this->swapChain->getImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult result = vkCreateRenderPass(*this->device, &renderPassInfo, nullptr, &this->renderPass);
    if (result != VK_SUCCESS) {
        std::string errorMsg("Failed to create render pass: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }
}

void VulkanRenderPass::createFrameBuffers()
{
    std::vector<VkImageView> swapChainImageViews = this->swapChain->getImageViews();
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            swapChainImageViews[i],
        };

        VkFramebufferCreateInfo frameBufferInfo{};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = *this;
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = this->swapChain->getExtent().width;
        frameBufferInfo.height = this->swapChain->getExtent().height;
        frameBufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(*this->device, &frameBufferInfo, nullptr, &this->swapChainFramebuffers[i]);
        if (result != VK_SUCCESS) {
            std::string errorMsg("Failed to create FrameBuffer: ");
            errorMsg.append(string_VkResult(result));
            throw std::runtime_error(errorMsg);
        }
    }
}

void VulkanRenderPass::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = this->device->getQueueFamiyIndices();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VkResult result = vkCreateCommandPool(*this->device, &poolInfo, nullptr, &this->commandPool);
    if (result != VK_SUCCESS) {
        std::string errorMsg("Failed to create command pool: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }
}

void VulkanRenderPass::createCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = this->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(*this->device, &allocInfo, &this->commandBuffer);
    if (result != VK_SUCCESS) {
        std::string errorMsg("Failed to allocate command buffer: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }
}

void VulkanRenderPass::recordCommandBuffer(VkCommandBuffer commandBuffer, Triangle triangle, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS) {
        std::string errorMsg("Failed to begin recording a command buffer: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = this->renderPass;
    renderPassInfo.framebuffer = this->swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = this->swapChain->getExtent();
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *this->pipeline);

    VkExtent2D swapChainExtent = this->swapChain->getExtent();
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {triangle.getVertexBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdDraw(commandBuffer, static_cast<uint32_t>(triangle.getVertices().size()), 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    result = vkEndCommandBuffer(commandBuffer);
    if (result != VK_SUCCESS) {
        std::string errorMsg("Failed to record a command buffer: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }
}
