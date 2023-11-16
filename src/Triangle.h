#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>

#include "VulkanTypes.h"

class Triangle
{
  public:
    Triangle(VulkanContext *context);
    ~Triangle();
    void init();

  private:
    void createVertexBuffer();

  public:
    VkBuffer getVertexBuffer() const { return vertexBuffer; }
    std::vector<Vertex> getVertices() const { return vertices; }

  private:
    VulkanContext *context;

    const std::vector<Vertex> vertices = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                          {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
};

#endif // TRIANGLE_H
