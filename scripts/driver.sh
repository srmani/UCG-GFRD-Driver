#!/bin/bash
#set -e
mkdir build
cd build
cmake ..
make
#make test
cd ../
pwd
