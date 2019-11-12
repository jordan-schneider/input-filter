#ifndef UTILS_HPP
#define UTILS_HPP

/**
 * Various utility functions.
 */

#include <assert.h>
#include <algorithm>
#include <random>
#include <vector>

namespace FilterModel {

template <class T>
inline void assert_nonnegative(std::vector<T> k) {
    for (T k_i : k) {
        assert(k_i >= 0);
    }
}

template <class T>
inline void assert_probability(std::vector<T> p) {
    static const double ERROR = 0.0001;

    assert(std::abs(std::accumulate(p.begin(), p.end(), 0.0) - 1.0) < ERROR);
    for (T p_i : p) {
        assert(p_i >= 0 && p_i <= 1.0);
    }
}

static inline std::vector<bool> not_v(std::vector<bool> v) {
    for (int i = 0; i < v.size(); ++i) {
        v.at(i) = !v.at(i);
    }
    return v;
}

template <class T>
inline std::vector<T> bool_to_index(std::vector<bool> bools, bool value = true) {
    std::vector<T> out;
    for (T i = 0; i < bools.size(); ++i) {
        if (bools.at(i) == value) {
            out.push_back(i);
        }
    }
    return out;
};

/**
 * Exact floating point sum algorithm. Taken from the python math library fsum algorithm.
 *
 * This takes linear time and space. The Kahan sum has a larger error but takes linear time
 * and constant space. If space becomes a constraint, consider switching to the Kahan sum.
 */
template <class T>
double stable_sum(std::vector<T> &numbers) {
    if (numbers.size() == 0) {
        return 0.0;
    }

    int n = 0;

    std::vector<T> partials;
    for (auto number : numbers) {
        number = number;
        if (number == -INFINITY) {
            return -INFINITY;
        }
        int i = 0;
        for (int j = 0; j < n; ++j) {
            volatile T partial = partials.at(j);
            if (abs(number) < abs(partial)) {
                T temp = number;
                number = partial;
                partial = temp;
            }
            volatile T hi = number + partial;
            volatile T lo = partial - (hi - number);
            if (lo) {
                partials.at(i) = lo;
                ++i;
            }
            number = hi;
        }
        partials.resize(i);
        partials.push_back(number);
        n = partials.size();
    }

    volatile T hi = partials.back();
    volatile T lo;
    int i = partials.size() - 2;
    while (i >= 0) {
        volatile T x = hi;
        volatile T y = partials.at(i);
        hi = x + y;
        volatile T yr = hi - x;
        lo = y - yr;
        i--;
        if (lo != 0.0) {
            break;
        }
    }
    if (i > 0 && ((lo < 0.0 && partials[i - 1] < 0.0) || (lo > 0.0 && partials[i - 1] > 0.0))) {
        volatile T y = lo * 2.0;
        volatile T x = hi + y;
        volatile T yr = x - hi;
        if (y == yr) {
            hi = x;
        }
    }
    return hi;
};

/**
 * Samples a value from a distribution using a given generator with a fixed upper and lower bound.
 *
 * Simply samples until the sampled value is within the range.
 */
template <class distribution, class generator>
inline double sample_in_range(distribution dist, generator &gen, double lower, double upper) {
    double number;
    while (number = dist(gen), (number < lower) || (number > upper))
        ;
    return number;
}

/**
 * Samples a probability from the given distibution using the given generator.
 */
template <class distribution, class generator>
inline double sample_probability(distribution dist, generator &gen) {
    return sample_in_range(dist, gen, 0.0, 1.0);
}

/**
 * Symmetrically samples a simplex vector of a given dimension from the given distribution and
 * generator.
 *
 * A simplex vector is a vector whose values sum to 1. This is often used as a probability vector.
 *
 * Each dimension is sampled from the given distribution and then the whole vector is normalized
 * so that its l_1 norm is 1 i.e. sum(simplex) == 1.
 */
template <class distribution, class generator>
std::vector<double> sample_symmetric_simplex(distribution dist, generator &gen, int dimension) {
    std::vector<double> simplex;
    double norm = 0.0;
    for (int i = 0; i < dimension; ++i) {
        double next = sample_probability(dist, gen);
        norm += next;
        simplex.push_back(next);
    }

    for (int i = 0; i < dimension; ++i) {
        simplex.at(i) /= norm;
    }
    return simplex;
}

/**
 * Samples a simplex vector of given dimension with components from a vector of given distributions
 * using a given random number generator.
 */
template <class distribution, class generator>
inline std::vector<double> sample_simplex(std::vector<distribution> dists, generator &gen,
                                          int dimension) {
    std::vector<double> simplex;
    double norm = 0.0;
    for (int i = 0; i < dimension; ++i) {
        double next = sample_probability(dists.at(i), gen);
        norm += next;
        simplex.push_back(next);
    }

    for (int i = 0; i < dimension; ++i) {
        simplex.at(i) /= norm;
    }
    return simplex;
}

/**
 * Samples a simplex from a normalized guassian distribution with a given mean and standard
 * deviation with identity covariance.
 */
template <class generator>
inline std::vector<double> sample_gaussian_simplex(std::vector<double> center, double std,
                                                   generator &gen) {
    std::vector<std::normal_distribution<>> dists;
    for (double mu_i : center) {
        dists.push_back(std::normal_distribution<>(mu_i, std));
    }
    return sample_simplex(dists, gen, center.size());
}

/**
 * Converts a vector to a string with square brackets and commas seprators.
 */
template <class T>
std::string vector_to_string(std::vector<T> v) {
    std::string out = "[";

    bool first_entry = true;
    for (T t : v) {
        if (first_entry) {
            out += std::to_string(t);
            first_entry = false;
        } else {
            out += "," + std::to_string(t);
        }
    }
    out += "]";
    return out;
}

/**
 * Convers a vector of vectors to a string of nested square brackets and comma separators.
 */
template <class T>
std::string vector_of_vector_to_string(std::vector<std::vector<T>> vv) {
    std::string out = "[";
    bool first_vector = true;
    for (std::vector<T> v : vv) {
        if (first_vector) {
            out += vector_to_string(v);
            first_vector = false;
        } else {
            out += "," + vector_to_string(v);
        }
    }
    out += "]";
    return out;
}

/**
 * Exponentiates the values of a given vector.
 */
template <class T>
inline std::vector<T> exp(const std::vector<T> &numbers) {
    std::vector<T> out(numbers.size());
    std::transform(numbers.begin(), numbers.end(), out.begin(),
                   [](T x) -> T { return std::exp(x); });
    return out;
}

template <class T>
inline std::vector<T> flatten(const std::vector<std::vector<T>> &vv) {
    std::vector<double> out(vv.size());
    std::transform(vv.begin(), vv.end(), out.begin(), [](std::vector<double> v) { return v[0]; });
    return out;
}

}  // namespace FilterModel
#endif