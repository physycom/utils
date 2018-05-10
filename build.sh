#!/bin/bash

<<<<<<< HEAD
rm -rf build
mkdir -p build
cd build
cmake ..
cmake --build .
=======
mkdir -p build
cd build
cmake ..
cmake --build . --target install
>>>>>>> 7b7d0af1052671707ebfd66e3a55b4c99f16f77f
cd ..
