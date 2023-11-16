#include "VulkanApp.h"

VulkanApp::VulkanApp() : triangle(&context) {}

VulkanApp::~VulkanApp() {}

void VulkanApp::run()
{
    init();
    mainLoop();
}

void VulkanApp::init()
{
    context.init();
    triangle.init();
}

void VulkanApp::mainLoop()
{
    while (!glfwWindowShouldClose(context.getWindow()))
    {
        glfwPollEvents();
        context.getPipeline().drawFrame(triangle);
    }

    vkDeviceWaitIdle(context.getDevice());
}
