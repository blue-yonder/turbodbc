set -exuo pipefail


mkdir build 
cd build 
cmake -DBUILD_COVERAGE=ON -DCMAKE_INSTALL_PREFIX=./dist -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_DEFAULT_CMP0074=NEW -GNinja .. 
ninja  
cmake --build . --target install 
cd dist 
python -m pip install --no-build-isolation --no-deps --disable-pip-version-check -e .
