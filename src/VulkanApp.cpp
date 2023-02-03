#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "VulkanDebugger.h"

#include "VulkanApp.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

void VulkanApp::run()
{
    init();
    mainLoop();
    clear();
}

void VulkanApp::init()
{
    glfwInit();

    if (useDebugger)
        VulkanDebugger::init(&this->instance);


    createInstance();
    this->window.init(WIDTH, HEIGHT, "Vulkan");
    this->surface.init(&this->instance, &this->window);
    this->device.init(&this->instance, &this->surface);
    this->swapChain.init(&this->instance, &this->device, &this->surface, &this->window);

    this->pipeline.init(&this->device, &this->swapChain, &this->triangle);

    this->triangle.init(&this->device, &pipeline.getBufferCreator());
}

void VulkanApp::mainLoop()
{
    while (!glfwWindowShouldClose(this->window)) {
        glfwPollEvents();
        this->pipeline.drawFrame();
    }

    vkDeviceWaitIdle(this->device);
}

void VulkanApp::clear()
{
    this->pipeline.clear();

    this->triangle.clear();

    this->swapChain.clear();
    this->device.clear();
    this->surface.clear();

    if (useDebugger)
        VulkanDebugger::clear();

    vkDestroyInstance(this->instance, nullptr);
    this->window.clear();

    glfwTerminate();
}

void VulkanApp::createInstance()
{
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

    createInfo.enabledLayerCount = 0;
    if (useDebugger)
        VulkanDebugger::addValidationLayers(createInfo);

    VkResult result = vkCreateInstance(&createInfo, nullptr, &this->instance);
    if (result != VK_SUCCESS) {
        std::string errorMsg("Failed to create instance: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }

    if (useDebugger)
        VulkanDebugger::setupDebugMessenger();
}

std::vector<const char*> VulkanApp::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (useDebugger)
        VulkanDebugger::addRequiredExtensions(extensions);

    return extensions;
}
