#ifndef MULTINOMIAL_HPP
#define MULTINOMIAL_HPP

/**
 * Contains static methods for computing pdfs of multinomial and binomial distributions in log
 * space.
 */

#include <vector>

namespace FilterModel {
class Multinomial {
   public:
    Multinomial(int n, std::vector<double> p, double log_adjust = 0);
    /**
     * Calculates the probability density of success count vector k out of n trials in a multinomial
     * distribution with and success probability vector p. sum(k) should equal n, sum(p) should
     * equal 1, and k.size should equal p.size.
     */
    double log_pdf(const std::vector<int> &k) const;

    Multinomial fix_dimensions(std::vector<bool> is_dimension_fixed,
                               std::vector<int> fixed_counts) const;
    const int n;
    const std::vector<double> p;
    const double log_adjust = 0;
};
}  // namespace FilterModel

#endif