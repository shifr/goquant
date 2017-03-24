#/usr/bin/bash
echo "Updating system packages..."
apt-get update

echo "Installing libpng..."
apt-get install libpng-dev -y

echo "Building libimagequant..."
cd $GOPATH/src/github.com/shifr/goquant/src/lib
./configure && make static;

echo "Copying libimagequant..."
cp libimagequant.a /usr/local/lib/libimagequant.a
cd ../

echo "Building rwpng..."
gcc -c -std=c99 -fPIC -w rwpng.c
echo "Building librwpng..."
gcc -std=c99 -shared -o /usr/local/lib/librwpng.so rwpng.o -lpng
