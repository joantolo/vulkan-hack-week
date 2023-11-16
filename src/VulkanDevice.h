#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include "VulkanTypes.h"

class VulkanDevice
{
  public:
    VulkanDevice(VulkanContext *context);
    ~VulkanDevice();
    void init();

  private:
    void pickPhysicalDevice();
    void createLogicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device) const;
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
    SwapChainSupportDetails querySwapChainSupport(
        VkPhysicalDevice device) const;
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

  public:
    const QueueFamilyIndices &getQueueFamiyIndices() const
    {
        return queueFamilyIndices;
    }
    const SwapChainSupportDetails &getSwapChainSupport() const
    {
        return swapChainSupport;
    }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    operator VkDevice() const { return device; }

  private:
    VulkanContext *context;

    VkDevice device;
    VkPhysicalDevice physicalDevice;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    QueueFamilyIndices queueFamilyIndices;
    SwapChainSupportDetails swapChainSupport;
};
#endif // VULKAN_DEVICE_H
