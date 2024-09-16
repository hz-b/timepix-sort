project(
	'purelib-and-platlib',
	'cpp',
	default_options: [
	'cpp_std=c++23',
	 ],
)

# timepix_sort_cpp_dep = dependency('timepix_sort_cpp', method : 'cmake', required: false)
incdir = include_directories(
    '/usr/local/include/',
    )

py = import('python').find_installation(pure: false)
pybind11_dep = dependency('pybind11')

# compile_args('-g', language : 'cpp')
# add_project_compile_arguments('-g', language : 'cpp')
# add_project_compile_arguments('-ggdb', language : 'cpp')

add_project_link_arguments('-L/usr/local/lib/', language : 'cpp')
add_project_link_arguments('-ltimepix-sort-c++', language : 'cpp')
py.extension_module(
    '_timepix_sort',
    'src/c++/chunks.cpp',
    'src/c++/io.cpp',
    'src/c++/events.cpp',
    'src/c++/module.cpp',
    'src/c++/volume.cpp',
    dependencies : [pybind11_dep],
    install: true,
    include_directories : incdir,
    subdir: 'timepix_sort'
)
subdir('src')
