#include "interval_type.hpp"
#include <UnitTest++/UnitTest++.h>


namespace phatbooks
{
namespace test
{

TEST(test_interval_type_phrase)
{
	CHECK_EQUAL(phrase(interval_type::days, false), "day");
	CHECK_EQUAL(phrase(interval_type::weeks, false), "week");
	CHECK_EQUAL(phrase(interval_type::months, false), "month");
	CHECK_EQUAL
	(	phrase(interval_type::month_ends, false),
		"month, on the last day of the month"
	);
	CHECK_EQUAL(phrase(interval_type::days, true), "days");
	CHECK_EQUAL(phrase(interval_type::weeks, true), "weeks");
	CHECK_EQUAL(phrase(interval_type::months, true), "months");
	CHECK_EQUAL
	(	phrase(interval_type::month_ends, true),
		"months, on the last day of the month"
	);
}






}  // namespace test
}  // namespace phatbooks
