#include "../sample_models.hpp"
#include "gtest/gtest.h"

namespace FilterModel {
TEST(generate_all_alphas, Zero) {
    int n_categores = 0;
    std::vector<alpha_t> alphas = ModelSampler::generate_all_alphas(n_categores);
    std::vector<alpha_t> expected_alphas = {};
    ASSERT_EQ(alphas, expected_alphas);
}

TEST(generate_all_alphas, One) {
    int n_categores = 1;
    std::vector<alpha_t> alphas = ModelSampler::generate_all_alphas(n_categores);
    std::vector<alpha_t> expected_alphas = {{1}};
    ASSERT_EQ(alphas, expected_alphas);
}

TEST(generate_all_alphas, Three) {
    int n_categores = 3;
    std::vector<alpha_t> alphas = ModelSampler::generate_all_alphas(n_categores);
    std::vector<alpha_t> expected_alphas = {{1, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, 1},
                                            {1, 0, 1}, {0, 1, 1}, {1, 1, 1}};
    ASSERT_EQ(alphas, expected_alphas);
}
}  // namespace FilterModel