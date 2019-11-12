#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <vector>
#include "../model_distribution.hpp"
#include "../monte_carlo_integration.hpp"
#include "../multinomial.hpp"
#include "../multivariate_guassian.hpp"
#include "../types.hpp"
#include "../utils.hpp"
#include "gtest/gtest.h"

// Not sure why but these examples only match to a very small accuracy.

using namespace FilterModel;

namespace FilterModel {

TEST(integrate, XFromZeroToOne) {
    std::default_random_engine generator;

    std::function<double(double)> f = [](double x) { return x; };

    std::function<double(std::default_random_engine&)> sampler =
        [](std::default_random_engine& generator) {
            std::uniform_real_distribution<double> x_distribution(0.0, 1.0);

            return x_distribution(generator);
        };

    double volume = 1.0;

    double result = integrate(f, sampler, generator, 1, 100000);

    ASSERT_NEAR(result, 1.0 / 2.0, 0.01);
}

// TODO(joschnei): tests for some alpha != 1 should be written.
TEST(integrate, IntegrationCloseToExact) {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                        boost::log::trivial::debug);

    std::default_random_engine generator;

    alpha_t alpha({true, true, true});
    category_counts_t object_counts({100, 100, 100});
    int n_positive = 200;
    int n_negative = 100;
    int n = n_positive + n_negative;

    delta_t delta({1.0 / 3.0, 1.0 / 2.0, 1.0 / 6.0});

    // Exact
    double sum_over_k_negative_exact = ModelDistribution::calculate_sum_over_k_negative_exact(
        n_positive, n_negative, alpha, delta, object_counts);

    // Approx
    ModelDistribution model_distribution({}, generator, false);
    double sum_over_k_negative_approx = model_distribution.calculate_sum_over_k_negative_approx_2(
        n_positive, n_negative, alpha, delta, object_counts);

    ASSERT_NEAR(sum_over_k_negative_approx, sum_over_k_negative_exact, 0.01);
}

}  // namespace FilterModel