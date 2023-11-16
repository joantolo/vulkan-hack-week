#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H

#include "VulkanTypes.h"

class VulkanWindow
{
  public:
    VulkanWindow(VulkanContext *context);
    ~VulkanWindow();
    void init();

  public:
    operator GlfwWindow() const { return window; }

  private:
    VulkanContext *context;

    GlfwWindow window;
};
#endif // VULKAN_SURFACE_H
