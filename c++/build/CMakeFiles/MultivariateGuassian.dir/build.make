# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/joschnei/.local/lib/python3.8/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /home/joschnei/.local/lib/python3.8/site-packages/cmake/data/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/joschnei/input-filter/c++

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/joschnei/input-filter/c++/build

# Include any dependencies generated for this target.
include CMakeFiles/MultivariateGuassian.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MultivariateGuassian.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MultivariateGuassian.dir/flags.make

CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.o: CMakeFiles/MultivariateGuassian.dir/flags.make
CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.o: ../multivariate_guassian.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/joschnei/input-filter/c++/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.o -c /home/joschnei/input-filter/c++/multivariate_guassian.cpp

CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/joschnei/input-filter/c++/multivariate_guassian.cpp > CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.i

CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/joschnei/input-filter/c++/multivariate_guassian.cpp -o CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.s

# Object files for target MultivariateGuassian
MultivariateGuassian_OBJECTS = \
"CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.o"

# External object files for target MultivariateGuassian
MultivariateGuassian_EXTERNAL_OBJECTS =

lib/libMultivariateGuassian.a: CMakeFiles/MultivariateGuassian.dir/multivariate_guassian.cpp.o
lib/libMultivariateGuassian.a: CMakeFiles/MultivariateGuassian.dir/build.make
lib/libMultivariateGuassian.a: CMakeFiles/MultivariateGuassian.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/joschnei/input-filter/c++/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library lib/libMultivariateGuassian.a"
	$(CMAKE_COMMAND) -P CMakeFiles/MultivariateGuassian.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MultivariateGuassian.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MultivariateGuassian.dir/build: lib/libMultivariateGuassian.a

.PHONY : CMakeFiles/MultivariateGuassian.dir/build

CMakeFiles/MultivariateGuassian.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MultivariateGuassian.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MultivariateGuassian.dir/clean

CMakeFiles/MultivariateGuassian.dir/depend:
	cd /home/joschnei/input-filter/c++/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/joschnei/input-filter/c++ /home/joschnei/input-filter/c++ /home/joschnei/input-filter/c++/build /home/joschnei/input-filter/c++/build /home/joschnei/input-filter/c++/build/CMakeFiles/MultivariateGuassian.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MultivariateGuassian.dir/depend
