wget --no-check-certificate http://downloads.sourceforge.net/project/boost/boost/1.64.0/boost_1_64_0.tar.gz -O /boost_1_64_0.tar.gz
tar xf boost_1_64_0.tar.gz
pushd /boost_1_64_0
./bootstrap.sh
./bjam cxxflags=-fPIC cflags=-fPIC --prefix=/usr --with-filesystem --with-date_time --with-system --with-regex --with-locale install
popd
rm -rf boost_1_64_0.tar.gz boost_1_64_0
