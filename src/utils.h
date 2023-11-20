#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

#include "VulkanTypes.h"

std::vector<char> readFile(const std::string &filename);

UniformBufferObject updateUniform(float width, float height);

#endif // UTILS_H
