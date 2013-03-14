#include "frequency.hpp"
#include "interval_type.hpp"
#include "phatbooks_tests_common.hpp"
#include <UnitTest++/UnitTest++.h>
#include <typeinfo>

namespace phatbooks
{
namespace test
{


TEST_FIXTURE(TestFixture, test_frequency_phrase_description)
{
	Frequency const frequency1(1, interval_type::days);
	CHECK_EQUAL(frequency_description(frequency1), "every day");
	CHECK
	(	typeid(frequency_description(frequency1)) ==
		typeid(std::string)
	);
	CHECK_EQUAL(frequency_description(frequency1), BString("every day"));

	Frequency const frequency2(12, interval_type::days);
	CHECK_EQUAL(frequency_description(frequency2), "every 12 days");

	Frequency const frequency3(1, interval_type::weeks);
	CHECK_EQUAL(frequency_description(frequency3), "every week");
	
	Frequency const frequency4(2, interval_type::weeks);
	CHECK_EQUAL(frequency_description(frequency4), "every 2 weeks");

	Frequency const frequency5(3, interval_type::months);
	CHECK_EQUAL(frequency_description(frequency5), "every 3 months");

	Frequency const frequency6(1, interval_type::months);
	CHECK_EQUAL(frequency_description(frequency6), "every month");

	Frequency const frequency7(12, interval_type::months);
	CHECK_EQUAL(frequency_description(frequency7), "every 12 months");

	Frequency const frequency8(1, interval_type::month_ends);
	CHECK_EQUAL
	(	frequency_description(frequency8),
		"every month, on the last day of the month"
	);

	Frequency const frequency9(10, interval_type::month_ends);
	CHECK_EQUAL
	(	frequency_description(frequency9),
		"every 10 months, on the last day of the month"
	);
	CHECK_EQUAL
	(	frequency_description(frequency9),
		BString("every 10 months, on the last day of the month")
	);
}





}  // namespace test
}  // namespace phatbooks
