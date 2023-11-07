#include <GLFW/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <iostream>
#include <limits>

#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanSurface.h"
#include "VulkanWindow.h"

#include "VulkanSwapChain.h"

void VulkanSwapChain::init(VkInstance *instance,
                           VulkanDevice *device,
                           VulkanSurface *surface,
                           VulkanWindow *window)
{
    this->instance = instance;
    this->device = device;
    this->window = window;
    this->surface = surface;
    createSwapChain();
    createImageViews();
    registerResizeCallback();
}

void VulkanSwapChain::clear()
{
    vkDestroySwapchainKHR(*this->device, this->swapChain, nullptr);

    for (auto imageView : this->swapChainImageViews)
    {
        vkDestroyImageView(*this->device, imageView, nullptr);
    }

    for (auto framebuffer : this->swapChainFramebuffers)
    {
        vkDestroyFramebuffer(*this->device, framebuffer, nullptr);
    }
}

static void framebufferSizeCallback(GLFWwindow *window, int, int)
{
    VulkanSwapChain *self =
        reinterpret_cast<VulkanSwapChain *>(glfwGetWindowUserPointer(window));

    self->clear();
    self->createSwapChain();
    self->createImageViews();
    self->createFrameBuffers();
}

void VulkanSwapChain::registerResizeCallback()
{
    glfwSetWindowUserPointer(*this->window, this);
    glfwSetFramebufferSizeCallback(*this->window, framebufferSizeCallback);
}

void VulkanSwapChain::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport =
        this->device->getSwapChainSupport();

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
    createInfo.surface = *this->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = this->device->getQueueFamiyIndices();
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

    if (vkCreateSwapchainKHR(
            *this->device, &createInfo, nullptr, &this->swapChain) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(
        *this->device, this->swapChain, &imageCount, nullptr);
    this->swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(*this->device,
                            this->swapChain,
                            &imageCount,
                            this->swapChainImages.data());

    this->swapChainImageFormat = surfaceFormat.format;
    this->swapChainExtent = extent;
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(*this->window, &width, &height);

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

void VulkanSwapChain::createImageViews()
{
    this->swapChainImageViews.resize(this->swapChainImages.size());
    for (size_t i = 0; i < this->swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        createInfo.image = this->swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = this->swapChainImageFormat;

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
            *this->device, &createInfo, nullptr, &this->swapChainImageViews[i]);
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
    this->swapChainFramebuffers.resize(this->swapChainImageViews.size());

    for (size_t i = 0; i < this->swapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = {
            this->swapChainImageViews[i],
        };

        VkFramebufferCreateInfo frameBufferInfo{};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = *this->renderPass;
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = this->swapChainExtent.width;
        frameBufferInfo.height = this->swapChainExtent.height;
        frameBufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(*this->device,
                                              &frameBufferInfo,
                                              nullptr,
                                              &this->swapChainFramebuffers[i]);
        if (result != VK_SUCCESS)
        {
            std::string errorMsg("Failed to create FrameBuffer: ");
            errorMsg.append(string_VkResult(result));
            throw std::runtime_error(errorMsg);
        }
    }
}
