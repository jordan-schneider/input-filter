#!/bin/bash
cd c++/
mkdir build
cd build
conan install .. --build=missing --settings compiler.libcxx="libstdc++11"
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target JointDistributionLearner --
cd ../..