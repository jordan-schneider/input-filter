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
include CMakeFiles/VectorFillTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/VectorFillTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/VectorFillTest.dir/flags.make

CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.o: CMakeFiles/VectorFillTest.dir/flags.make
CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.o: ../tests/vector_fill_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/joschnei/input-filter/c++/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.o -c /home/joschnei/input-filter/c++/tests/vector_fill_test.cpp

CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/joschnei/input-filter/c++/tests/vector_fill_test.cpp > CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.i

CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/joschnei/input-filter/c++/tests/vector_fill_test.cpp -o CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.s

# Object files for target VectorFillTest
VectorFillTest_OBJECTS = \
"CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.o"

# External object files for target VectorFillTest
VectorFillTest_EXTERNAL_OBJECTS =

bin/VectorFillTest: CMakeFiles/VectorFillTest.dir/tests/vector_fill_test.cpp.o
bin/VectorFillTest: CMakeFiles/VectorFillTest.dir/build.make
bin/VectorFillTest: lib/libgtest_main.a
bin/VectorFillTest: lib/libgtest.a
bin/VectorFillTest: CMakeFiles/VectorFillTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/joschnei/input-filter/c++/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/VectorFillTest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/VectorFillTest.dir/link.txt --verbose=$(VERBOSE)
	/home/joschnei/.local/lib/python3.8/site-packages/cmake/data/bin/cmake -D TEST_TARGET=VectorFillTest -D TEST_EXECUTABLE=/home/joschnei/input-filter/c++/build/bin/VectorFillTest -D TEST_EXECUTOR= -D TEST_WORKING_DIR=/home/joschnei/input-filter/c++/build -D TEST_EXTRA_ARGS= -D TEST_PROPERTIES= -D TEST_PREFIX= -D TEST_SUFFIX= -D NO_PRETTY_TYPES=FALSE -D NO_PRETTY_VALUES=FALSE -D TEST_LIST=VectorFillTest_TESTS -D CTEST_FILE=/home/joschnei/input-filter/c++/build/VectorFillTest[1]_tests.cmake -D TEST_DISCOVERY_TIMEOUT=5 -P /home/joschnei/.local/lib/python3.8/site-packages/cmake/data/share/cmake-3.15/Modules/GoogleTestAddTests.cmake

# Rule to build all files generated by this target.
CMakeFiles/VectorFillTest.dir/build: bin/VectorFillTest

.PHONY : CMakeFiles/VectorFillTest.dir/build

CMakeFiles/VectorFillTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/VectorFillTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/VectorFillTest.dir/clean

CMakeFiles/VectorFillTest.dir/depend:
	cd /home/joschnei/input-filter/c++/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/joschnei/input-filter/c++ /home/joschnei/input-filter/c++ /home/joschnei/input-filter/c++/build /home/joschnei/input-filter/c++/build /home/joschnei/input-filter/c++/build/CMakeFiles/VectorFillTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/VectorFillTest.dir/depend

