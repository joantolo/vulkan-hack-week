#ifndef VULKAN_SWAP_CHAIN_H
#define VULKAN_SWAP_CHAIN_H

#include <vulkan/vulkan.h>

#include <vector>

#include "VulkanTypes.h"

class VulkanSwapChain
{
public:
    void init(VkInstance* instance, VulkanDevice* device, VulkanSurface* surface, GlfwWindow* window);
    void clear();

    VkFormat getImageFormat() const { return swapChainImageFormat; }
    VkExtent2D getExtent() const { return swapChainExtent; }
    std::vector<VkFramebuffer> getFrameBuffers() const { return swapChainFramebuffers; }
    operator VkSwapchainKHR() const { return swapChain; }

    void setRenderPass (VulkanRenderPass* renderPass) { this->renderPass = renderPass; }

    void createFrameBuffers();

private:
    void createSwapChain();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void createImageViews();

private:
    VkSwapchainKHR swapChain;

    VkInstance* instance;
    VulkanDevice* device;
    GlfwWindow* window;
    VulkanSurface* surface;
    VulkanRenderPass* renderPass;

    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
};
#endif // VULKAN_SWAP_CHAIN_H
