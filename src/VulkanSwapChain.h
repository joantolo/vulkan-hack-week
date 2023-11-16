#ifndef VULKAN_SWAP_CHAIN_H
#define VULKAN_SWAP_CHAIN_H

#include <vector>

#include "VulkanTypes.h"

class VulkanSwapChain
{
  public:
    VulkanSwapChain(VulkanContext *context);
    ~VulkanSwapChain();
    void init();
    void recreate();
    void createFrameBuffers();

  private:
    void createSwapChain();
    void createImageViews();
    void clearSwapChain();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats) const;
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes) const;
    VkExtent2D chooseSwapExtent(
        const VkSurfaceCapabilitiesKHR &capabilities) const;

  public:
    VkFormat getImageFormat() const { return swapChainImageFormat; }
    VkExtent2D getExtent() const { return swapChainExtent; }
    const std::vector<VkFramebuffer> &getFrameBuffers() const
    {
        return swapChainFramebuffers;
    }
    operator VkSwapchainKHR() const { return swapChain; }

  private:
    VulkanContext *context;

    VkSwapchainKHR swapChain;

    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
};
#endif // VULKAN_SWAP_CHAIN_H
