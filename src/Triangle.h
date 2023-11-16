#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <array>
#include <vector>

#include "VulkanTypes.h"

class Triangle
{
  public:
    void init(VulkanDevice *device, VulkanBufferCreator *bufferCreator);
    void clear();

  private:
    void createVertexBuffer();

  public:
    VkBuffer getVertexBuffer() const { return vertexBuffer; }
    std::vector<Vertex> getVertices() const { return vertices; }

  private:
    const std::vector<Vertex> vertices = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                          {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VulkanDevice *device;
    VulkanBufferCreator *bufferCreator;
};

#endif // TRIANGLE_H
