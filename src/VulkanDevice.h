#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include "VulkanTypes.h"

class VulkanDevice
{
  public:
    void init(VkInstance *instance, VulkanSurface *surface);
    void clear();

  private:
    void pickPhysicalDevice();
    void createLogicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  public:
    QueueFamilyIndices getQueueFamiyIndices() const
    {
        return queueFamilyIndices;
    }
    SwapChainSupportDetails getSwapChainSupport() { return swapChainSupport; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    operator VkDevice() const { return device; }

  private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;

    VkInstance *instance;
    VulkanSurface *surface;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    QueueFamilyIndices queueFamilyIndices;
    SwapChainSupportDetails swapChainSupport;
};
#endif // VULKAN_DEVICE_H
