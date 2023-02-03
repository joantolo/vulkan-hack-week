project('vulkan-hack-week', 'cpp',
        version: '0.1.0',
        meson_version: '>= 0.59.0',
        default_options: ['warning_level=3',
                          'buildtype=debugoptimized',
                          'cpp_std=c++17'])

vulkan_dep = dependency('vulkan')
glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')

conf_data = configuration_data()
shaders_dir = join_paths(meson.current_source_dir(), 'src/shaders')
conf_data.set_quoted('SHADERS_DIR', shaders_dir)
configure_file(input: 'config.h.meson',
               output: 'config.h',
               configuration: conf_data)
configinc = include_directories('.')

subdir('src')
