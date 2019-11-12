#include "sample_models.hpp"

#include "model_distribution.hpp"
#include "types.hpp"
#include "utils.hpp"

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

namespace FilterModel {

ModelSampler::ModelSampler(const std::vector<category_counts_t> &data,
                           std::default_random_engine &generator, const Options &options)
    : model_distribution(data, generator, options),
      generator(generator),
      alphas(ModelSampler::generate_alphas(data.at(0).size(), options)),
      n_objects(data.size()),
      n_categories(data.at(0).size()){};

std::vector<alpha_t> ModelSampler::sample(double epsilon, const delta_t &delta) {
    std::vector<double> priors(alphas.size(), 1.0 / alphas.size());

    std::vector<std::vector<double>> log_alpha_likelyhoods =
        model_distribution.distribution(alphas, epsilon, delta);

    std::vector<alpha_t> models;
    for (int object_index = 0; object_index < n_objects; ++object_index) {
        std::vector<double> alpha_weights(alphas.size(), 0.0);
        double total_weight = 0.0;
        for (int i = 0; i < alphas.size(); ++i) {
            alpha_weights.at(i) = log_alpha_likelyhoods.at(object_index).at(i);
            // check that log_alpha_likelyhoods is sane.
            // shouldn't be uniform.
            total_weight += std::exp(alpha_weights.at(i));
        }

        for (int i = 0; i < alphas.size(); ++i) {
            alpha_weights.at(i) -= std::log(total_weight);
        }

        std::vector<double> probabilities = exp(alpha_weights);

        assert_probability(probabilities);
        // look at probabilities

        // Select an alpha at random for the current object.
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        double random = distribution(generator);
        double probability_sum = 0.0;
        int i = 0;
        while (probability_sum < random) {
            probability_sum += probabilities.at(i);
            ++i;
        }
        models.push_back(alphas.at(i - 1));
    }
    return models;
};

std::vector<alpha_t> ModelSampler::generate_alphas(int n_categories, const Options &options) {
    if (options.use_smaller_alphas) {
        return generate_real_alphas();
    } else {
        return generate_all_alphas(n_categories);
    }
}

std::vector<alpha_t> ModelSampler::generate_all_alphas(int n_categories) {
    alpha_t current = std::vector<category_t>(n_categories, 0);
    std::vector<alpha_t> alphas;
    alpha_t end = std::vector<category_t>(n_categories, 1);
    while (current != end) {
        current = alpha_t(current);
        int i = 0;
        while (current.at(i) == 1) {
            current.at(i) = 0;
            ++i;
        }
        current.at(i) = 1;
        alphas.push_back(current);
    }
    return alphas;
}

std::vector<alpha_t> ModelSampler::generate_real_alphas() {
    return {{0, 0, 1}, {0, 1, 0}, {1, 1, 1}, {1, 0, 1}};
}
}  // namespace FilterModel