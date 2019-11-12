/* Computes the joint distribution over all the latent variables in the model using Gibbs Sampling.
 *
 * This latent variable model adapted from the one described in "Learning an Input Filter for
 * Argument Structure Acquisition" by Perkins, Feldman, and Lidz. See the paper for details.
 */

#include "metropolis_hastings.hpp"
#include "sample_models.hpp"
#include "utils.hpp"

#include <CLI/CLI.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <tuple>
#include <utility>

using namespace FilterModel;
namespace logging = boost::log;

/**
 * Jointly inferrs the alpha vector of each object, the epsilon, and the delta value for some
 * objects by using a fixed number of iterations of gibbs sampling.
 *
 * The sampling uses the following flow:
 *   alpha -> epsilon -> delta.
 *
 * This function takes as input a vector of category count vectors of each object and the number of
 * iterations or time steps to sample over.
 *
 * This function outputs a tuple of three vectors. The first vector contains the sampled
 * alpha of each object in the data at each time step. So the first element of outer vector
 * is a vector of alphas sampled at the first time step, the second element is the
 * alphas at the second time step, etc. The second and third vectors are the sampled epsilon
 * and delta values at each time step respectively.
 */
std::tuple<std::vector<std::vector<alpha_t>>, std::vector<double>, std::vector<delta_t>,
           std::vector<double>>
joint_inference(std::vector<category_counts_t> &data, int iterations,
                std::function<void(std::vector<std::vector<alpha_t>>, std::vector<double>,
                                   std::vector<delta_t>, std::vector<double>)>
                    write_batch,
                int batch_size, Options options) {
    BOOST_LOG_TRIVIAL(info) << "Starting Gibbs Sampling";

    int n_categories = 0;
    if (data.size() > 0) {
        n_categories = data.at(0).size();
    }

    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

    std::uniform_real_distribution<double> parameter_distribution(0.0, 1.0);
    std::vector<double> epsilons = {parameter_distribution(generator)};
    std::vector<delta_t> deltas = {
        sample_symmetric_simplex(parameter_distribution, generator, n_categories)};
    // Each item contains a vector of the estimated alpha of each object at each timestep.
    std::vector<std::vector<alpha_t>> models;
    models.push_back(std::vector<alpha_t>());

    ModelDistribution model_distribution(data, generator, options);
    ModelSampler sampler(data, generator, options);

    std::vector<double> log_likelyhoods;

    for (int iteration = 1; iteration <= iterations; ++iteration) {
        BOOST_LOG_TRIVIAL(info) << "Iteration " << iteration;

        if (options.fixed_alphas.empty()) {
            std::vector<alpha_t> new_models =
                sampler.sample(epsilons.at(iteration - 1), deltas.at(iteration - 1));
            models.push_back(new_models);
        } else {
            models.push_back(options.fixed_alphas);
        }

        epsilons.push_back(MetropolisHastingsSampler::sample<double, std::default_random_engine>(
            10,  // Iterations
            [delta = deltas.at(iteration - 1), model = models.at(iteration),
             model_distribution](double epsilon) {
                std::vector<std::vector<alpha_t>> alphas_per_object(model.size());
                for (int i = 0; i < alphas_per_object.size(); ++i) {
                    alphas_per_object.at(i) = std::vector<alpha_t>(1, model.at(i));
                }
                std::vector<std::vector<double>> log_alpha_likelyhoods_per_object =
                    model_distribution.distribution(alphas_per_object, epsilon, delta);
                std::vector<double> log_alpha_likelyhoods(log_alpha_likelyhoods_per_object.size());
                std::transform(log_alpha_likelyhoods_per_object.begin(),
                               log_alpha_likelyhoods_per_object.end(),
                               log_alpha_likelyhoods.begin(),
                               [](std::vector<double> v) { return v[0]; });

                return std::accumulate(log_alpha_likelyhoods.begin(), log_alpha_likelyhoods.end(),
                                       0.0);
            },  // log_pdf
            [&parameter_distribution](std::default_random_engine &generator) {
                return parameter_distribution(generator);
            },  // uniform_sampler
            [](double center, std::default_random_engine &generator) {
                std::normal_distribution<> dist(center, 0.25);
                return sample_probability(dist, generator);
            },  // conditional_sampler
            generator));

        deltas.push_back(MetropolisHastingsSampler::sample<delta_t, std::default_random_engine>(
            10,  // Iterations
            [epsilon = epsilons.at(iteration), model = models.at(iteration),
             model_distribution](delta_t delta) {
                std::vector<std::vector<alpha_t>> alphas_per_object(model.size());
                for (int i = 0; i < alphas_per_object.size(); ++i) {
                    alphas_per_object.at(i) = std::vector<alpha_t>(1, model.at(i));
                }
                std::vector<std::vector<double>> log_alpha_likelyhoods_per_object =
                    model_distribution.distribution(alphas_per_object, epsilon, delta);
                std::vector<double> log_alpha_likelyhoods =
                    flatten<double>(log_alpha_likelyhoods_per_object);

                return accumulate(log_alpha_likelyhoods.begin(), log_alpha_likelyhoods.end(), 0.0);
            },  // log_pdf
            [parameter_distribution, n_categories](std::default_random_engine &generator) {
                return sample_symmetric_simplex(parameter_distribution, generator, n_categories);
            },  // uniform_sampler
            [](delta_t center, std::default_random_engine &generator) {
                return sample_gaussian_simplex<>(center, 0.25, generator);
            },  // Conditional sampler
            generator));

        if (options.record_likelyhood) {
            std::vector<double> log_likelyhood_per_object =
                flatten<double>(model_distribution.distribution(
                    models.at(iteration), epsilons.at(iteration), deltas.at(iteration)));
            log_likelyhoods.push_back(std::accumulate(log_likelyhood_per_object.begin(),
                                                      log_likelyhood_per_object.end(), 0.0));
        }

        if (iteration % batch_size == batch_size - 1 || iteration == iterations) {
            int batch = iteration / batch_size + 1;
            int start_index = (batch - 1) * batch_size;
            int end_index = std::min(batch * batch_size, iterations + 1);

            std::vector<std::vector<alpha_t>> alpha_batch(models.begin() + start_index,
                                                          models.begin() + end_index);
            std::vector<double> epsilon_batch(epsilons.begin() + start_index,
                                              epsilons.begin() + end_index);
            std::vector<delta_t> delta_batch(deltas.begin() + start_index,
                                             deltas.begin() + end_index);
            std::vector<double> log_likelyhood_batch;
            if (options.record_likelyhood) {
                log_likelyhood_batch = std::vector<double>(log_likelyhoods.begin() + start_index,
                                                           log_likelyhoods.begin() + end_index);
            }

            write_batch(alpha_batch, epsilon_batch, delta_batch, log_likelyhood_batch);
        }
    }

    return std::make_tuple(models, epsilons, deltas, log_likelyhoods);
}

/**
 * Reads in a csv file of category_count_t objects in order.
 * See category_count_t in types.hpp for details.
 * Expects a header line.
 */
std::vector<category_counts_t> read_category_counts_file(std::string in_path) {
    std::ifstream in_file(in_path);
    std::string line;
    std::vector<category_counts_t> data;
    if (in_file.is_open()) {
        // Trim the header line
        getline(in_file, line);
        while (getline(in_file, line)) {
            std::vector<std::string> items;
            boost::split(items, line, boost::is_any_of(","));
            category_counts_t datum = {std::stoi(items.at(1)), std::stoi(items.at(2)),
                                       std::stoi(items.at(3))};
            data.push_back(datum);
        }
        in_file.close();
    } else {
        BOOST_LOG_TRIVIAL(fatal) << "File " << in_path << " not found.";
        assert(false);
    }

    return data;
}

std::vector<alpha_t> read_alphas(std::string alpha_path) {
    std::ifstream in_file(alpha_path);
    std::string line;
    std::vector<alpha_t> alphas;
    if (in_file.is_open()) {
        // Trim the header line
        getline(in_file, line);
        while (getline(in_file, line)) {
            std::vector<std::string> items;
            boost::split(items, line, boost::is_any_of(","));
            alpha_t alpha = {bool(std::stoi(items.at(1))), bool(std::stoi(items.at(2))),
                             bool(std::stoi(items.at(3)))};
            alphas.push_back(alpha);
        }
        in_file.close();
    } else {
        BOOST_LOG_TRIVIAL(fatal) << "File " << alpha_path << " not found.";
        assert(false);
    }

    return alphas;
}

std::ofstream setup_output(std::string out_path) {
    return std::ofstream(out_path, std::ios::out | std::ios::trunc);
}

/**
 * Sets the current logging level.
 */
void initLogging() {
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
}

int main(int argc, char *const *argv) {
    initLogging();

    // Setting up command line flags.
    CLI::App app{"Input Filter Model"};

    std::string in_path = "input.csv";
    app.add_option("-i,--in", in_path,
                   "The path to a csv file containing pairs of direct object and object counts.")
        ->required();

    std::string out_path = "output.csv";
    app.add_option("-o,--output", out_path, "The path of the output file.")->required();

    int iterations = 1;
    app.add_option("--iterations", iterations,
                   "The number of iterations to run Gibbs sampling for.");

    std::string alpha_path;
    CLI::Option *alpha_path_option = app.add_option(
        "--alpha-path", alpha_path, "Path to fixed alpha values to use, only if provided.");

    std::string message;
    CLI::Option *message_option =
        app.add_option("-m, --message", message, "Message to be written at the top of the output.")
            ->required();

    bool record_likelyhood = false;
    CLI::Option *record_likelyhood_flag =
        app.add_flag("--record-likelyhood", record_likelyhood,
                     "Additionally record the likelyhood of the sampled parameters.");

    bool exact = false;
    CLI::Option *exact_flag =
        app.add_flag("--exact", exact, "Prevent approximations in calcualtion.");

    bool comparison = false;
    app.add_flag("--comparison", comparison,
                 "If both integration methods shosuld be used and compared.")
        ->excludes(exact_flag);

    bool use_smaller_alphas = false;
    CLI::Option *use_smaller_alphas_flag = app.add_flag("--use-smaller-alphas", use_smaller_alphas,
                                                        "Only consider alphas that are possible.")
                                               ->excludes(alpha_path_option);

    CLI11_PARSE(app, argc, argv);

    std::vector<alpha_t> alphas;
    if (!alpha_path.empty()) {
        alphas = read_alphas(alpha_path);
    }

    Options options;
    options.comparison = comparison;
    options.exact = exact;
    options.use_smaller_alphas = use_smaller_alphas;
    options.record_likelyhood = record_likelyhood;
    options.fixed_alphas = alphas;

    auto data = read_category_counts_file(in_path);

    std::ofstream out_file = setup_output(out_path);

    out_file << "Comment: " << message << ", Input path: " << in_path
             << ", Output path: " << out_path << ", Alpha path: " << alpha_path
             << ", Iterations: " << std::to_string(iterations)
             << ", Comparison: " << std::to_string(options.comparison)
             << ", Exact: " << std::to_string(options.exact)
             << ", Use smaller alphas: " << std::to_string(options.use_smaller_alphas)
             << ", Record likelyhood: " << std::to_string(options.record_likelyhood) << std::endl;

    auto write_batch = [&out_file](std::vector<std::vector<alpha_t>> alpha_batch,
                                   std::vector<double> epsilon_batch,
                                   std::vector<delta_t> delta_batch,
                                   std::vector<double> log_likelyhood_batch) {
        for (int i = 0; i < alpha_batch.size(); ++i) {
            if (!log_likelyhood_batch.empty()) {
                out_file << vector_of_vector_to_string<>(alpha_batch.at(i)) << ","
                         << epsilon_batch.at(i) << "," << vector_to_string<>(delta_batch.at(i))
                         << "," << log_likelyhood_batch.at(i) << std::endl;
            } else {
                out_file << vector_of_vector_to_string<>(alpha_batch.at(i)) << ","
                         << epsilon_batch.at(i) << "," << vector_to_string<>(delta_batch.at(i))
                         << std::endl;
            }
        }
    };

    joint_inference(data, iterations, write_batch, 100, options);

    BOOST_LOG_TRIVIAL(info) << "Inference complete.";

    out_file.close();
}
