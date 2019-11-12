#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>

/**
 * Contains typedefs used throughout the project.
 */

namespace FilterModel {

// One count how many uses of a specific object (e.g. determiner) were observed being used in some
// context. E.g. if the object is 'the' and the value is 5 then 'the' was seen being used in some
// context 5 times.
typedef int category_count_t;

// A vector of uses of a specific object in each context.
// E.g. if the object is 'the' and the categories are 'MASS', 'PLURAL', or 'SINGULAR_OR_MASS' then
// <6,4,1> means that the the was seen 6 times with a noun in the 'MASS' category, 4
// times with a noun in the 'PLURAL' category, and 1 time with a noun in the 'SINGULAR_OR_MASS' category.
typedef std::vector<category_count_t> category_counts_t;

// Boolean value representing if a specific object can be used in some context.
typedef bool category_t;
// A vector of category_ts representing the allowable usage context.
// E.g. if the object is 'the' and the categories are 'MASS', 'PLURAL', or 'SINGULAR_OR_MASS' then
// <1,0,1> means 'the' can be used in the 'MASS' and 'SINGULAR_OR_MASS' contexts, but not the
// 'PLURAL' context.
typedef std::vector<category_t> alpha_t;
// A vector of doubles representing the probability of a given error observation falling into each
// context.
// E.g. if the contexts are 'MASS', 'PLURAL', or 'SINGULAR_OR_MASS' then <1/2,1/3,1/6> means that a
// given error observation has 1/2 odds of being a 'MASS' noun, 1/3 odds of being a
// 'PLURAL' noun, and 1/6 odds of being a 'SINGULAR_OR_MASS' noun.
typedef std::vector<double> delta_t;

struct Options {
    bool comparison = false;
    bool exact = false;
    bool use_smaller_alphas = false;
    bool record_likelyhood = false;

    std::vector<alpha_t> fixed_alphas;
};

}  // namespace FilterModel

#endif