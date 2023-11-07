#ifndef VULKAN_SWAP_CHAIN_H
#define VULKAN_SWAP_CHAIN_H

#include <vulkan/vulkan.h>

#include <vector>

#include "VulkanTypes.h"

class VulkanSwapChain
{
  public:
    void init(VkInstance *instance,
              VulkanDevice *device,
              VulkanSurface *surface,
              VulkanWindow *window);
    void clear();
    void createSwapChain();
    void createImageViews();
    void createFrameBuffers();

  private:
    void registerResizeCallback();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  public:
    VkFormat getImageFormat() const { return swapChainImageFormat; }
    VkExtent2D getExtent() const { return swapChainExtent; }
    std::vector<VkFramebuffer> getFrameBuffers() const
    {
        return swapChainFramebuffers;
    }
    void setRenderPass(VulkanRenderPass *renderPass)
    {
        this->renderPass = renderPass;
    }
    operator VkSwapchainKHR() const { return swapChain; }

  private:
    VkSwapchainKHR swapChain;

    VkInstance *instance;
    VulkanDevice *device;
    VulkanWindow *window;
    VulkanSurface *surface;
    VulkanRenderPass *renderPass;

    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
};
#endif // VULKAN_SWAP_CHAIN_H
