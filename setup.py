from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup
import os
from pathlib import Path

t_dir = Path(os.environ["HOME"]) / "Devel" / "github" / "hz-b" / "timepix-sort-cpp"
t_dir = Path(os.environ["HOME"]) / ".local"
include_dirs=[t_dir / "include" ]
library_dirs=[ str(t_dir / "lib") ]

ext_modules = [
    Pybind11Extension(
        "_timepix_sort",
        [
            "src/c++/chunks.cpp",
            "src/c++/io.cpp",
            "src/c++/events.cpp",
            "src/c++/module.cpp",
            "src/c++/volume.cpp",
        ],
        include_dirs=include_dirs + ["/usr/local/include"],
        libraries=["timepix-sort-c++",],
        library_dirs=library_dirs + ["/usr/local/lib"],
    )
]
setup(cmdclass={"build_ext": build_ext}, ext_package="timepix_sort", ext_modules=ext_modules)
