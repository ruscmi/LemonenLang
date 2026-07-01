#!/bin/bash

mkdir -p build && cd build
# В скрипте:
cmake -DCMAKE_BUILD_TYPE=Release ..
make 
