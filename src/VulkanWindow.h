#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H

#include <GLFW/glfw3.h>

class VulkanWindow
{
  public:
    void init(size_t width, size_t heigth, const char *name);
    void clear();

  public:
    operator GLFWwindow *() const { return window; }

  private:
    GLFWwindow *window;
};
#endif // VULKAN_SURFACE_H
