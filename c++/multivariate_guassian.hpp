#ifndef MULTIVARIATE_GAUSSIAN_HPP
#define MULTIVARIATE_GAUSSIAN_HPP

#include "gtest/gtest_prod.h"
#include "multinomial.hpp"
#include "types.hpp"

#include <Eigen/Dense>
#include <boost/log/trivial.hpp>
#include <vector>

namespace FilterModel {

/**
 * Represents a Multivariate Guassian distribution, especially when approximated from a multinomial.
 */
class MultivariateGuassian {
   public:
    MultivariateGuassian(const std::vector<double> mean,
                         const std::vector<std::vector<double>> covariance);

    /**
     * Constructs a MultivariateGuassian as an approximation of a multinomial distribution.
     */
    static MultivariateGuassian from_multinomial(int n, const std::vector<double>& p);
    static MultivariateGuassian from_multinomial(const Multinomial& m);
    /**
     * Returns the triangular covariance matrix of the guassian as a vector of vectors.
     */
    std::vector<std::vector<double>> get_covariance() const;
    /**
     * Returns the mean vector of the guassian.
     */
    std::vector<double> get_mean() const;

    void shift_hyperplanes(std::vector<std::vector<double>>& hyperplanes);
    double density(std::vector<double> point) const;

   private:
    Eigen::VectorXd mean;
    Eigen::MatrixXd covariance;
    Eigen::MatrixXd covariance_inverse;
};
}  // namespace FilterModel

#endif