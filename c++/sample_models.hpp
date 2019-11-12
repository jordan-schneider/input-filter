#ifndef SAMPLE_MODELS_HPP
#define SAMPLE_MODELS_HPP

#include "gtest/gtest_prod.h"
#include "model_distribution.hpp"
#include "types.hpp"

#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <utility>
#include <vector>

namespace FilterModel {
/**
 * MH Sampler for object alphas using the latent variable model in Perkins et. al.
 */
class ModelSampler {
   public:
    /**
     * Constructs a ModelSampler from the category counts.
     */
    ModelSampler(const std::vector<category_counts_t> &data, std::default_random_engine &generator,
                 const Options &options);

    /**
     * Samples an alpha for each object in the data given the epsilon and delta parameters.
     */
    std::vector<alpha_t> sample(double epsilon, const delta_t &delta);

   private:
    FRIEND_TEST(generate_all_alphas, Zero);
    FRIEND_TEST(generate_all_alphas, One);
    FRIEND_TEST(generate_all_alphas, Three);

    ModelDistribution model_distribution;
    const size_t n_objects;
    const size_t n_categories;
    const std::vector<alpha_t> alphas;
    std::default_random_engine &generator;

    /**
     * Generates all possible alpha vectors of length n_categories.
     */
    static std::vector<alpha_t> generate_alphas(int n_categories, const Options &options);
    static std::vector<alpha_t> generate_all_alphas(int n_categories);
    static std::vector<alpha_t> generate_real_alphas();
};
}  // namespace FilterModel

#endif