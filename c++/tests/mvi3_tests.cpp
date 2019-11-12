#include "../model_distribution.hpp"
#include "../mvi3/mvi3.hpp"
#include "../types.hpp"
#include "../utils.hpp"
#include "gtest/gtest.h"

#include <vector>

// Not sure why but these examples only match to a very small accuracy.

using namespace FilterModel;

namespace MVI3 {
TEST(Integrate, Example1) {
    Mvi3 mvi3;
    std::vector<std::vector<double>> covariance;
    covariance.push_back(std::vector<double>({1}));
    covariance.push_back(std::vector<double>({0, 1}));
    covariance.push_back(std::vector<double>({0, 0, 1}));

    std::vector<std::vector<double>> hyperplanes;
    hyperplanes.push_back(std::vector<double>({-1, 0, 0, 1}));
    hyperplanes.push_back(std::vector<double>({1, 0, 0, 2}));
    hyperplanes.push_back(std::vector<double>({0, -1, 0, 2.1}));
    hyperplanes.push_back(std::vector<double>({0, 1, 0, 1.4}));
    hyperplanes.push_back(std::vector<double>({0, 0, -1, 0.5}));

    ASSERT_NEAR(mvi3.integrate(457, 30, 1000, 10, covariance, hyperplanes), 5.003167E-01, 0.01);
}

TEST(Integrate, Example2) {
    Mvi3 mvi3;
    std::vector<std::vector<double>> covariance;
    covariance.push_back(std::vector<double>({1}));
    covariance.push_back(std::vector<double>({0, 1}));
    covariance.push_back(std::vector<double>({0, 0, 1}));

    std::vector<std::vector<double>> hyperplanes;
    hyperplanes.push_back(std::vector<double>({-1, 0, 0, 1}));
    hyperplanes.push_back(std::vector<double>({1, 0, 0, 2}));
    hyperplanes.push_back(std::vector<double>({0, -1, 0, 2.1}));
    hyperplanes.push_back(std::vector<double>({0, 1, 0, 1.4}));
    hyperplanes.push_back(std::vector<double>({0, 0, -1, 0.5}));

    ASSERT_NEAR(mvi3.integrate(457, 30, 10000, 1, covariance, hyperplanes), 5.003167E-01, 0.01);
}

TEST(Integrate, Example3) {
    Mvi3 mvi3;
    std::vector<std::vector<double>> covariance;
    covariance.push_back(std::vector<double>({1}));
    covariance.push_back(std::vector<double>({0, 1}));
    covariance.push_back(std::vector<double>({0, 0, 1}));

    std::vector<std::vector<double>> hyperplanes;
    hyperplanes.push_back(std::vector<double>({-1, 0, 0, 1}));
    hyperplanes.push_back(std::vector<double>({1, 0, 0, 2}));
    hyperplanes.push_back(std::vector<double>({0, -1, 0, 2.1}));
    hyperplanes.push_back(std::vector<double>({0, 1, 0, 1.4}));
    hyperplanes.push_back(std::vector<double>({0, 0, -1, 0.5}));

    ASSERT_NEAR(mvi3.integrate(457, 30, 100, 100, covariance, hyperplanes), 5.003167E-01, 0.01);
}

// TODO(joschnei): This test currently only works for all alpha = 1. When this issue is fixed,
// tests for some alpha != 1 should be written.
TEST(Integrate, IntegrationCloseToExact1) {
    alpha_t alpha({true, true, true});
    category_counts_t object_counts({100, 100, 100});
    int n_positive = 200;
    int n_negative = 100;
    int n = n_positive + n_negative;

    delta_t delta({1.0 / 3.0, 1.0 / 2.0, 1.0 / 6.0});

    double sum_over_k_negative_exact = ModelDistribution::calculate_sum_over_k_negative_exact(
        n_positive, n_negative, alpha, delta, object_counts);

    // Approx
    volatile double sum_over_k_negative_approx =
        ModelDistribution::calculate_sum_over_k_negative_approx(n_positive, n_negative, alpha,
                                                                delta, object_counts);
    ASSERT_NEAR(sum_over_k_negative_approx, sum_over_k_negative_exact, 0.04);
}

TEST(Integrate, IntegrationCloseToExact2) {
    alpha_t alpha({true, true, true});
    category_counts_t object_counts({100, 100, 100});
    int n_positive = 100;
    int n_negative = 200;
    int n = n_positive + n_negative;

    delta_t delta({1.0 / 3.0, 1.0 / 2.0, 1.0 / 6.0});

    double sum_over_k_negative_exact = ModelDistribution::calculate_sum_over_k_negative_exact(
        n_positive, n_negative, alpha, delta, object_counts);

    // Approx
    double sum_over_k_negative_approx = ModelDistribution::calculate_sum_over_k_negative_approx(
        n_positive, n_negative, alpha, delta, object_counts);
    ASSERT_NEAR(sum_over_k_negative_approx, sum_over_k_negative_exact, 0.03);
}

TEST(Integrate, IntegrationCloseToExact3) {
    alpha_t alpha({true, false, true});
    category_counts_t object_counts({100, 100, 100});
    int n_positive = 100;
    int n_negative = 200;
    int n = n_positive + n_negative;

    delta_t delta({1.0 / 3.0, 1.0 / 2.0, 1.0 / 6.0});

    double sum_over_k_negative_exact = ModelDistribution::calculate_sum_over_k_negative_exact(
        n_positive, n_negative, alpha, delta, object_counts);

    // Approx
    double sum_over_k_negative_approx = ModelDistribution::calculate_sum_over_k_negative_approx(
        n_positive, n_negative, alpha, delta, object_counts);
    ASSERT_NEAR(sum_over_k_negative_approx, sum_over_k_negative_exact, 0.01);
}

// TEST(Integrate, IntegrationCloseToExact4) {
//     alpha_t alpha({true, true, true});
//     category_counts_t object_counts({135453, 184001, 2019190});
//     int n_positive = 184001;
//     int n_negative = 2154643;
//     int n = n_positive + n_negative;

//     delta_t delta({1.0 / 3.0, 1.0 / 2.0, 1.0 / 6.0});

//     double sum_over_k_negative_exact = ModelDistribution::calculate_sum_over_k_negative_exact(
//         n_positive, n_negative, alpha, delta, object_counts);

//     // Approx
//     double sum_over_k_negative_approx = ModelDistribution::calculate_sum_over_k_negative_approx(
//         n_positive, n_negative, alpha, delta, object_counts);
//     ASSERT_NEAR(sum_over_k_negative_approx, sum_over_k_negative_exact, 0.01);
// }

}  // namespace MVI3