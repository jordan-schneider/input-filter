#include <boost/log/trivial.hpp>
#include "gtest/gtest.h"

namespace FilterModel {

TEST(boost_test, ImportWorks) { BOOST_LOG_TRIVIAL(info) << "Hi"; }
}  // namespace FilterModel