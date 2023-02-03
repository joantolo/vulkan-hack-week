#ifndef VULKAN_DEBUGGER_H
#define VULKAN_DEBUGGER_H

#include <vulkan/vulkan.h>

#include <vector>


#ifdef NDEBUG
    const bool useDebugger = false;
#else
    const bool useDebugger = true;
#endif

class VulkanDebugger
{
public:
    static void init(VkInstance* instance);
    static void clear();
    static void setupDebugMessenger();
    static bool checkValidationLayerSupport();
    static void addRequiredExtensions(std::vector<const char*>& extensions);
    static void addValidationLayers(VkInstanceCreateInfo& createInfo);
    static void addValidationLayers(VkDeviceCreateInfo& createInfo);

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData);
    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                 const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                 const VkAllocationCallbacks* pAllocator,
                                                 VkDebugUtilsMessengerEXT* pDebugMessenger);
    static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                              VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks* pAllocator);
private:
    static VkInstance* instance;

    static const std::vector<const char*> validationLayers;
    static VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    static VkDebugUtilsMessengerEXT debugMessenger;
};
#endif // VULKAN_DEBUGGER_H
