# Multiclass Input Filtering Model

This project contains a graphical model that attempts to infer which combination of classes certain
objects are allowed to be used in combination with. For example, one might want to learn which noun
pluralities each determiner can be used with (the current case). It does this by performing
inference over a graphical model. This graphical model understands that some obversations will be
made in error, and thus can filter out some degree of noise.

## Build

This project is primarily in c++. It uses [conan](https://conan.io/) for package managment and
[CMake](https://cmake.org/) for build. There are also some utility scripts in python3.

Perform the following steps to build the project
1. Install [conan](https://docs.conan.io/en/latest/installation.html). You'll need python and pip
for this if you don't already have it.
1. Install [CMake](https://cmake.org/install/).
1. Create a build folder in c++ with `mkdir c++/build`
1. Navigate to this build folder, and use conan to install c++ dependencies with
`cd c++/build; conan install ..`. This might take a while.
1. Return to the c++ folder and use the provided build.sh script to use CMake to build the code.
`cd ..; ./build.sh`. You may need to enable execute permissions on the script with
`chmod +x build.sh`.

## Run
After building the code, you can run the code on your data from the root directory.

1. If you're not already there, return to the root directory.
1. Run `c++/build/bin/JointDistributionLearner -i [in_file] -o [out_file] --iterations [iters]`