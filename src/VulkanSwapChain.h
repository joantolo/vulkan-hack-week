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
    std::vector<VkImageView> getImageViews() const { return swapChainImageViews; }
    operator VkSwapchainKHR() const { return swapChain; }

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

    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
};
#endif // VULKAN_SWAP_CHAIN_H
