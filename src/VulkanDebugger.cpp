#include <vulkan/vulkan.h>

#include <cstring>
#include <iostream>
#include <vector>

#include "VulkanDebugger.h"

VkInstance *VulkanDebugger::instance;
const std::vector<const char *> VulkanDebugger::validationLayers = {
    "VK_LAYER_KHRONOS_validation"};
VkDebugUtilsMessengerCreateInfoEXT VulkanDebugger::debugCreateInfo;
VkDebugUtilsMessengerEXT VulkanDebugger::debugMessenger;

void VulkanDebugger::init(VkInstance *instance)
{
    if (!checkValidationLayerSupport())
    {
        throw std::runtime_error(
            "Validation layers requested, but not available!");
    }

    VulkanDebugger::instance = instance;
    VulkanDebugger::debugCreateInfo = {};
    VulkanDebugger::debugCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    VulkanDebugger::debugCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    VulkanDebugger::debugCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    VulkanDebugger::debugCreateInfo.pfnUserCallback = debugCallback;
}

void VulkanDebugger::clear()
{
    DestroyDebugUtilsMessengerEXT(
        *VulkanDebugger::instance, VulkanDebugger::debugMessenger, nullptr);
}

void VulkanDebugger::setupDebugMessenger()
{
    if (CreateDebugUtilsMessengerEXT(*VulkanDebugger::instance,
                                     &VulkanDebugger::debugCreateInfo,
                                     nullptr,
                                     &VulkanDebugger::debugMessenger) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to set up debug messenger!");
    }
}

bool VulkanDebugger::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}

void VulkanDebugger::addRequiredExtensions(
    std::vector<const char *> &extensions)
{
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

void VulkanDebugger::addValidationLayers(VkInstanceCreateInfo &createInfo)
{
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(VulkanDebugger::validationLayers.size());
    createInfo.ppEnabledLayerNames = VulkanDebugger::validationLayers.data();

    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
}

void VulkanDebugger::addValidationLayers(VkDeviceCreateInfo &createInfo)
{
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(VulkanDebugger::validationLayers.size());
    createInfo.ppEnabledLayerNames = VulkanDebugger::validationLayers.data();
}

VkBool32 VulkanDebugger::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *)
{
    if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        return VK_FALSE;

    std::cerr << "DEBUG: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult VulkanDebugger::CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanDebugger::DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
        func(instance, debugMessenger, pAllocator);
}
