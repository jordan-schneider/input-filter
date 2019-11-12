#ifndef METROPOLIS_HASTINGS_HPP
#define METROPOLIS_HASTINGS_HPP

/**
 * Preforms generic metropolis hasting sampling.
 */

#include "utils.hpp"

#include <functional>
#include <random>
#include <vector>

namespace FilterModel {
class MetropolisHastingsSampler {
   public:
    /**
     * Does Metropolis hastings sampling over a variable that takes values in [0, 1].
     *
     * Template arguments:
     *   T - type of element in the domain being sampled.
     *   generator - random number generator class.
     *
     * Arguments:
     *  log_pdf - Takes in an element of type T and returns the log of the pdf of that distribution.
     *  uniform_sampler - Accepts a random number generator and then samples an element of type T at
     *     uniform.
     *  conditional_sampler - Accepts a current element of type T and a random number generator and
     *     returns a new element of type T given the previous value.
     *  gen - A random number generator.
     *  *values - a pointer to a vector. If not null, all of the sampled values are stored here.
     */
    template <class T, class generator>
    static T sample(int iterations, const std::function<double(T value)> log_pdf,
                    const std::function<T(generator &gen)> uniform_sampler,
                    const std::function<T(T center, generator &gen)> conditional_sampler,
                    generator &gen, std::vector<T> *values = nullptr) {
        std::uniform_real_distribution<> prior_distribution(0.0, 1.0);
        T value = uniform_sampler(gen);
        if (values != nullptr) {
            values->push_back(value);
        }

        double p_value = log_pdf(value);

        for (int i = 0; i < iterations; ++i) {
            T candidate_value = conditional_sampler(value, gen);
            double p_candidate_value = log_pdf(candidate_value);
            if (p_candidate_value != -INFINITY) {
                double p_accept = std::min(0.0, p_candidate_value - p_value);
                if (prior_distribution(gen) < std::exp(p_accept)) {
                    value = candidate_value;
                    p_value = p_candidate_value;
                }
            }

            if (values != nullptr) {
                values->push_back(value);
            }
        }

        return value;
    }
};
}  // namespace FilterModel
#endif