#include "../multinomial.hpp"

#include <cmath>

#include "gtest/gtest.h"

namespace FilterModel {

const double ERROR = 0.0001;

TEST(Multinomial, CorrectParametersSet) {
    int n = 4;
    std::vector<double> p = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 6.0};
    Multinomial m1(n, p);

    ASSERT_EQ(m1.n, n);
    ASSERT_EQ(m1.p, p);
    ASSERT_EQ(m1.log_adjust, 0.0);

    double log_adjust = -0.05;
    Multinomial m2(n, p, log_adjust);
    ASSERT_EQ(m2.n, n);
    ASSERT_EQ(m2.p, p);
    ASSERT_EQ(m2.log_adjust, log_adjust);
}

TEST(Multinomial, CrashesWithBadP) {
    int n = 5;
    ASSERT_DEATH(Multinomial(n, {-0.5, 0.5, 1.0}), ".*");
    ASSERT_DEATH(Multinomial(n, {2}), ".*");
    ASSERT_DEATH(Multinomial(n, {0.1, 0.1, 0.1}), ".*");
}

TEST(Multinomial, CrashesWithBadN) {
    int n = -1;
    std::vector<double> p = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 6.0};

    ASSERT_DEATH(Multinomial(n, p), ".*");
}

TEST(log_pdf, CorrectWithoutZeros) {
    int n = 4;
    std::vector<double> p = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 6.0};
    Multinomial m(n, p);

    ASSERT_NEAR(m.log_pdf({1, 2, 1}), -2.197224577336219382790490, ERROR);
}

TEST(log_pdf, CorrectWithZeros) {
    int n = 4;
    std::vector<double> p = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 6.0};
    Multinomial m(n, p);

    ASSERT_NEAR(m.log_pdf({0, 3, 1}), -3.701301974112493456163748, ERROR);
}

TEST(log_pdf, CrashesWithBadK) {
    int n = 5;
    std::vector<double> p = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 6.0};
    Multinomial m(n, p);

    ASSERT_DEATH(m.log_pdf({1, 2}), ".*");
    ASSERT_DEATH(m.log_pdf({1, 4}), ".*");
    ASSERT_DEATH(m.log_pdf({1, 5}), ".*");
    ASSERT_DEATH(m.log_pdf({1, 1, 1}), ".*");
    ASSERT_DEATH(m.log_pdf({1, 1, 4}), ".*");
    ASSERT_DEATH(m.log_pdf({1, 1, 1, 1}), ".*");
    ASSERT_DEATH(m.log_pdf({1, 1, 1, 2}), ".*");
    ASSERT_DEATH(m.log_pdf({1, 1, 1, 3}), ".*");
}

TEST(fix_dimensions, UntestedBecauseCurrentlyDeadCode) {}
}  // namespace FilterModel