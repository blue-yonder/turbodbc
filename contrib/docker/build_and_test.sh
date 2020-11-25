#!/bin/bash

# This should run in the 'turbodbc' docker service. It will
# build turbodbc and run the tests.

set -ex

pyarrow_path=`python -c 'import pyarrow; print(pyarrow.__path__[0])'`
pyarrow_so_version=`python -c 'import pyarrow; print(pyarrow.cpp_build_info.so_version)'`
ln -s $pyarrow_path/libarrow.so.${pyarrow_so_version} $pyarrow_path/libarrow.so
ln -s $pyarrow_path/libarrow_python.so.${pyarrow_so_version} $pyarrow_path/libarrow_python.so

mkdir -p /turbodbc/build_docker && cd /turbodbc/build_docker
cmake -DCMAKE_INSTALL_PREFIX=./dist -DPYBIND11_PYTHON_VERSION=3.7 -DDISABLE_CXX11_ABI=ON -GNinja ..
ninja


# run all tests (C++ and python tests):
ctest --verbose

# to just run the python integration tests use these commands:
# cd /turbodbc/python/turbodbc_test
# pytest
