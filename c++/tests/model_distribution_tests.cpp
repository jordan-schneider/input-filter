#include "../model_distribution.hpp"
#include "gtest/gtest.h"

double ERROR = 0.0001;

namespace FilterModel {

TEST(iterate_over_k_negatives, WorksWithAlphaTrue) {
    int n_positive = 3;
    int n_negative = 3;
    std::vector<int> k = {2, 2, 2};
    alpha_t alpha = {true, true, true};

    std::vector<std::vector<int>> k_negatives;
    ModelDistribution::iterate_over_k_negatives(
        [&k_negatives](int k_1_negative, int k_2_negative, int k_3_negative) {
            k_negatives.push_back(std::vector<int>({k_1_negative, k_2_negative, k_3_negative}));
        },
        n_positive, n_negative, alpha, k);

    ASSERT_EQ(k_negatives,
              std::vector<std::vector<int>>(
                  {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 1, 1}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}}));
}

TEST(iterate_over_k_negatives, WorksWithFirstAlphaFalse) {
    int n_positive = 3;
    int n_negative = 3;
    std::vector<int> k = {2, 2, 2};
    alpha_t alpha = {false, true, true};

    std::vector<std::vector<int>> k_negatives;
    ModelDistribution::iterate_over_k_negatives(
        [&k_negatives](int k_1_negative, int k_2_negative, int k_3_negative) {
            k_negatives.push_back(std::vector<int>({k_1_negative, k_2_negative, k_3_negative}));
        },
        n_positive, n_negative, alpha, k);

    ASSERT_EQ(k_negatives, std::vector<std::vector<int>>({{2, 0, 1}, {2, 1, 0}}));
}

TEST(iterate_over_k_negatives, WorksWithSecondAlphaFalse) {
    int n_positive = 3;
    int n_negative = 3;
    std::vector<int> k = {2, 2, 2};
    alpha_t alpha = {true, false, true};

    std::vector<std::vector<int>> k_negatives;
    ModelDistribution::iterate_over_k_negatives(
        [&k_negatives](int k_1_negative, int k_2_negative, int k_3_negative) {
            k_negatives.push_back(std::vector<int>({k_1_negative, k_2_negative, k_3_negative}));
        },
        n_positive, n_negative, alpha, k);

    ASSERT_EQ(k_negatives, std::vector<std::vector<int>>({{0, 2, 1}, {1, 2, 0}}));
}

TEST(iterate_over_k_negatives, WorksWithThirdAlphaFalse) {
    int n_positive = 3;
    int n_negative = 3;
    std::vector<int> k = {2, 2, 2};
    alpha_t alpha = {true, true, false};

    std::vector<std::vector<int>> k_negatives;
    ModelDistribution::iterate_over_k_negatives(
        [&k_negatives](int k_1_negative, int k_2_negative, int k_3_negative) {
            k_negatives.push_back(std::vector<int>({k_1_negative, k_2_negative, k_3_negative}));
        },
        n_positive, n_negative, alpha, k);

    ASSERT_EQ(k_negatives, std::vector<std::vector<int>>({{0, 1, 2}, {1, 0, 2}}));
}

TEST(iterate_over_k_negatives, WorksWithFirstAlphaTrue) {
    int n_positive = 1;
    int n_negative = 5;
    std::vector<int> k = {2, 2, 2};
    alpha_t alpha = {true, false, false};

    std::vector<std::vector<int>> k_negatives;
    ModelDistribution::iterate_over_k_negatives(
        [&k_negatives](int k_1_negative, int k_2_negative, int k_3_negative) {
            k_negatives.push_back(std::vector<int>({k_1_negative, k_2_negative, k_3_negative}));
        },
        n_positive, n_negative, alpha, k);

    ASSERT_EQ(k_negatives, std::vector<std::vector<int>>({{1, 2, 2}}));
}

TEST(iterate_over_k_negatives, WorksWithSecondAlphaTrue) {
    int n_positive = 1;
    int n_negative = 5;
    std::vector<int> k = {2, 2, 2};
    alpha_t alpha = {false, true, false};

    std::vector<std::vector<int>> k_negatives;
    ModelDistribution::iterate_over_k_negatives(
        [&k_negatives](int k_1_negative, int k_2_negative, int k_3_negative) {
            k_negatives.push_back(std::vector<int>({k_1_negative, k_2_negative, k_3_negative}));
        },
        n_positive, n_negative, alpha, k);

    ASSERT_EQ(k_negatives, std::vector<std::vector<int>>({{2, 1, 2}}));
}

TEST(iterate_over_k_negatives, WorksWithThirdAlphaTrue) {
    int n_positive = 1;
    int n_negative = 5;
    std::vector<int> k = {2, 2, 2};
    alpha_t alpha = {false, false, true};

    std::vector<std::vector<int>> k_negatives;
    ModelDistribution::iterate_over_k_negatives(
        [&k_negatives](int k_1_negative, int k_2_negative, int k_3_negative) {
            k_negatives.push_back(std::vector<int>({k_1_negative, k_2_negative, k_3_negative}));
        },
        n_positive, n_negative, alpha, k);

    ASSERT_EQ(k_negatives, std::vector<std::vector<int>>({{2, 2, 1}}));
}

TEST(iterate_over_k_negatives, WorksWithAlphaFalse) {
    int n_positive = 0;
    int n_negative = 6;
    std::vector<int> k = {2, 2, 2};
    alpha_t alpha = {false, false, false};

    std::vector<std::vector<int>> k_negatives;
    ModelDistribution::iterate_over_k_negatives(
        [&k_negatives](int k_1_negative, int k_2_negative, int k_3_negative) {
            k_negatives.push_back(std::vector<int>({k_1_negative, k_2_negative, k_3_negative}));
        },
        n_positive, n_negative, alpha, k);

    ASSERT_EQ(k_negatives, std::vector<std::vector<int>>({{2, 2, 2}}));
}

TEST(iterate_over_k_negatives, NoResults) {
    int n_positive = 1;
    int n_negative = 5;
    std::vector<int> k = {2, 2, 2};
    alpha_t alpha = {false, false, false};

    std::vector<std::vector<int>> k_negatives;
    ModelDistribution::iterate_over_k_negatives(
        [&k_negatives](int k_1_negative, int k_2_negative, int k_3_negative) {
            k_negatives.push_back(std::vector<int>({k_1_negative, k_2_negative, k_3_negative}));
        },
        n_positive, n_negative, alpha, k);

    ASSERT_EQ(k_negatives, std::vector<std::vector<int>>({}));
}

TEST(calculate_sum_over_k_negative_exact, StandardSetup) {
    int n_positive = 3;
    int n_negative = 3;
    alpha_t alpha = {true, true, true};
    delta_t delta = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 6.0};
    category_counts_t object_counts = {2, 2, 2};

    double result = ModelDistribution::calculate_sum_over_k_negative_exact(
        n_positive, n_negative, alpha, delta, object_counts);

    ASSERT_NEAR(result, 5.0 / 6.0, ERROR);
}

TEST(calculate_sum_over_k_negative_exact, NoTerms) {
    int n_positive = 3;
    int n_negative = 3;
    alpha_t alpha = {true, false, false};
    delta_t delta = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 6.0};
    category_counts_t object_counts = {2, 2, 2};

    double result = ModelDistribution::calculate_sum_over_k_negative_exact(
        n_positive, n_negative, alpha, delta, object_counts);

    ASSERT_NEAR(result, 0.0, ERROR);
}

TEST(calculate_log_sum_over_k_negative, StandardSetup) {
    int n_positive = 3;
    int n_negative = 3;
    alpha_t alpha = {true, true, true};
    delta_t delta = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 6.0};
    category_counts_t object_counts = {2, 2, 2};

    std::default_random_engine gen;
    Options options;
    options.exact = true;
    ModelDistribution model_distribution(std::vector<category_counts_t>({}), gen, options);

    double result = model_distribution.calculate_log_sum_over_k_negative(
        n_positive, n_negative, alpha, delta, object_counts);

    ASSERT_NEAR(result, std::log(5.0 / 6.0), ERROR);
}

TEST(calculate_log_sum_over_k_negative, NoTerms) {
    int n_positive = 3;
    int n_negative = 3;
    alpha_t alpha = {false, false, true};
    delta_t delta = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 6.0};
    category_counts_t object_counts = {2, 2, 2};

    std::default_random_engine gen;
    Options options;
    options.exact = true;
    ModelDistribution model_distribution(std::vector<category_counts_t>({}), gen, options);

    double result = model_distribution.calculate_log_sum_over_k_negative(
        n_positive, n_negative, alpha, delta, object_counts);

    ASSERT_EQ(result, -INFINITY);
}

TEST(calculate_log_p_k_positive_given_alpha_n_positive, Tests) {
    ASSERT_EQ(ModelDistribution::calculate_log_p_k_positive_given_alpha_n_positive(
                  5, {false, true, false}),
              0.0);
    ASSERT_NEAR(ModelDistribution::calculate_log_p_k_positive_given_alpha_n_positive(
                    5, {true, true, false}),
                std::log(1.0 / 6.0), ERROR);
    ASSERT_NEAR(
        ModelDistribution::calculate_log_p_k_positive_given_alpha_n_positive(5, {true, true, true}),
        std::log(1.0 / 21.0), ERROR);
}

TEST(distribution, AllNonzero) {
    std::vector<category_counts_t> data = {{2, 2, 2}};
    std::default_random_engine generator;
    Options options;
    options.exact = true;
    ModelDistribution model_distribution(data, generator, options);

    alpha_t alpha_true = {true, true, true};
    std::vector<alpha_t> alphas = {alpha_true};
    double epsilon = 0.5;
    delta_t delta = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 6.0};
    double result = model_distribution.distribution(alphas, epsilon, delta).at(0).at(0);
    ASSERT_NEAR(std::exp(result), 1669.0 / 86016.0, ERROR);
}

TEST(distribution, SmallAlphaCorrect) {
    std::vector<category_counts_t> data = {{0, 0, 5}};
    std::default_random_engine generator;
    Options options;
    options.exact = true;
    ModelDistribution model_distribution(data, generator, options);

    alpha_t alpha = {false, false, true};
    std::vector<alpha_t> alphas = {alpha};
    double epsilon = 0.5;
    delta_t delta = {1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0};
    double result = model_distribution.distribution(alphas, epsilon, delta).at(0).at(0);
    ASSERT_NEAR(std::exp(result), 32.0 / 243.0, ERROR);
}

TEST(distribution, SmallAlphaWrong) {
    std::vector<category_counts_t> data = {{0, 0, 5}};
    std::default_random_engine generator;
    Options options;
    options.exact = true;
    ModelDistribution model_distribution(data, generator, options);

    alpha_t alpha = {true, true, true};
    std::vector<alpha_t> alphas = {alpha};
    double epsilon = 0.5;
    delta_t delta = {1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0};
    double result = model_distribution.distribution(alphas, epsilon, delta).at(0).at(0);
    ASSERT_NEAR(std::exp(result), 101.0 / 9072.0, ERROR);
}

TEST(distribution, BigAlphaCorrect) {
    std::vector<category_counts_t> data = {{0, 0, 10}};
    std::default_random_engine generator;
    Options options;
    options.exact = true;
    ModelDistribution model_distribution(data, generator, options);

    alpha_t alpha = {false, false, true};
    std::vector<alpha_t> alphas = {alpha};
    double epsilon = 0.5;
    delta_t delta = {1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0};
    double result = model_distribution.distribution(alphas, epsilon, delta).at(0).at(0);
    ASSERT_NEAR(std::exp(result), 1024.0 / 59049.0, ERROR);
}

TEST(distribution, BigAlphaWrong) {
    std::vector<category_counts_t> data = {{0, 0, 10}};
    std::default_random_engine generator;
    Options options;
    options.exact = true;
    ModelDistribution model_distribution(data, generator, options);

    alpha_t alpha = {true, true, true};
    std::vector<alpha_t> alphas = {alpha};
    double epsilon = 0.5;
    delta_t delta = {1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0};
    double result = model_distribution.distribution(alphas, epsilon, delta).at(0).at(0);
    ASSERT_NEAR(std::exp(result), 621377.0 / 1330255872.0, ERROR);
}

}  // namespace FilterModel
