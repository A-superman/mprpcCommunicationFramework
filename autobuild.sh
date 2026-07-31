set -e
mkdir -p ../build
rm -rf ../build/*
cd ../build &&
    cmake ../mprpcCommunicationFramework && 
    make
cd ../mprpcCommunicationFramework
cp -r ./src/include ../build/lib
cp -r ./bin/test.conf ../build/bin
