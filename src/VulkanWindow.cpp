#include <GLFW/glfw3.h>

#include "VulkanContext.h"

#include "VulkanWindow.h"

static const uint32_t WINDOW_WIDTH = 800;
static const uint32_t WINDOW_HEIGHT = 600;
static const char *WINDOW_NAME = "Vulkan";

VulkanWindow::VulkanWindow(VulkanContext *context) : context(context){};

VulkanWindow::~VulkanWindow()
{
    glfwDestroyWindow(window);
}

static void framebufferSizeCallback(GLFWwindow *window, int, int)
{
    VulkanContext *context =
        reinterpret_cast<VulkanContext *>(glfwGetWindowUserPointer(window));

    const_cast<VulkanSwapChain &>(context->getSwapChain()).recreate();
}

void VulkanWindow::init()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);

    glfwSetWindowUserPointer(window, (void *)context);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}
