#include "../metropolis_hastings.hpp"
#include "../utils.hpp"
#include "gtest/gtest.h"

#include <boost/log/trivial.hpp>
#include <boost/math/distributions/beta.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

namespace FilterModel {
TEST(sample, wtv) {
    boost::math::beta_distribution<double> dist(2, 2);
    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

    std::vector<double> values;

    double x = MetropolisHastingsSampler::sample<double, std::default_random_engine>(
        100000,
        [dist](double x) {
            double p = pdf(dist, x);
            double log_p = std::log(p);
            return log_p;
        },
        [](std::default_random_engine gen) {
            std::uniform_real_distribution<double> uniform(0, 1);
            double x = uniform(gen);
            return x;
        },
        [](double center, std::default_random_engine gen) {
            std::normal_distribution<double> conditional(center, 0.25);
            double x = sample_probability(conditional, gen);
            return x;
        },
        generator, &values);

    std::string out_path =
        "/home/skinnersboxy/perkins-input-filter/MulticlassFilterModel/output/values.csv";
    std::ofstream output_file(out_path);

    if (output_file.is_open()) {
        output_file << vector_to_string(values);
        output_file.close();
    } else {
        BOOST_LOG_TRIVIAL(fatal) << "File " << out_path << " cannot be written to.";
        assert(false);
    }
}

}  // namespace FilterModel