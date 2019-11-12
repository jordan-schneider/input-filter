cmake_minimum_required(VERSION 3.10.0)
project(FilterModel VERSION 0.1.0)

enable_testing ()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Download and install google test
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()
# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)
# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

set (CMAKE_CXX_STANDARD 14)

# Libraries

add_library(Mvi3 mvi3/mvi3.cpp)

add_library(Multinomial multinomial.cpp)
target_link_libraries(Multinomial CONAN_PKG::boost)

add_library(MultivariateGuassian multivariate_guassian.cpp)
target_link_libraries(MultivariateGuassian Multinomial CONAN_PKG::boost CONAN_PKG::eigen gtest_main)

add_library(ModelDistribution model_distribution.cpp)
target_link_libraries(ModelDistribution Multinomial MultivariateGuassian Mvi3 CONAN_PKG::boost gtest_main)

add_library(SampleModels sample_models.cpp)
target_link_libraries(SampleModels ModelDistribution CONAN_PKG::boost)

# Executables

add_executable(JointDistributionLearner joint_distribution.cpp)
target_link_libraries(JointDistributionLearner SampleModels ModelDistribution CONAN_PKG::boost CONAN_PKG::cli11)

add_executable(InnerLoop inner_loop.cpp)
target_link_libraries(InnerLoop Multinomial CONAN_PKG::cli11)

# Test executables

include(GoogleTest)

add_executable(UtilsTests tests/utils_tests.cpp)
target_link_libraries(UtilsTests ModelDistribution gtest_main)
gtest_discover_tests(UtilsTests)

add_executable(SampleModelsTests tests/sample_models_tests.cpp)
target_link_libraries(SampleModelsTests SampleModels gtest_main)
gtest_discover_tests(SampleModelsTests)

add_executable(MultivariateGuassianTests tests/multivariate_guassian_tests.cpp)
target_link_libraries(MultivariateGuassianTests MultivariateGuassian gtest_main CONAN_PKG::boost)
gtest_discover_tests(MultivariateGuassianTests)

add_executable(VectorFillTest tests/vector_fill_test.cpp)
target_link_libraries(VectorFillTest gtest_main)
gtest_discover_tests(VectorFillTest)

add_executable(MultinomialTests tests/multinomial_tests.cpp)
target_link_libraries(MultinomialTests Multinomial gtest_main)
gtest_discover_tests(MultinomialTests)

add_executable(ModelDistributionTests tests/model_distribution_tests.cpp)
target_link_libraries(ModelDistributionTests ModelDistribution gtest_main)
gtest_discover_tests(ModelDistributionTests)

add_executable(BoostTest tests/boost_test.cpp)
target_link_libraries(BoostTest gtest_main CONAN_PKG::boost)
gtest_discover_tests(BoostTest)

add_executable(RandomnessTests tests/randomness_tests.cpp)
target_link_libraries(RandomnessTests gtest_main)
gtest_discover_tests(RandomnessTests)

add_executable(MetropolisHastingsTests tests/metropolis_hastings_tests.cpp)
target_link_libraries(MetropolisHastingsTests gtest_main CONAN_PKG::boost)
gtest_discover_tests(MetropolisHastingsTests)

add_executable(Mvi3Tests tests/mvi3_tests.cpp)
target_link_libraries(Mvi3Tests Mvi3 ModelDistribution gtest_main)