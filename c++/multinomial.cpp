#include "multinomial.hpp"

#include "utils.hpp"

#include <boost/log/trivial.hpp>
#include <cmath>
#include <numeric>

namespace FilterModel {

Multinomial::Multinomial(int n, std::vector<double> p, double log_adjust)
    : n(n), p(p), log_adjust(log_adjust) {
    assert(n >= 0);
    assert_probability(p);
};

Multinomial Multinomial::fix_dimensions(std::vector<bool> is_dimension_fixed,
                                        std::vector<int> fixed_counts) const {
    std::vector<double> new_p;
    for (int dimension = 0; dimension < is_dimension_fixed.size(); ++dimension) {
        if (!is_dimension_fixed.at(dimension)) {
            new_p.push_back(p.at(dimension));
            fixed_counts.insert(fixed_counts.begin() + dimension, 0);
        }
    }
    double norm_p = std::accumulate(new_p.begin(), new_p.end(), 0.0);
    std::transform(new_p.begin(), new_p.end(), new_p.begin(),
                   [norm_p](double x) { return x / norm_p; });
    assert_probability(new_p);

    int new_n = n - std::accumulate(fixed_counts.begin(), fixed_counts.end(), 0);

    double log_adjust = std::lgamma(n + 1) - std::lgamma(new_n + 1);

    // Adjust fo the alpha=false terms.
    for (int index : bool_to_index<int>(is_dimension_fixed)) {
        log_adjust += fixed_counts.at(index) * std::log(p.at(index));
        log_adjust -= std::lgamma(fixed_counts.at(index) + 1);
    }
    log_adjust += new_n * std::log(norm_p);

    return Multinomial(new_n, new_p, log_adjust);
}

double Multinomial::log_pdf(const std::vector<int> &k) const {
    assert_nonnegative(k);
    int sum_k = std::accumulate(k.begin(), k.end(), 0);

    assert(sum_k == n);
    assert(k.size() == p.size());

    if (n == 0) {
        return std::log(1);
    }

    double log_n_choose_k = std::lgamma(n + 1);
    double log_prod_p_k = 0.0;
    for (int i = 0; i < k.size(); ++i) {
        log_n_choose_k -= std::lgamma(k.at(i) + 1);
        log_prod_p_k += k.at(i) * std::log(p.at(i));
    }
    return log_n_choose_k + log_prod_p_k + log_adjust;
}

}  // namespace FilterModel
