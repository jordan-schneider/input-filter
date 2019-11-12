#include "../utils.hpp"
#include "gtest/gtest.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <vector>

namespace FilterModel {

TEST(uniform_real_distribution, ApproximatelyUniform) {
    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(0, 1);

    std::vector<int> quantile_counts(10);
    int total = 1000;
    for (int i = 0; i < total; i++) {
        double val = dist(generator);
        quantile_counts.at(int(val * 10)) += 1;
    }

    for (int count : quantile_counts) {
        EXPECT_LE(count, total / 10 + 50);
        EXPECT_GE(count, total / 10 - 50);
    }
}

TEST(sample_in_range, Correct) {
    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(0, 1);

    std::vector<int> quantile_counts(10);
    int total = 1000;
    for (int i = 0; i < total; i++) {
        double val = sample_in_range(dist, generator, 0.2, 0.8);
        quantile_counts.at(int(val * 10)) += 1;
    }

    for (int i = 2; i < 8; i++) {
        EXPECT_LE(quantile_counts[i], total / 7 + 50);
        EXPECT_GE(quantile_counts[i], total / 7 - 60);
    }

    EXPECT_EQ(quantile_counts[0], 0);
    EXPECT_EQ(quantile_counts[8], 0);
    EXPECT_EQ(quantile_counts[9], 0);
}

TEST(sample_symmetric_simplex, Correct) {
    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(0, 1);

    int n = 1000;
    for (int i = 0; i < n; i++) {
        std::vector<double> p = sample_symmetric_simplex(dist, generator, 3);
        ASSERT_EQ(p.size(), 3);
        ASSERT_NEAR(p[0] + p[1] + p[2], 1, 0.001);
    }
}

}  // namespace FilterModel
