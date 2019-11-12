#ifndef MVI3_HPP
#define MVI3_HPP

#include <fstream>
#include <string>
#include <vector>

namespace MVI3 {
class Mvi3 {
   public:
    /**
     * Integrates a multivariate normal guassian over a convex subset.
     * seed: Random number generator seed
     * dof: Degrees of freedom of the covariance matrix, or -1 if unknown.
     * mocar: # of random directions used for each estimate
     * irep: # of individual estimates
     * covariance: A diagonal matrix of guassian covariances
     * hyperplanes: A rectangular matrix containing linear equations specifying hyperplanes of the
     *   form x1 + x2 + ... xk <= y.
     */
    double integrate(int seed, int dof, int mocar, int irep, std::vector<std::vector<double>> covariance,
                     std::vector<std::vector<double>> hyperplanes);

   private:
    const std::string IN_FILE = "c++/mvi3/build/qcalc.in";
    std::ofstream in_stream;
    const std::string OUT_FILE = "c++/mvi3/build/qcalc.out";
    std::ifstream out_stream;
    const std::string EXECUTABLE_PATH = "mvi3/build/mvi3";
    std::vector<std::string> TARGET_LINES = {" mean value is", " estimate of integral is"};
};
}  // namespace MVI3

#endif