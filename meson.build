project('vulkan-hack-week', 'cpp',
        version: '0.1.0',
        meson_version: '>= 0.59.0',
        default_options: ['warning_level=3',
                          'buildtype=debugoptimized',
                          'cpp_std=c++17'])

cpp = meson.get_compiler('cpp')
vulkan_dep = dependency('vulkan')
vk_validation_layers_dep = cpp.find_library('VkLayer_khronos_validation', required: true)
glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')

shaders_dir = join_paths(meson.current_source_dir(), 'src/shaders')

conf_data = configuration_data()
conf_data.set_quoted('SHADERS_DIR', shaders_dir)

configure_file(input: 'config.h.meson',
               output: 'config.h',
               configuration: conf_data)

configinc = include_directories('.')

subdir('src')
