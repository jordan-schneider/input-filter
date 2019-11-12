#include "model_distribution.hpp"

#include "monte_carlo_integration.hpp"
#include "multinomial.hpp"
#include "multivariate_guassian.hpp"
#include "mvi3/mvi3.hpp"
#include "types.hpp"
#include "utils.hpp"

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <numeric>
#include <vector>

namespace FilterModel {
ModelDistribution::ModelDistribution(const std::vector<category_counts_t> &data,
                                     std::default_random_engine &generator, const Options &options)
    : data(data), generator(generator), options(options){};

std::vector<std::vector<double>> ModelDistribution::distribution(
    const std::vector<std::vector<alpha_t>> &alphas_per_object, double epsilon,
    const delta_t &delta) const {
    // See function definition for description.
    std::vector<std::vector<double>> log_alpha_likelyhoods_per_object(data.size(),
                                                                      std::vector<double>());
    for (int obs_index = 0; obs_index < data.size(); ++obs_index) {
        const category_counts_t &object_counts = data.at(obs_index);
        const std::vector<alpha_t> &alphas = alphas_per_object.at(obs_index);

        int n = std::accumulate(object_counts.begin(), object_counts.end(), 0);
        std::vector<double> p = {1 - epsilon, epsilon};
        Multinomial n_positive_distribution(n, p);

        for (int alpha_index = 0; alpha_index < alphas.size(); ++alpha_index) {
            const alpha_t &alpha = alphas.at(alpha_index);

            // n^+ goes from 0 to the sum of k_i for which alpha_i is true
            // When alpha_i is false, then all of the k_i corresponding must be noise reads.
            const category_counts_t filtered_object_counts = filter_by_alpha(alpha, object_counts);
            const int max_n_positive =
                std::accumulate(filtered_object_counts.begin(), filtered_object_counts.end(), 0);

            std::vector<double> log_p_k_n_positive_given_all_terms;
            // n_positive is the number of observations not made by the error process.
            for (int n_positive = 0; n_positive <= max_n_positive; ++n_positive) {
                // n_negative is the number of observations made by the error process.
                int n_negative = n - n_positive;
                double log_p_n_plus_given_n_epsilon =
                    n_positive_distribution.log_pdf(std::vector<int>({n_positive, n_negative}));

                double log_p_k_positive_given_alpha_n_positive =
                    calculate_log_p_k_positive_given_alpha_n_positive(n_positive, alpha);

                double log_sum_over_k_negative = calculate_log_sum_over_k_negative(
                    n_positive, n_negative, alpha, delta, object_counts);

                double log_p_k_n_positive_given_all = log_p_n_plus_given_n_epsilon +
                                                      log_p_k_positive_given_alpha_n_positive +
                                                      log_sum_over_k_negative;

                log_p_k_n_positive_given_all_terms.push_back(log_p_k_n_positive_given_all);
            }

            std::vector<double> p_k_n_positive_given_all =
                exp<double>(log_p_k_n_positive_given_all_terms);

            log_alpha_likelyhoods_per_object.at(obs_index).push_back(
                std::log(stable_sum<double>(p_k_n_positive_given_all)));
        }
    }
    return log_alpha_likelyhoods_per_object;
}

std::vector<std::vector<double>> ModelDistribution::distribution(const std::vector<alpha_t> &alphas,
                                                                 double epsilon,
                                                                 const delta_t &delta) const {
    std::vector<std::vector<alpha_t>> alphas_per_obs(data.size(), alphas);
    return distribution(alphas_per_obs, epsilon, delta);
}

double ModelDistribution::calculate_log_p_k_positive_given_alpha_n_positive(int n_positive,
                                                                            const alpha_t &alpha) {
    int sum_alpha = accumulate(alpha.begin(), alpha.end(), 0);

    // 1/(n^+ + sum alpha_i - 1 choose n^+)
    // lgamma(x + 1) = log(x!)
    double log_p_k_given_alpha_n_positive =
        lgamma(n_positive + 1) + lgamma(sum_alpha) - lgamma(n_positive + sum_alpha);
    return log_p_k_given_alpha_n_positive;
}

double ModelDistribution::calculate_log_sum_over_k_negative(
    int n_positive, int n_negative, const alpha_t &alpha, const delta_t &delta,
    const category_counts_t &object_counts) const {
    // TODO(joschnie): Check for viability and return -1 if unviable.

    int n = n_positive + n_negative;

    delta_t delta_for_alpha_true = filter_by_alpha(alpha, delta);

    double sum_over_k_negative;
    if (options.comparison) {
        sum_over_k_negative = calculate_sum_over_k_negative_exact(n_positive, n_negative, alpha,
                                                                  delta, object_counts);
        double sum_over_k_negative_approx = calculate_sum_over_k_negative_approx(
            n_positive, n_negative, alpha, delta, object_counts);
        double error = sum_over_k_negative - sum_over_k_negative_approx;

        BOOST_LOG_TRIVIAL(info) << "Approximation error: " << std::to_string(error);
    } else {
        if (!can_use_normal_approx(n_negative, delta_for_alpha_true)) {
            sum_over_k_negative = calculate_sum_over_k_negative_exact(n_positive, n_negative, alpha,
                                                                      delta, object_counts);
        } else {
            sum_over_k_negative = calculate_sum_over_k_negative_approx(n_positive, n_negative,
                                                                       alpha, delta, object_counts);
        }
    }

    return std::log(sum_over_k_negative);
}

double ModelDistribution::calculate_sum_over_k_negative_exact(
    int n_positive, int n_negative, const alpha_t &alpha, const delta_t &delta,
    const category_counts_t &object_counts) {
    Multinomial k_negative_distribution(n_negative, delta);

    std::vector<double> log_p_k_positive_given_delta_n_negative;

    iterate_over_k_negatives(
        [&log_p_k_positive_given_delta_n_negative, &k_negative_distribution](
            int k_1_negative, int k_2_negative, int k_3_negative) {
            log_p_k_positive_given_delta_n_negative.push_back(
                k_negative_distribution.log_pdf({k_1_negative, k_2_negative, k_3_negative}));
        },
        n_positive, n_negative, alpha, object_counts);

    std::vector<double> p_k_positive_given_delta_n_negative =
        exp(log_p_k_positive_given_delta_n_negative);
    return stable_sum<double>(p_k_positive_given_delta_n_negative);
}

void ModelDistribution::iterate_over_k_negatives(std::function<void(int, int, int)> f,
                                                 int n_positive, int n_negative,
                                                 const alpha_t &alpha,
                                                 const category_counts_t &object_counts) {
    // Because some alpha_i might be 0, we have to fix some of the k^- values to start with.
    std::vector<int> fixed_k_negative = {0, 0, 0};
    for (int i = 0; i < 3; ++i) {
        if (alpha.at(i) == false) {
            fixed_k_negative.at(i) = object_counts.at(i);
        }
    }

    int k_1_negative_start = std::max({object_counts[0] - n_positive, fixed_k_negative[0],
                                       n_negative - object_counts[1] - object_counts[2]});
    int k_1_negative_end =
        std::min(n_negative - fixed_k_negative[1] - fixed_k_negative[2], object_counts[0]);
    for (int k_1_negative = k_1_negative_start; k_1_negative <= k_1_negative_end; ++k_1_negative) {
        int k_2_negative_start = std::max({object_counts[1] - n_positive, fixed_k_negative[1],
                                           n_negative - k_1_negative - object_counts[2]});
        int k_2_negative_end =
            std::min(n_negative - k_1_negative - fixed_k_negative[2], object_counts[1]);
        for (int k_2_negative = k_2_negative_start; k_2_negative <= k_2_negative_end;
             ++k_2_negative) {
            int k_3_negative = n_negative - k_1_negative - k_2_negative;
            if (alpha[2] == false && k_3_negative == object_counts[2] ||
                alpha[2] == true && k_3_negative >= 0 && k_3_negative <= object_counts[2]) {
                f(k_1_negative, k_2_negative, k_3_negative);
            }
        }
    }
}

double ModelDistribution::calculate_sum_over_k_negative_approx(
    int n_positive, int n_negative, const alpha_t &alpha, const delta_t &delta,
    const category_counts_t &object_counts) {
    std::vector<bool> object_count_nonzero(object_counts.size(), false);
    std::transform(object_counts.begin(), object_counts.end(), object_count_nonzero.begin(),
                   [](int count) { return count > 0; });

    std::vector<bool> effective_alpha;
    for (int i = 0; i < alpha.size(); ++i) {
        effective_alpha.push_back(alpha.at(i) && object_count_nonzero.at(i));
    }

    category_counts_t filtered_object_counts = filter_by_alpha(effective_alpha, object_counts);

    if (effective_alpha == std::vector<bool>({0, 0, 0})) {
        return 0;
    }

    Multinomial m_fixed =
        Multinomial(n_negative, delta)
            .fix_dimensions(not_v(effective_alpha),
                            filter_by_alpha(effective_alpha, object_counts, false));

    double sum_over_k_negative_approx;
    if (m_fixed.p.size() > 1) {
        MultivariateGuassian mg = MultivariateGuassian::from_multinomial(m_fixed);
        std::vector<std::vector<double>> hyperplanes =
            ModelDistribution::get_hyperplanes(m_fixed.n, filtered_object_counts);
        mg.shift_hyperplanes(hyperplanes);

        MVI3::Mvi3 mvi3;
        double log_sum_over_k_negative_approx =
            std::log(mvi3.integrate(12456, -1, 10, 10, mg.get_covariance(), hyperplanes)) +
            m_fixed.log_adjust;

        sum_over_k_negative_approx = std::exp(log_sum_over_k_negative_approx);
    } else {
        if (filtered_object_counts.size() > 0 && filtered_object_counts.back() >= m_fixed.n) {
            sum_over_k_negative_approx = std::exp(m_fixed.log_adjust);
            // TODO(joschnei): condition above is wrong. Need to check both n+ and n-b
        } else {
            sum_over_k_negative_approx = 0;
        }
    }
    return sum_over_k_negative_approx;
}

double ModelDistribution::calculate_sum_over_k_negative_approx_2(
    int n_positive, int n_negative, const alpha_t &alpha, const delta_t &delta,
    const category_counts_t &object_counts) {
    Multinomial m_fixed =
        Multinomial(n_negative, delta)
            .fix_dimensions(not_v(alpha), filter_by_alpha(alpha, object_counts, false));
    MultivariateGuassian mg = MultivariateGuassian::from_multinomial(m_fixed);

    std::function<double(std::vector<double>)> multivariate_guassian_density =
        [mg](std::vector<double> point) { return mg.density(point); };

    std::function<std::vector<double>(std::default_random_engine &)> sampler =
        [m_fixed, object_counts = filter_by_alpha(alpha, object_counts)](
            std::default_random_engine &generator) {
            int dim = m_fixed.p.size();

            std::vector<std::uniform_real_distribution<double>> dists;
            for (int i = 0; i < dim - 1; ++i) {
                dists.push_back(std::uniform_real_distribution<double>(0.0, object_counts.at(i)));
            }

            std::vector<double> point;
            double last_component;
            do {
                point = std::vector<double>();
                for (std::uniform_real_distribution<double> dist : dists) {
                    point.push_back(dist(generator));
                }
                last_component = m_fixed.n - std::accumulate(point.begin(), point.end(), 0.0);
            } while (!(0 <= last_component && last_component <= object_counts.back()));
            return point;
        };

    double volume;
    if (m_fixed.p.size() == 3) {
        volume = n_negative * object_counts.at(2) - std::pow(object_counts.at(2), 2) / 2.0;
        if (n_negative > object_counts.at(0)) {
            volume -= 1.0 / 2.0 * std::pow(n_negative - object_counts.at(0), 2);
            if (n_negative - object_counts.at(2) > object_counts.at(0)) {
                volume +=
                    1.0 / 2.0 * std::pow(n_negative - object_counts.at(2) - object_counts.at(0), 2);
            }
        }
        if (n_negative > object_counts.at(1)) {
            volume -= 1.0 / 2.0 * std::pow(n_negative - object_counts.at(1), 2);
            if (n_negative - object_counts.at(2) > object_counts.at(1)) {
                volume +=
                    1.0 / 2.0 * std::pow(n_negative - object_counts.at(2) - object_counts.at(1), 2);
            }
        }
        if (n_negative > object_counts.at(0) + object_counts.at(1)) {
            volume +=
                1.0 / 2.0 * std::pow(n_negative - object_counts.at(0) - object_counts.at(1), 2);
        }
        if (n_negative - object_counts.at(2) < 0) {
            volume += 1.0 / 2.0 * std::pow(n_negative - object_counts.at(2), 2.0);
        }
    } else if (m_fixed.p.size() == 2) {
        volume = std::min(n_negative, object_counts.at(0)) -
                 std::max(0, n_negative - object_counts.at(1));
    }

    return std::exp(
        std::log(integrate(multivariate_guassian_density, sampler, generator, volume, 500000)) +
        m_fixed.log_adjust);
}

/**
 * Assumes n_negative has already been adjusted, objct_counts has already been filtered.
 */
std::vector<std::vector<double>> ModelDistribution::get_hyperplanes(
    int n_negative, category_counts_t object_counts) {
    std::vector<std::vector<double>> hyperplanes;

    // Of the the remaining dimensions, pick the last one to equal n - sum k_i.
    category_count_t k_fixed = object_counts.back();
    object_counts.pop_back();

    // sum k_i^- = n^- => k_1^- = n^- - sum k_i^- (from 2)
    // k_1^- >= 0 => sum k_i^- (from 2) <= n^-
    std::vector<double> k_fixed_negative_lower_constraint(object_counts.size(), 1);
    k_fixed_negative_lower_constraint.push_back(n_negative);
    hyperplanes.push_back(k_fixed_negative_lower_constraint);

    // sum k_i^- = n^+- => k_1^- = n^- - sum k_i^- (from 2)
    // k_1^- <= k_1 => - sum k_i^- (from 2) <= k_1 - n^-
    std::vector<double> k_fixed_negative_upper_constraint(object_counts.size(), -1);
    k_fixed_negative_upper_constraint.push_back(k_fixed - n_negative);
    hyperplanes.push_back(k_fixed_negative_upper_constraint);

    // k_i^+ and k_i^- are both >= 0.
    // k_i^- >= 0 implies -k_i^- <= 0.
    // k_i+ >= 0 implies k_i - k_i^- >= 0 implies k_i^- <= k_i
    for (int i = 0; i < object_counts.size(); ++i) {
        std::vector<double> k_i_minus_constraint(object_counts.size(), 0);
        k_i_minus_constraint.at(i) = -1;
        k_i_minus_constraint.push_back(0);
        hyperplanes.push_back(k_i_minus_constraint);

        std::vector<double> k_i_plus_constraint(object_counts.size(), 0);
        k_i_plus_constraint.at(i) = 1;
        k_i_plus_constraint.push_back(object_counts.at(i));
        hyperplanes.push_back(k_i_plus_constraint);
    }

    return hyperplanes;
}

bool ModelDistribution::can_use_normal_approx(int n_negative, const delta_t &delta) const {
    if (options.exact) {
        return false;
    }
    for (double p : delta) {
        if (n_negative * p <= 5.0) {
            return false;
        }
    }
    return true;
}
}  // namespace FilterModel