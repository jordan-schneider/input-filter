#include <boost/log/trivial.hpp>
#include "../multivariate_guassian.hpp"
#include "gtest/gtest.h"

namespace FilterModel {
const double ERROR = 0.0001;

TEST(density, OneDimensionalAtZeroCorrect) {
    MultivariateGuassian normal({0}, {{1}});
    ASSERT_NEAR(normal.density({0.0}), 1.0 / std::sqrt(2.0 * M_PI), ERROR);
}

TEST(density, OneDimensionalAtOneCorrect) {
    MultivariateGuassian normal({0}, {{1}});
    ASSERT_NEAR(normal.density({1.0}), std::exp(-1.0 / 2.0) / std::sqrt(2.0 * M_PI), ERROR);
}

TEST(density, OneDimensionalAtTwoCorrect) {
    MultivariateGuassian normal({0}, {{1}});
    ASSERT_NEAR(normal.density({2.0}), std::exp(-4.0 / 2.0) / std::sqrt(2.0 * M_PI), ERROR);
}

TEST(density, TwoDimensionalAtZeroCorrect) {
    MultivariateGuassian normal({0, 0}, {{1, 0}, {0, 1}});
    ASSERT_NEAR(normal.density({0.0, 0.0}), std::exp(0) / (2.0 * M_PI), ERROR);
}

}  // namespace FilterModel