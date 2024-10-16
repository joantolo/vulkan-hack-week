#include <GLFW/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <iostream>
#include <limits>

#include "VulkanContext.h"

#include "VulkanSwapChain.h"

VulkanSwapChain::VulkanSwapChain(VulkanContext *context) : context(context) {}

VulkanSwapChain::~VulkanSwapChain()
{
    clear();
}

void VulkanSwapChain::init()
{
    createSwapChain();
    createImageViews();
}

void VulkanSwapChain::recreate()
{
    vkDeviceWaitIdle(context->getDevice());

    clear();
    createSwapChain();
    createImageViews();
    createFrameBuffers();
}

void VulkanSwapChain::createSwapChain()
{
    const VulkanDevice &device = context->getDevice();

    const SwapChainSupportDetails &swapChainSupport =
        device.getSwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = context->getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const QueueFamilyIndices &indices = device.getQueueFamiyIndices();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result =
        vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to create swap chain: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());

    imageFormat = surfaceFormat.format;
    this->extent = extent;
}

void VulkanSwapChain::createImageViews()
{
    imageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        createInfo.image = images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = imageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(
            context->getDevice(), &createInfo, nullptr, &imageViews[i]);
        if (result != VK_SUCCESS)
        {
            std::string errorMsg("Failed to create Image View: ");
            errorMsg.append(string_VkResult(result));
            throw std::runtime_error(errorMsg);
        }
    }
}

void VulkanSwapChain::createFrameBuffers()
{
    frameBuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++)
    {
        VkImageView attachments[] = {
            imageViews[i],
        };

        VkFramebufferCreateInfo frameBufferInfo{};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = context->getRenderPass();
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = extent.width;
        frameBufferInfo.height = extent.height;
        frameBufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(
            context->getDevice(), &frameBufferInfo, nullptr, &frameBuffers[i]);
        if (result != VK_SUCCESS)
        {
            std::string errorMsg("Failed to create FrameBuffer: ");
            errorMsg.append(string_VkResult(result));
            throw std::runtime_error(errorMsg);
        }
    }
}

void VulkanSwapChain::clear()
{
    VkDevice device = context->getDevice();

    for (auto imageView : imageViews)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }

    for (auto framebuffer : frameBuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) const
{
    auto it = std::find_if(availableFormats.begin(),
                           availableFormats.end(),
                           [](const VkSurfaceFormatKHR &val) {
                               return val.format == VK_FORMAT_B8G8R8A8_SRGB &&
                                      val.colorSpace ==
                                          VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
                           });

    if (it != availableFormats.end())
        return *it;

    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) const
{
    auto it = std::find(availablePresentModes.begin(),
                        availablePresentModes.end(),
                        VK_PRESENT_MODE_MAILBOX_KHR);

    if (it != availablePresentModes.end())
        return *it;

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR &capabilities) const
{
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        const VulkanWindow &window = context->getWindow();
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                                   static_cast<uint32_t>(height)};

        actualExtent.width = std::clamp(actualExtent.width,
                                        capabilities.minImageExtent.width,
                                        capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                         capabilities.minImageExtent.height,
                                         capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
