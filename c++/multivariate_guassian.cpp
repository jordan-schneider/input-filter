#include "multivariate_guassian.hpp"

#include "multinomial.hpp"
#include "utils.hpp"

#include <Eigen/Dense>
#include <vector>

namespace FilterModel {

MultivariateGuassian::MultivariateGuassian(const std::vector<double> mean_in,
                                           const std::vector<std::vector<double>> covariance_in)
    : mean(mean_in.size()), covariance(covariance_in.size(), covariance_in.size()) {
    for (int i = 0; i < mean_in.size(); ++i) {
        mean[i] = mean_in.at(i);
    }

    for (int i = 0; i < covariance_in.size(); ++i) {
        const std::vector<double>& row = covariance_in.at(i);
        for (int j = 0; j < row.size(); ++j) {
            covariance(i, j) = row.at(j);
            covariance(j, i) = row.at(j);
        }
    }

    covariance_inverse = covariance.inverse();
};

MultivariateGuassian MultivariateGuassian::from_multinomial(int n, const std::vector<double>& p) {
    std::vector<double> mean;
    for (double pi : p) {
        mean.push_back(n * pi);
    }

    std::vector<std::vector<double>> covariance;
    for (int row = 0; row < p.size(); ++row) {
        std::vector<double> covariance_row;
        for (int column = 0; column < row; ++column) {
            covariance_row.push_back(-n * p.at(row) * p.at(column));
        }
        covariance_row.push_back(n * p.at(row) * (1.0 - p.at(row)));
        covariance.push_back(covariance_row);
    }

    MultivariateGuassian out(mean, covariance);

    return out;
}

MultivariateGuassian MultivariateGuassian::from_multinomial(const Multinomial& m) {
    std::vector<double> new_p(m.p);
    new_p.pop_back();
    return MultivariateGuassian::from_multinomial(m.n, new_p);
}

double MultivariateGuassian::density(std::vector<double> point) const {
    if (point.size() != mean.size()) {
        BOOST_LOG_TRIVIAL(fatal) << "MultivariateGuassian density input must have the same number "
                                    "of dimensions as the distribution.";
        assert(false);
    }

    // TODO(joschnei): There has to be a better way to do this copy.
    Eigen::VectorXd x(point.size());
    for (int i = 0; i < point.size(); ++i) {
        x[i] = point.at(i);
    }

    Eigen::VectorXd diff = (x - mean).eval();
    Eigen::RowVectorXd mult1 = (diff.transpose() * covariance_inverse).eval();
    double mult2 = (mult1 * diff).eval()(0, 0);
    double det = covariance.determinant();
    double val = std::exp(-1.0 / 2.0 * mult2) / std::sqrt(std::pow(2 * M_PI, mean.size()) * det);
    return val;
}

void MultivariateGuassian::shift_hyperplanes(std::vector<std::vector<double>>& hyperplanes) {
    for (std::vector<double>& hyperplane : hyperplanes) {
        double constant = hyperplane.back();
        for (int i = 0; i < hyperplane.size() - 1; ++i) {
            constant -= hyperplane.at(i) * mean[i];
        }
        hyperplane[hyperplane.size() - 1] = constant;
    }
}

std::vector<std::vector<double>> MultivariateGuassian::get_covariance() const {
    std::vector<std::vector<double>> out;
    for (int row_index = 0; row_index < covariance.rows(); ++row_index) {
        std::vector<double> row;
        for (int column_index = 0; column_index < covariance.cols(); ++column_index) {
            row.push_back(covariance(row_index, column_index));
        }
        out.push_back(row);
    }
    return out;
}

std::vector<double> MultivariateGuassian::get_mean() const {
    std::vector<double> out;
    for (int i = 0; i < mean.size(); ++i) {
        out.push_back(mean[i]);
    }
    return out;
}

}  // namespace FilterModel
