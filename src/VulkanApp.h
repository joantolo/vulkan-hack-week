#ifndef VULKAN_APP_H
#define VULKAN_APP_H

#include "Triangle.h"
#include "VulkanContext.h"

class VulkanApp
{
  public:
    VulkanApp();
    ~VulkanApp();
    void run();

  private:
    void init();
    void mainLoop();

  private:
    VulkanContext context;
    Triangle triangle;
};
#endif // VULKAN_APP_H
