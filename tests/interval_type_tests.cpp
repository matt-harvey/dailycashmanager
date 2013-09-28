// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "interval_type.hpp"
#include <UnitTest++/UnitTest++.h>
#include <wx/string.h>


namespace phatbooks
{
namespace test
{

TEST(test_interval_type_phrase)
{
	CHECK_EQUAL(phrase(IntervalType::days, false), "day");
	CHECK_EQUAL(phrase(IntervalType::weeks, false), wxString("week"));
	CHECK_EQUAL(phrase(IntervalType::weeks, false), "week");
	CHECK_EQUAL(phrase(IntervalType::weeks, false), L"week");
	CHECK_EQUAL(phrase(IntervalType::months, false), "month");
	CHECK_EQUAL
	(	phrase(IntervalType::month_ends, false),
		"month, on the last day of the month"
	);
	CHECK_EQUAL(phrase(IntervalType::days, true), "days");
	CHECK_EQUAL(phrase(IntervalType::weeks, true), "weeks");
	CHECK_EQUAL(phrase(IntervalType::months, true), "months");
	CHECK_EQUAL
	(	phrase(IntervalType::month_ends, true),
		wxString("months, on the last day of the month")
	);
}






}  // namespace test
}  // namespace phatbooks
