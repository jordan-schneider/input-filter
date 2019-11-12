#include "gtest/gtest.h"

#include <vector>

namespace FilterModel {

TEST(FillConstructor, WhyIsThisBreaking) {
    std::vector<std::vector<bool>> alphas = {{true, false, true}, {true, true, false}};

    std::vector<std::vector<std::vector<bool>>> alphas_per_obs =
        std::vector<std::vector<std::vector<bool>>>(5, alphas);

    ASSERT_EQ(alphas_per_obs.size(), 5);
}

}  // namespace FilterModel
