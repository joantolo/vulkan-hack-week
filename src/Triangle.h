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
    void createIndexBuffer();

  public:
    VkBuffer getVertexBuffer() const { return vertexBuffer; }
    uint32_t getVertexCount() const
    {
        return static_cast<uint32_t>(vertices.size());
    }
    VkBuffer getIndexBuffer() const { return indexBuffer; }
    uint32_t getIndexCount() const
    {
        return static_cast<uint32_t>(indices.size());
    }

  private:
    VulkanContext *context;

    const std::vector<Vertex> vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                          {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                          {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}};

    const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

#endif // TRIANGLE_H
