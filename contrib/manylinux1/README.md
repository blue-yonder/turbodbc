## Manylinux1 wheels for Turbodbc

This folder provides base Docker images and an infrastructure to build
`manylinux1` compatible Python wheels that should be installable on all
Linux distributions published in last four years.

The process is split up in two parts: There is the base Docker image that
builds the native, Python-indenpendent dependencies, then a script builds the
manylinux1 wheels for various Python versions.

### Build instructions

```bash
# Create a clean copy of the turbodbc source tree
git clone ../../ turbodbc
# Build the baseimage with the native dependencies
docker build -t turbodbc-manylinux1-x86_64 -f Dockerfile-x86_64 .
# Build the manylinux1 wheels
docker run --rm -ti -v $PWD:/io turbodbc-manylinux1-x86_64 /io/build_turbodbc.sh
# Now the new packages are located in the dist/ folder
ls -l dist/
```
