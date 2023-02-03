#include <GLFW/glfw3.h>

#include "GlfwWindow.h"

void GlfwWindow::init(size_t width, size_t heigth, const char* name)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    this->window = glfwCreateWindow(width, heigth, name, nullptr, nullptr);
}

void GlfwWindow::clear()
{
    glfwDestroyWindow(this->window);
}
