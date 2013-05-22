// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_interval_type_hpp
#define GUARD_interval_type_hpp

#include "b_string.hpp"

namespace phatbooks
{
namespace interval_type
{

/**
 * Enumerated type representing different
 * date interval.
 *
 * Should be self-explanatory; except note that
 * \c month_ends represents an interval between the
 * last day of one month and the last day of the next,
 * whereas \c months simply represents an interval of
 * one month with no particular reference to the end
 * of the month.
 *
 * Note the numbering of the enumeration is significant.
 * The class PhatbooksDatabaseConnection relies on the
 * numbering shown here.
 */
enum IntervalType
{
	days = 1,
	weeks,
	months,
	month_ends
};

}  // namespace interval_type


/**
 * @returns a short phrase describing the interval.
 * E.g. "days", "month", etc.. The phrase is plural if and
 * only if \e is_plural is true.
 */
BString phrase(interval_type::IntervalType x, bool is_plural);


}  // namespace phatbooks


#endif  // GUARD_interval_type_hpp
