#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

#include "VulkanContext.h"

#include "VulkanSurface.h"

VulkanSurface::VulkanSurface(VulkanContext *context) : context(context) {}

VulkanSurface::~VulkanSurface()
{
    vkDestroySurfaceKHR(context->getInstance(), surface, nullptr);
}

void VulkanSurface::init()
{
    VkResult result = glfwCreateWindowSurface(
        context->getInstance(), context->getWindow(), nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        std::string errorMsg("Failed to create window surface: ");
        errorMsg.append(string_VkResult(result));
        throw std::runtime_error(errorMsg);
    }
}
