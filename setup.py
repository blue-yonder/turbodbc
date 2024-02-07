import distutils.ccompiler
import distutils.sysconfig
import itertools
import os
import os.path
import sys
from glob import iglob

import setuptools.command.build_ext
from setuptools import Distribution, Extension, setup
from setuptools.command.build_ext import build_ext


class TurbodbcExtensionBuilder(build_ext):
    def run(self) -> None:
        super().run()

    def build_extension(self, extension: setuptools.extension.Extension) -> None:
        extension.library_dirs.append(self.build_lib)  # type: ignore
        super().build_extension(extension)


def _get_turbodbc_libname():
    builder = setuptools.command.build_ext.build_ext(Distribution())
    full_name = builder.get_ext_filename("libturbodbc")
    without_lib = full_name.split("lib", 1)[-1]
    without_so = without_lib.rsplit(".so", 1)[0]
    return without_so


def _get_source_files(directory):
    path = os.path.join("src", directory)
    iterable_sources = (
        iglob(os.path.join(root, "*.cpp")) for root, dirs, files in os.walk(path)
    )
    source_files = itertools.chain.from_iterable(iterable_sources)
    return list(source_files)


def _remove_strict_prototype_option_from_distutils_config():
    strict_prototypes = "-Wstrict-prototypes"
    config = distutils.sysconfig.get_config_vars()
    for key, value in config.items():
        if strict_prototypes in str(value):
            config[key] = config[key].replace(strict_prototypes, "")  # type: ignore


_remove_strict_prototype_option_from_distutils_config()


def _has_arrow_headers():
    try:
        import pyarrow  # noqa: F401

        return True
    except ImportError:
        return False


def _has_numpy_headers():
    try:
        import numpy  # noqa: F401

        return True
    except ImportError:
        return False


class _deferred_pybind11_include:
    def __str__(self):
        import pybind11

        return pybind11.get_include()


extra_compile_args = []
hidden_visibility_args: list[str] = []
include_dirs: list[str] = ["include/", str(_deferred_pybind11_include())]

library_dirs: list[str] = []
python_module_link_args: list[str] = []
base_library_link_args: list[str] = []

if sys.platform == "darwin":
    extra_compile_args.append("--std=c++17")
    extra_compile_args.append("--stdlib=libc++")
    extra_compile_args.append("-mmacosx-version-min=10.9")
    # See https://conda-forge.org/docs/maintainer/knowledge_base.html#newer-c-features-with-old-sdk
    extra_compile_args.append("-D_LIBCPP_DISABLE_AVAILABILITY")
    hidden_visibility_args.append("-fvisibility=hidden")
    include_dirs.append(os.getenv("UNIXODBC_INCLUDE_DIR", "/usr/local/include/"))
    library_dirs.append(os.getenv("UNIXODBC_LIBRARY_DIR", "/usr/local/lib/"))

    config_vars = distutils.sysconfig.get_config_vars()
    config_vars["LDSHARED"] = config_vars["LDSHARED"].replace("-bundle", "")  # type: ignore
    python_module_link_args.append("-bundle")
    builder = setuptools.command.build_ext.build_ext(Distribution())
    full_name = builder.get_ext_filename("libturbodbc")
    base_library_link_args.append(
        f"-Wl,-dylib_install_name,@loader_path/{full_name}"  # noqa
    )
    base_library_link_args.append("-dynamiclib")
    odbclib = "odbc"
elif sys.platform == "win32":
    extra_compile_args.append("-DNOMINMAX")
    extra_compile_args.append("/std:c++17")
    boost_root = os.getenv("BOOST_ROOT")
    if boost_root:
        include_dirs.append(boost_root)
        library_dirs.append(os.path.join(boost_root, "stage", "lib"))
        library_dirs.append(os.path.join(boost_root, "lib64-msvc-14.0"))
    else:
        print("warning: BOOST_ROOT enviroment variable not set")
    odbclib = "odbc32"
    conda_prefix = os.getenv("CONDA_PREFIX")
    if conda_prefix:
        include_dirs.append(os.path.join(conda_prefix, "Library", "include"))
        library_dirs.append(os.path.join(conda_prefix, "Library", "lib"))
else:
    extra_compile_args.append("--std=c++17")
    hidden_visibility_args.append("-fvisibility=hidden")
    python_module_link_args.append("-Wl,-rpath,$ORIGIN")
    unixodbc_include_dir = os.getenv("UNIXODBC_INCLUDE_DIR")
    if unixodbc_include_dir:
        include_dirs.append(unixodbc_include_dir)
    unixodbc_library_dir = os.getenv("UNIXODBC_LIBRARY_DIR")
    if unixodbc_library_dir:
        library_dirs.append(unixodbc_library_dir)
    odbclib = "odbc"


def _get_cxx_compiler():
    cc = distutils.ccompiler.new_compiler()
    distutils.sysconfig.customize_compiler(cc)
    return cc.compiler_cxx[0]  # type: ignore


def is_cxx11_abi():
    import pathlib

    import pyarrow.lib

    binary_so = pathlib.Path(pyarrow.lib.__file__).read_bytes()

    # Check for an old CXXABI symbol in the main library. This one is quite stable across all Arrow releases.
    # arrow::Status::ToString() -> std::string
    if b"_ZNK5arrow6Status8ToStringEv" in binary_so:
        return False
    # Here we can add other symbols to check for if future releases would come with a different API.
    return True


def get_extension_modules():
    extension_modules = []

    """
    Extension module which is actually a plain C++ library without Python bindings
    """
    turbodbc_sources = _get_source_files("cpp_odbc") + _get_source_files("turbodbc")
    turbodbc_library = Extension(
        "libturbodbc",
        sources=turbodbc_sources,
        include_dirs=include_dirs,
        extra_compile_args=extra_compile_args,
        extra_link_args=base_library_link_args,
        libraries=[odbclib, "simdutf"],
        library_dirs=library_dirs,
    )
    turbodbc_libs: list[str]
    if sys.platform == "win32":
        turbodbc_libs = []
    else:
        turbodbc_libs = [_get_turbodbc_libname()]
        extension_modules.append(turbodbc_library)

    """
    An extension module which contains the main Python bindings for turbodbc
    """
    turbodbc_python_sources = _get_source_files("turbodbc_python")
    if sys.platform == "win32":
        turbodbc_python_sources = turbodbc_sources + turbodbc_python_sources
    turbodbc_python = Extension(
        "turbodbc_intern",
        sources=turbodbc_python_sources,
        include_dirs=include_dirs,
        extra_compile_args=extra_compile_args + hidden_visibility_args,
        libraries=[odbclib, "simdutf"] + turbodbc_libs,
        extra_link_args=python_module_link_args,
        library_dirs=library_dirs,
    )
    extension_modules.append(turbodbc_python)

    """
    An extension module which contains Python bindings which require numpy support
    to work. Not included in the standard Python bindings so this can stay optional.
    """
    if _has_numpy_headers():
        import numpy

        turbodbc_numpy_sources = _get_source_files("turbodbc_numpy")
        if sys.platform == "win32":
            turbodbc_numpy_sources = turbodbc_sources + turbodbc_numpy_sources
        turbodbc_numpy = Extension(
            "turbodbc_numpy_support",
            sources=turbodbc_numpy_sources,
            include_dirs=include_dirs + [numpy.get_include()],
            extra_compile_args=extra_compile_args + hidden_visibility_args,
            libraries=[odbclib, "simdutf"] + turbodbc_libs,
            extra_link_args=python_module_link_args,
            library_dirs=library_dirs,
        )
        extension_modules.append(turbodbc_numpy)

    """
    An extension module which contains Python bindings which require Apache Arrow
    support to work. Not included in the standard Python bindings so this can
    stay optional.
    """
    if _has_arrow_headers():
        import pyarrow

        # Make default named pyarrow shared libs available.
        pyarrow.create_library_symlinks()

        pyarrow_include_dir = pyarrow.get_include()

        turbodbc_arrow_sources = _get_source_files("turbodbc_arrow")
        pyarrow_module_link_args = list(python_module_link_args)
        if sys.platform == "win32":
            turbodbc_arrow_sources = turbodbc_sources + turbodbc_arrow_sources
        elif sys.platform == "darwin":
            pyarrow_module_link_args.append("-Wl,-rpath,@loader_path/pyarrow")
        else:
            pyarrow_module_link_args.append("-Wl,-rpath,$ORIGIN/pyarrow")
            if not is_cxx11_abi():
                extra_compile_args.append("-D_GLIBCXX_USE_CXX11_ABI=0")

        arrow_libs = pyarrow.get_libraries()

        arrow_lib_dirs = pyarrow.get_library_dirs()

        turbodbc_arrow = Extension(
            "turbodbc_arrow_support",
            sources=turbodbc_arrow_sources,
            include_dirs=include_dirs + [pyarrow_include_dir],
            extra_compile_args=extra_compile_args + hidden_visibility_args,
            libraries=[odbclib, "simdutf"] + arrow_libs + turbodbc_libs,
            extra_link_args=pyarrow_module_link_args,
            library_dirs=library_dirs + arrow_lib_dirs,
        )
        extension_modules.append(turbodbc_arrow)

    return extension_modules


here = os.path.abspath(os.path.dirname(__file__))
with open(os.path.join(here, "README.md")) as f:
    long_description = f.read()

setup(
    name="turbodbc",
    version="4.11.0",
    description="turbodbc is a Python DB API 2.0 compatible ODBC driver",
    long_description=long_description,
    long_description_content_type="text/markdown",
    include_package_data=True,
    url="https://github.com/blue-yonder/turbodbc",
    author="Michael Koenig",
    author_email="michael.koenig@blue-yonder.com",
    packages=["turbodbc"],
    setup_requires=[
        "pybind11>=2.10.4",
        "pyarrow>=7,<16",
        "numpy>=1.20",
    ],
    install_requires=[],
    extras_require={"arrow": ["pyarrow>=7.0,<16"], "numpy": "numpy>=1.20.0"},
    python_requires=">=3.9",
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: POSIX :: Linux",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: Microsoft :: Windows",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: Implementation :: CPython",
        "Topic :: Database",
    ],
    cmdclass=dict(build_ext=TurbodbcExtensionBuilder),
    ext_modules=get_extension_modules(),
)
