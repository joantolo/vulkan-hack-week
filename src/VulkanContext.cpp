#include <iostream>
#include <vector>
#include <vulkan/vk_enum_string_helper.h>

#include "VulkanDebugger.h"

#include "VulkanContext.h"

static std::vector<const char *> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

    if (useDebugger)
        VulkanDebugger::addRequiredExtensions(extensions);

    return extensions;
}

static VkInstance createInstance()
{
    glfwInit();

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkInstance instance{};
    createInfo.enabledLayerCount = 0;
    if (useDebugger)
    {
        VulkanDebugger::init(&instance);
        VulkanDebugger::addValidationLayers(createInfo);
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to create instance: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }

    if (useDebugger)
        VulkanDebugger::setupDebugMessenger();

    return instance;
}

VulkanContext::VulkanContext()
    : instance(createInstance()),
      window(VulkanWindow(this)),
      surface(VulkanSurface(this)),
      device(VulkanDevice(this)),
      swapChain(VulkanSwapChain(this)),
      bufferCreator(VulkanBufferCreator(this)),
      renderPass(VulkanRenderPass(this)),
      pipeline(VulkanPipeline(this))
{
}

VulkanContext::~VulkanContext()
{
    if (useDebugger)
        VulkanDebugger::clear();

    glfwTerminate();
}

void VulkanContext::init()
{
    window.init();
    surface.init();
    device.init();
    swapChain.init();
    bufferCreator.init();
    renderPass.init();
    pipeline.init();
}
