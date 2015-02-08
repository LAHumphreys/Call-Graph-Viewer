#!/bin/bash

mkdir CEF_3.2

pushd DEV_TOOLS/EXTERNAL_LIBS/CEF/

./rebuild.sh
popd

mv DEV_TOOLS/EXTERNAL_LIBS/CEF/cef_binary_3.2171.1979_linux64.7z CEF_3.2/

pushd CEF_3.2

7z x cef_binary_3.2171.1979_linux64.7z
rm cef_binary_3.2171.1979_linux64.7z

pushd cef_binary_3.2171.1979_linux64

CXX=g++-4.8 make -j 8 libcef_dll_wrapper BUILDTYPE=Debug
CXX=g++-4.8 make -j 8 libcef_dll_wrapper BUILDTYPE=Release

mv  Debug/* out/Debug/
mv  Release/* out/Release/

echo "Giving SUID permissions to chrome-sandbox..."
echo "(using sudo so you may be asked for your password)"

sudo -- chown root:root out/Debug/chrome-sandbox 
sudo -- chmod 4755 out/Debug/chrome-sandbox

sudo -- chown root:root out/Release/chrome-sandbox 
sudo -- chmod 4755 out/Release/chrome-sandbox

echo "CEF_DIR = $PWD"
