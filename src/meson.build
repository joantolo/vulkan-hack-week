deps = [
  vulkan_dep,
  vk_validation_layers_dep,
  glfw_dep,
  glm_dep,
]

sources = files([
  'main.cpp',
  'Triangle.cpp',
  'utils.cpp',
  'VulkanApp.cpp',
  'VulkanBufferCreator.cpp',
  'VulkanContext.cpp',
  'VulkanDebugger.cpp',
  'VulkanDevice.cpp',
  'VulkanPipeline.cpp',
  'VulkanRenderPass.cpp',
  'VulkanSwapChain.cpp',
  'VulkanSurface.cpp',
  'VulkanWindow.cpp',
])

vulkan_hack_week = executable('vulkan-hack-week',
                              sources,
                              dependencies: deps,
                              include_directories: [configinc],
                              install: false)

subdir('shaders')
