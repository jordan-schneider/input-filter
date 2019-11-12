#include "../utils.hpp"
#include "gtest/gtest.h"

namespace FilterModel {
const double ERROR = 0.0001;

TEST(assert_nonnegative, NonNegative) {
    ASSERT_NO_FATAL_FAILURE(assert_nonnegative<int>({0, 1, 2, 3}));
    ASSERT_NO_FATAL_FAILURE(assert_nonnegative<int>({1, 2, 3}));
    ASSERT_NO_FATAL_FAILURE(assert_nonnegative<int>({}));
}

TEST(assert_nonnegative, Negative) {
    ASSERT_DEATH(assert_nonnegative<int>({-1, 0, 1, 2, 3}), ".*");
    ASSERT_DEATH(assert_nonnegative<int>({0, 1, -1, 2, 3}), ".*");
    ASSERT_DEATH(assert_nonnegative<int>({0, 1, 2, 3, -1}), ".*");
    ASSERT_DEATH(assert_nonnegative<int>({-1}), ".*");
}

TEST(assert_probability, IsProbability) {
    ASSERT_NO_FATAL_FAILURE(assert_probability<int>({1}));
    ASSERT_NO_FATAL_FAILURE(assert_probability<double>({1.0}));
    ASSERT_NO_FATAL_FAILURE(assert_probability<double>({0.5, 0.5}));
    ASSERT_NO_FATAL_FAILURE(assert_probability<double>({0.1, 0.2, 0.3, 0.4}));
    ASSERT_NO_FATAL_FAILURE(assert_probability<double>({0.4, 0.3, 0.2, 0.1}));
}

TEST(assert_probability, IsNotProbability) {
    ASSERT_DEATH(assert_probability<int>({0}), ".*");
    ASSERT_DEATH(assert_probability<int>({5}), ".*");
    ASSERT_DEATH(assert_probability<int>({-1, 2}), ".*");
    ASSERT_DEATH(assert_probability<double>({}), ".*");
    ASSERT_DEATH(assert_probability<double>({0.1}), ".*");
    ASSERT_DEATH(assert_probability<double>({-0.5, 0.5, 0.5}), ".*");
    ASSERT_DEATH(assert_probability<double>({0.5, 0.5, 0.5}), ".*");
}

TEST(not_v, EmptyV) {
    std::vector<bool> empty = {};
    ASSERT_EQ(not_v(empty), empty);
}

TEST(not_v, SingleV) {
    ASSERT_EQ(not_v(std::vector<bool>({true})), std::vector<bool>({false}));
    ASSERT_EQ(not_v(std::vector<bool>({false})), std::vector<bool>({true}));
}

TEST(not_v, MultipleVs) {
    ASSERT_EQ(not_v(std::vector<bool>({true, true})), std::vector<bool>({false, false}));
    ASSERT_EQ(not_v(std::vector<bool>({true, false})), std::vector<bool>({false, true}));
    ASSERT_EQ(not_v(std::vector<bool>({true, true, true, true, true})),
              std::vector<bool>({false, false, false, false, false}));
}

TEST(bool_to_index, Empty) {
    ASSERT_EQ(bool_to_index<int>(std::vector<bool>({})), std::vector<int>({}));
    ASSERT_EQ(bool_to_index<int>(std::vector<bool>({}), false), std::vector<int>({}));
}

TEST(bool_to_index, Single) {
    ASSERT_EQ(bool_to_index<int>(std::vector<bool>({true})), std::vector<int>({0}));
    ASSERT_EQ(bool_to_index<int>(std::vector<bool>({true}), false), std::vector<int>({}));
}

TEST(bool_to_index, Many) {
    ASSERT_EQ(bool_to_index<int>(std::vector<bool>({true, false, true, true})),
              std::vector<int>({0, 2, 3}));
    ASSERT_EQ(bool_to_index<int>(std::vector<bool>({true, false, true, true}), false),
              std::vector<int>({1}));
}

TEST(stable_sum, ManyNumbers) {
    std::vector<double> numbers = {-4.0, -0.3, -0.1, -0.9};
    ASSERT_NEAR(stable_sum<>(numbers), -5.3, ERROR);
}

TEST(stable_sum, SmallNumbers) {
    std::vector<double> numbers = {1e-9, -1e-9};
    ASSERT_NEAR(stable_sum<>(numbers), 0.0, ERROR);
}

TEST(stable_sum, Empty) {
    std::vector<double> numbers;
    ASSERT_EQ(stable_sum<>(numbers), 0.0);
}

TEST(stable_sum, OneNumber) {
    std::vector<double> numbers = {0.9};
    ASSERT_NEAR(stable_sum<>(numbers), 0.9, ERROR);
}

TEST(vector_to_string, Empty) { ASSERT_EQ(vector_to_string(std::vector<int>({})), "[]"); }

TEST(vector_to_string, OneElement) { ASSERT_EQ(vector_to_string(std::vector<int>({1})), "[1]"); }

TEST(vector_to_string, MultipleElements) {
    ASSERT_EQ(vector_to_string(std::vector<int>({1, 2, 3})), "[1,2,3]");
}

TEST(vector_of_vector_to_string, Empty) {
    ASSERT_EQ(vector_of_vector_to_string(std::vector<std::vector<int>>()), "[]");
}

TEST(vector_of_vector_to_string, OneElementInOneVector) {
    std::vector<std::vector<int>> vv;
    vv.push_back(std::vector<int>({1}));
    ASSERT_EQ(vector_of_vector_to_string(vv), "[[1]]");
}

TEST(vector_of_vector_to_string, MultipleElementsInOneVector) {
    std::vector<std::vector<int>> vv;
    vv.push_back(std::vector<int>({1, 2, 3}));
    ASSERT_EQ(vector_of_vector_to_string(vv), "[[1,2,3]]");
}

TEST(vector_of_vector_to_string, MultipleElementsInMultipleVectors) {
    std::vector<std::vector<int>> vv;
    vv.push_back(std::vector<int>({1, 2, 3}));
    vv.push_back(std::vector<int>({4, 5, 6}));

    ASSERT_EQ(vector_of_vector_to_string(vv), "[[1,2,3],[4,5,6]]");
}

TEST(exp, Empty) { ASSERT_EQ(exp(std::vector<double>({})), std::vector<double>({})); }

TEST(exp, OneElement) {
    ASSERT_EQ(exp(std::vector<double>({0.0})), std::vector<double>({1.0}));
    ASSERT_EQ(exp(std::vector<double>({1.0})), std::vector<double>({M_E}));
    ASSERT_NEAR(exp(std::vector<double>({2.5}))[0], 12.1825, ERROR);
}

}  // namespace FilterModel