from setuptools import setup, Extension, Distribution
import setuptools.command.build_ext

import sys
import sysconfig
import os
import os.path
import distutils.sysconfig

import itertools
from glob import iglob


def _get_turbodbc_libname():
    builder = setuptools.command.build_ext.build_ext(Distribution())
    full_name = builder.get_ext_filename('libturbodbc')
    without_lib = full_name.split('lib', 1)[-1]
    without_so = without_lib.rsplit('.so', 1)[0]
    return without_so


def _get_distutils_build_directory():
    """
    Returns the directory distutils uses to build its files.
    We need this directory since we build extensions which have to link
    other ones.
    """
    pattern = "lib.{platform}-{major}.{minor}"
    return os.path.join('build', pattern.format(platform=sysconfig.get_platform(),
                                                major=sys.version_info[0],
                                                minor=sys.version_info[1]))


def _get_source_files(directory):
    path = os.path.join('src', directory)
    iterable_sources = (iglob(os.path.join(root,'*.cpp')) for root, dirs, files in os.walk(path))
    source_files = itertools.chain.from_iterable(iterable_sources)
    return list(source_files)


def _remove_strict_prototype_option_from_distutils_config():
    strict_prototypes = '-Wstrict-prototypes'
    config = distutils.sysconfig.get_config_vars()
    for key, value in config.items():
        if strict_prototypes in str(value):
            config[key] = config[key].replace(strict_prototypes, '')

_remove_strict_prototype_option_from_distutils_config()


def _has_arrow_headers():
    try:
        import pyarrow
        return True
    except:
        return False


def _has_numpy_headers():
    try:
        import numpy
        return True
    except:
        return False



class _deferred_pybind11_include(object):
    def __str__(self):
        import pybind11
        return pybind11.get_include()


extra_compile_args = []
hidden_visibility_args = []
include_dirs = ['include/', _deferred_pybind11_include()]

library_dirs = [_get_distutils_build_directory()]
python_module_link_args = []
base_library_link_args = []

if sys.platform == 'darwin':
    extra_compile_args.append('--std=c++11')
    extra_compile_args.append('--stdlib=libc++')
    extra_compile_args.append('-mmacosx-version-min=10.9')
    hidden_visibility_args.append('-fvisibility=hidden')
    include_dirs.append(os.getenv('UNIXODBC_INCLUDE_DIR', '/usr/local/include/'))
    library_dirs.append(os.getenv('UNIXODBC_LIBRARY_DIR', '/usr/local/lib/'))

    from distutils import sysconfig
    vars = sysconfig.get_config_vars()
    vars['LDSHARED'] = vars['LDSHARED'].replace('-bundle', '')
    python_module_link_args.append('-bundle')
    builder = setuptools.command.build_ext.build_ext(Distribution())
    full_name = builder.get_ext_filename('libturbodbc')
    base_library_link_args.append('-Wl,-dylib_install_name,@loader_path/{}'.format(full_name))
    base_library_link_args.append('-dynamiclib')
    odbclib = 'odbc'
elif sys.platform == 'win32':
    extra_compile_args.append('-DNOMINMAX')
    if 'BOOST_ROOT' in os.environ:
        include_dirs.append(os.getenv('BOOST_ROOT'))
        library_dirs.append(os.path.join(os.getenv('BOOST_ROOT'), "stage", "lib"))
        library_dirs.append(os.path.join(os.getenv('BOOST_ROOT'), "lib64-msvc-14.0"))
    else:
        print("warning: BOOST_ROOT enviroment variable not set")
    odbclib = 'odbc32'
else:
    extra_compile_args.append('--std=c++11')
    hidden_visibility_args.append('-fvisibility=hidden')
    python_module_link_args.append("-Wl,-rpath,$ORIGIN")
    if 'UNIXODBC_INCLUDE_DIR' in os.environ:
        include_dirs.append(os.getenv('UNIXODBC_INCLUDE_DIR'))
    if 'UNIXODBC_LIBRARY_DIR' in os.environ:
        library_dirs.append(os.getenv('UNIXODBC_LIBRARY_DIR'))
    odbclib = 'odbc'


def get_extension_modules():
    extension_modules = []

    """
    Extension module which is actually a plain C++ library without Python bindings
    """
    turbodbc_sources = _get_source_files('cpp_odbc') + _get_source_files('turbodbc')
    turbodbc_library = Extension('libturbodbc',
                                 sources=turbodbc_sources,
                                 include_dirs=include_dirs,
                                 extra_compile_args=extra_compile_args,
                                 extra_link_args=base_library_link_args,
                                 libraries=[odbclib],
                                 library_dirs=library_dirs)
    if sys.platform == "win32":
        turbodbc_libs = []
    else:
        turbodbc_libs = [_get_turbodbc_libname()]
        extension_modules.append(turbodbc_library)

    """
    An extension module which contains the main Python bindings for turbodbc
    """
    turbodbc_python_sources = _get_source_files('turbodbc_python')
    if sys.platform == "win32":
        turbodbc_python_sources = turbodbc_sources + turbodbc_python_sources
    turbodbc_python = Extension('turbodbc_intern',
                                sources=turbodbc_python_sources,
                                include_dirs=include_dirs,
                                extra_compile_args=extra_compile_args + hidden_visibility_args,
                                libraries=[odbclib] + turbodbc_libs,
                                extra_link_args=python_module_link_args,
                                library_dirs=library_dirs)
    extension_modules.append(turbodbc_python)

    """
    An extension module which contains Python bindings which require numpy support
    to work. Not included in the standard Python bindings so this can stay optional.
    """
    if _has_numpy_headers():
        import numpy
        turbodbc_numpy_sources = _get_source_files('turbodbc_numpy')
        if sys.platform == "win32":
            turbodbc_numpy_sources = turbodbc_sources + turbodbc_numpy_sources
        turbodbc_numpy = Extension('turbodbc_numpy_support',
                                   sources=turbodbc_numpy_sources,
                                   include_dirs=include_dirs + [numpy.get_include()],
                                   extra_compile_args=extra_compile_args + hidden_visibility_args,
                                   libraries=[odbclib] + turbodbc_libs,
                                   extra_link_args=python_module_link_args,
                                   library_dirs=library_dirs)
        extension_modules.append(turbodbc_numpy)

    """
    An extension module which contains Python bindings which require Apache Arrow
    support to work. Not included in the standard Python bindings so this can
    stay optional.
    """
    if _has_arrow_headers():
        import pyarrow
        pyarrow_location = os.path.dirname(pyarrow.__file__)
        # For now, assume that we build against bundled pyarrow releases.
        pyarrow_include_dir = os.path.join(pyarrow_location, 'include')
        turbodbc_arrow_sources = _get_source_files('turbodbc_arrow')
        pyarrow_module_link_args = list(python_module_link_args)
        if sys.platform == "win32":
            turbodbc_arrow_sources = turbodbc_sources + turbodbc_arrow_sources
        elif sys.platform == "darwin":
            pyarrow_module_link_args.append('-Wl,-rpath,@loader_path/pyarrow')
        else:
            pyarrow_module_link_args.append("-Wl,-rpath,$ORIGIN/pyarrow")
        turbodbc_arrow = Extension('turbodbc_arrow_support',
                                   sources=turbodbc_arrow_sources,
                                   include_dirs=include_dirs + [pyarrow_include_dir],
                                   extra_compile_args=extra_compile_args + hidden_visibility_args,
                                   libraries=[odbclib, 'arrow', 'arrow_python'] + turbodbc_libs,
                                   extra_link_args=pyarrow_module_link_args,
                                   library_dirs=library_dirs + [pyarrow_location])
        extension_modules.append(turbodbc_arrow)

    return extension_modules


here = os.path.abspath(os.path.dirname(__file__))
with open(os.path.join(here, 'README.md')) as f:
    long_description = f.read()


setup(name = 'turbodbc',
      version = '4.1.2',
      description = 'turbodbc is a Python DB API 2.0 compatible ODBC driver',
      long_description=long_description,
      long_description_content_type='text/markdown',
      include_package_data = True,
      url = 'https://github.com/blue-yonder/turbodbc',
      author='Michael Koenig',
      author_email = 'michael.koenig@blue-yonder.com',
      packages = ['turbodbc'],
      extras_require={
          'arrow': ['pyarrow>=0.15,<3.1.0'],
          'numpy': 'numpy>=1.16.0'
      },
      classifiers = ['Development Status :: 5 - Production/Stable',
                     'Intended Audience :: Developers',
                     'License :: OSI Approved :: MIT License',
                     'Operating System :: POSIX :: Linux',
                     'Operating System :: MacOS :: MacOS X',
                     'Operating System :: Microsoft :: Windows',
                     'Programming Language :: C++',
                     'Programming Language :: Python :: 3.6',
                     'Programming Language :: Python :: 3.7',
                     'Programming Language :: Python :: 3.8',
                     'Programming Language :: Python :: Implementation :: CPython',
                     'Topic :: Database'],
      ext_modules = get_extension_modules(),
      install_requires=['pybind11>=2.2.0']
      )
