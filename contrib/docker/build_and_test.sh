#!/bin/bash

# This should run in the 'turbodbc' docker service

set -ex

mkdir -p /turbodbc/build_docker && cd /turbodbc/build_docker
cmake -DCMAKE_INSTALL_PREFIX=./dist -DPYBIND11_PYTHON_VERSION=3.5 ..
make -j4


# run all tests (C++ and python tests):
ctest --verbose

# to just run the python integration tests use these commands:
# cd /turbodbc/python/turbodbc_test
# pytest
