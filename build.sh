#!/bin/sh
mkdir -p build
cd build
cmake -B . -S .. "${@}" 
make
