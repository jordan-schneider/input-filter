#include "multinomial.hpp"
#include "types.hpp"
#include "utils.hpp"

#include <CLI/CLI.hpp>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace FilterModel;

int main(int argc, char *const *argv) {
    // Setting up command line flags.
    CLI::App app{"Inner Loop"};

    int n_positive;
    app.add_option("--n-positive", n_positive, "")->required();

    int n_negative;
    app.add_option("--n-negative", n_negative, "")->required();

    std::vector<std::string> alpha_s;
    app.add_option("-a", alpha_s, "")->required()->expected(3);

    std::vector<std::string> delta_s;
    app.add_option("-d", delta_s, "")->required()->expected(3);

    std::vector<std::string> object_counts_s;
    app.add_option("-k", object_counts_s, "")->required()->expected(3);

    int k_1_negative;
    app.add_option("--k-1-negative", k_1_negative, "")->required();

    std::string out_path;
    app.add_option("--out", out_path, "")->required();

    CLI11_PARSE(app, argc, argv);

    alpha_t alpha = {alpha_s[0] == "1", alpha_s[1] == "1", alpha_s[2] == "1"};

    std::vector<double> delta(3);
    std::transform(delta_s.begin(), delta_s.end(), delta.begin(), [](std::string s) -> double {
        double test = std::stod(s);
        return test;
    });

    category_counts_t object_counts(3);
    std::transform(object_counts_s.begin(), object_counts_s.end(), object_counts.begin(),
                   [](const std::string &s) -> int { return std::stoi(s); });

    std::vector<int> fixed_k_negative = {0, 0, 0};
    for (int i = 0; i < 3; ++i) {
        if (alpha.at(i) == false) {
            fixed_k_negative.at(i) = object_counts.at(i);
        }
    }

    Multinomial m(n_negative, delta);

    std::vector<double> terms;

    for (int k_2_negative = std::max({object_counts[1] - n_positive, fixed_k_negative[1],
                                      n_negative - k_1_negative - object_counts[2]});
         k_2_negative <=
         std::min(n_negative - k_1_negative - fixed_k_negative[2], object_counts[1]);
         ++k_2_negative) {
        int k_3_negative = n_negative - k_1_negative - k_2_negative;
        if (alpha[2] == false && k_3_negative == object_counts[2] ||
            alpha[2] == true && k_3_negative >= 0 && k_3_negative <= object_counts[2]) {
            terms.push_back(m.log_pdf({k_1_negative, k_2_negative, k_3_negative}));
        }
    }

    double partial_sum = stable_sum<double>(terms);

    std::ofstream output_file(out_path);

    if (output_file.is_open()) {
        output_file << std::to_string(partial_sum);

        output_file.close();
    } else {
        assert(false);
    }
}