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
    void clear();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats) const;
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes) const;
    VkExtent2D chooseSwapExtent(
        const VkSurfaceCapabilitiesKHR &capabilities) const;

  public:
    VkFormat getImageFormat() const { return imageFormat; }
    VkExtent2D getExtent() const { return extent; }
    const std::vector<VkFramebuffer> &getFrameBuffers() const
    {
        return frameBuffers;
    }
    operator VkSwapchainKHR() const { return swapChain; }

  private:
    VulkanContext *context;

    VkSwapchainKHR swapChain;

    std::vector<VkImageView> imageViews;
    std::vector<VkImage> images;
    std::vector<VkFramebuffer> frameBuffers;
    VkFormat imageFormat;
    VkExtent2D extent;
};
#endif // VULKAN_SWAP_CHAIN_H
