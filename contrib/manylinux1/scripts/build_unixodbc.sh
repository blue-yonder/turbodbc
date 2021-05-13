wget http://www.unixodbc.org/unixODBC-2.3.4.tar.gz
tar xf unixODBC-2.3.4.tar.gz
pushd /unixODBC-2.3.4
./configure --prefix=/usr
make -j5
make install
popd
rm -rf unixODBC-2.3.4.tar.gz unixODBC-2.3.4
