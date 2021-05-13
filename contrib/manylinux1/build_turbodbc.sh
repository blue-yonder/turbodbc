#!/bin/bash
# Usage:
#   docker run --rm -v $PWD:/io turbodbc-manylinux1-x86_64 /io/build_turbodbc.sh

# Build upon the scripts in https://github.com/matthew-brett/manylinux-builds
# * Copyright (c) 2013-2016, Matt Terry and Matthew Brett (BSD 2-clause)

PYTHON_VERSIONS="${PYTHON_VERSIONS:-2.7 3.4 3.5 3.6}"

# Package index with only manylinux1 builds
MANYLINUX_URL=https://nipy.bic.berkeley.edu/manylinux

source /multibuild/manylinux_utils.sh

# Quit on failure
set -e

cd /io/turbodbc

mkdir -p dist
git submodule update --init --recursive

for PYTHON in ${PYTHON_VERSIONS}; do
    PYTHON_INTERPRETER="$(cpython_path $PYTHON)/bin/python"
    PIP="$(cpython_path $PYTHON)/bin/pip"
    PIPI_IO="$PIP install -f $MANYLINUX_URL"
    PATH="$PATH:$(cpython_path $PYTHON)/bin"

    rm -rf build-$PYTHON
    mkdir build-$PYTHON
    pushd build-$PYTHON
    virtualenv venv -p ${PYTHON_INTERPRETER}
    source ./venv/bin/activate
    LD_LIBRARY_PATH=$(pwd)/venv/lib/python${PYTHON}/site-packages/pyarrow
    pip install numpy==1.10.4 pyarrow pybind11
    cmake -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=./dist -DPYBIND11_PYTHON_VERSION=${PYTHON} ..
    make -j5 install
    pushd dist
    python setup.py bdist_wheel
    auditwheel -v repair -L . dist/turbodbc-*.whl -w repaired_wheels/
    mv repaired_wheels/*.whl /io/dist

    popd
    popd
done

