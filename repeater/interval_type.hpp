#ifndef GUARD_interval_type_hpp
#define GUARD_interval_type_hpp

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
}  // namespace phatbooks


#endif  // GUARD_interval_type_hpp
