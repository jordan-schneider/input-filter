#ifndef MONTE_CARLO_INTEGRATION_HPP
#define MONTE_CARLO_INTEGRATION_HPP

#include <functional>

namespace FilterModel {

template <class T, class generator>
double integrate(const std::function<double(T)> f, const std::function<T(generator &)> sampler,
                 generator &g, double volume, int iterations) {
    double sum = 0.0;
    for (int iteration = 0; iteration < iterations; ++iteration) {
        T point = sampler(g);
        sum += f(point);
    }
    return volume / iterations * sum;
}
}  // namespace FilterModel

#endif