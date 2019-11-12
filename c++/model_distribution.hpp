#ifndef MODEL_DISTRIBUTION_HPP
#define MODEL_DISTRIBUTION_HPP

#include "gtest/gtest_prod.h"
#include "types.hpp"

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <numeric>
#include <random>
#include <unordered_map>
#include <utility>

namespace FilterModel {

/**
 * Represents the probability distribution encoded by the latent variable model adapted from in
 * Perkins et al.
 */
class ModelDistribution {
   public:
    /**
     * Constructs a new ModelDistribution object from a vector of category counts.
     *
     * Arguments:
     *  data - vector of category counts
     *  generator - random number generator
     *  comparison - test parameter, should usually ignore. If true, runs both exact and approximate
     *    methods for evaluating the probability, compares them, and stops the program if the
     *    estimates are too far apart.
     */
    ModelDistribution(const std::vector<category_counts_t> &data,
                      std::default_random_engine &generator, const Options &options);

    /**
     * Calculates log(p(k | alpha, epsilon, delta)) for every object in the data for every value
     * of alpha in alphas_per_object for that object. Returns a vector over objects of vectors
     * over log conditional probabilities per value of alpha.
     *
     * For example, if alphas_per_object = {{{1,0}}, {0,1}}, {{1,0}},
     * {{1,0}, {0,1}, {1, 1}} then the output might be {{-2, -8}, {-7}, {-1.2,
     * -2.1, -5.7}} where {-2, -8} are the log conditional probabilities for alpha={1,0} and
     * alpha={0,1} for the first object.
     */
    std::vector<std::vector<double>> distribution(
        const std::vector<std::vector<alpha_t>> &alphas_per_object, double epsilon,
        const delta_t &delta) const;

    /**
     * Calculates log(p(k | alpha, epsilon, delta)) for every object in the data for the given
     * alpha. Returns a vector over objects of vectors over log conditional probabilities per value
     * of alpha.
     *
     * For example, if alphas = {{0,1,0}, {1,1,1}} then the output might be {{-2, -7}, {-4, -5}}
     * where log(p(k | alpha={0,1,0}, ...)) = -2 for the first object,
     * log(p(k | alpha={0,1,0}, ...)) = -4 for the second obect, log(p(k | alpha={1,1,1}, ...)) = -7
     * for the first object, and so on.
     */
    std::vector<std::vector<double>> distribution(const std::vector<alpha_t> &alphas,
                                                  double epsilon, const delta_t &delta) const;

    // The following are public only so I can test them easier. FRIEND_TEST exists, but it doesn't
    // work right for static methods.

    static double calculate_log_p_k_positive_given_alpha_n_positive(int n_positive,
                                                                    const alpha_t &alpha);
    // This is non-static only so it has access to the comparison testing parameter.
    double calculate_log_sum_over_k_negative(int n_positive, int n_negative, const alpha_t &alpha,
                                             const delta_t &delta,
                                             const category_counts_t &object_counts) const;
    static double calculate_sum_over_k_negative_exact(int n_positive, int n_negative,
                                                      const alpha_t &alpha, const delta_t &delta,
                                                      const category_counts_t &object_counts);

    // Uses MVI3 integration
    static double calculate_sum_over_k_negative_approx(int n_positive, int n_negative,
                                                       const alpha_t &alpha, const delta_t &delta,
                                                       const category_counts_t &object_counts);
    /**
     * Given n^+, n^-, alpha, and k, loop over all possible values for k^-, and call f() on each.
     *
     * This is hard-coded for |alpha|=3. Doing this in a generic way was incredibly slow.
     */
    static void iterate_over_k_negatives(std::function<void(int, int, int)> f, int n_positive,
                                         int n_negative, const alpha_t &alpha,
                                         const category_counts_t &object_counts);

    /**
     * Removes components of v for which alpha_i has the wrong value.
     */
    template <class T>
    static std::vector<T> filter_by_alpha(const alpha_t &alpha, const std::vector<T> &v,
                                          const bool value = true) {
        assert(alpha.size() == v.size());

        std::vector<T> filtered;
        for (int i = 0; i < alpha.size(); ++i) {
            if (alpha.at(i) == value) {
                filtered.push_back(v.at(i));
            }
        }
        return filtered;
    }

   private:
    const std::vector<category_counts_t> &data;
    const Options options;
    std::default_random_engine &generator;

    /**
     * Test for if using a normal approximation (and mvi3 integration) is permissible.
     *
     * There is no real standard for when the multinomial to multivariate guassian approximation
     * is valid, so we adapt the standard binomial to guassian rule, which is that there is a
     * mean of at least 5 elements in every dimension.
     *
     * TODO(joschnei): Verify experimentally that this is a good heuristic.
     */
    bool can_use_normal_approx(int n_negative, const delta_t &delta) const;

    // The below are currently not in use beacuse of randomness problems.

    // Uses naive Monte-Carlo integration
    double calculate_sum_over_k_negative_approx_2(int n_positive, int n_negative,
                                                  const alpha_t &alpha, const delta_t &delta,
                                                  const category_counts_t &object_counts);
    /**
     * Creates a hyperplane array representing the constraints on k_minus for use in mvi3
     * integration.
     *
     * Each of the four constraints on k_negative can be represented as a set of linear
     * inequalities in the values of k_negative. This function creates those linear equalities
     * so that they can be passed as constraints to mvi3, a method for integrating a multinomial
     * pdf over a convex set. See function defintion for details of the exact constraints.
     */
    static std::vector<std::vector<double>> get_hyperplanes(int n_negative,
                                                            category_counts_t object_counts);
};
}  // namespace FilterModel

#endif