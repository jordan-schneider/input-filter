#include "mvi3.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>

namespace MVI3 {
double Mvi3::integrate(int seed, int dof, int mocar, int irep,
                       std::vector<std::vector<double>> covariance,
                       std::vector<std::vector<double>> hyperplanes) {
    // Possibly wipe qcalc.out and create an empty qcalc.out
    in_stream.open(Mvi3::OUT_FILE, std::ios::out | std::ios::trunc);
    in_stream.close();

    in_stream.open(Mvi3::IN_FILE, std::ios::out | std::ios::trunc);

    // Writes header line.
    in_stream << std::to_string(covariance.size()) << " " << std::to_string(seed) << " "
              << std::to_string(dof) << " " << std::to_string(mocar) << " " << std::to_string(irep)
              << " " << std::to_string(hyperplanes.size()) << std::endl;

    for (int row = 0; row < covariance.size(); ++row) {
        for (int column = 0; column <= row; ++column) {
            in_stream << std::to_string(covariance.at(row).at(column)) << " ";
        }
        in_stream << std::endl;
    }

    for (std::vector<double> hyperplane : hyperplanes) {
        for (double coeff : hyperplane) {
            in_stream << std::to_string(coeff) << " ";
        }
        in_stream << std::endl;
    }

    in_stream << "-1 -1 -1 -1 -1 -1" << std::endl;
    in_stream.close();

    int ignored = chdir("c++/mvi3/build/");
    ignored = system("./mvi3 2>&1 >/dev/null");
    ignored = chdir("../../..");

    out_stream.open(Mvi3::OUT_FILE, std::ios::in);
    std::string line;
    while (getline(out_stream, line)) {
        for (std::string target_line : TARGET_LINES) {
            if (line.substr(0, target_line.size()) == target_line) {
                out_stream.close();
                return stod(line.substr(line.find_first_of("0123456789"), line.size()));
            }
        }
    }

    // Should never happen.
    return -1.0;
};
};  // namespace MVI3