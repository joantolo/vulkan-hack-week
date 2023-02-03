deps = [
  vulkan_dep,
  glfw_dep,
  glm_dep,
]

sources = files([
  'main.cpp',
  'GlfwWindow.cpp',
  'Triangle.cpp',
  'utils.cpp',
  'VulkanApp.cpp',
  'VulkanDebugger.cpp',
  'VulkanDevice.cpp',
  'VulkanBuffer.cpp',
  'VulkanPipeline.cpp',
  'VulkanRenderPass.cpp',
  'VulkanSwapChain.cpp',
  'VulkanSurface.cpp',
])

vulkan_hack_week = executable('vulkan-hack-week',
                              sources,
                              dependencies: deps,
                              include_directories: [configinc],
                              install: false)

subdir('shaders')
