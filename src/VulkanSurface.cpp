#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "GlfwWindow.h"

#include "VulkanSurface.h"

void VulkanSurface::init(VkInstance* instance, GlfwWindow* window)
{
    this->instance = instance;
    this->window = window;

    createSurface();
}

void VulkanSurface::clear()
{
    vkDestroySurfaceKHR(*this->instance, this->surface, nullptr);
}

void VulkanSurface::createSurface()
{
    VkResult result = glfwCreateWindowSurface(*this->instance, *this->window, nullptr, &this->surface);
    if (result != VK_SUCCESS) {
        std::string errorMsg("Failed to create window surface: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }
}
