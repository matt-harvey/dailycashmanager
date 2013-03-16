#include "frequency.hpp"
#include "interval_type.hpp"
#include "phatbooks_tests_common.hpp"
#include <UnitTest++/UnitTest++.h>
#include <typeinfo>

namespace phatbooks
{
namespace test
{


TEST(test_frequency_constructors_assignment_num_steps_and_step_type)
{
	Frequency const freq1(76, interval_type::days);
	Frequency const freq2(50, interval_type::weeks);
	Frequency const freq3(-3, interval_type::months);
	Frequency const freq4(0, interval_type::month_ends);

	Frequency const freq5(1, interval_type::days);
	Frequency const freq6(-1, interval_type::weeks);
	Frequency const freq7(-5014, interval_type::months);
	Frequency const freq8(0, interval_type::month_ends);

	CHECK_EQUAL(freq1.num_steps(), 76);
	CHECK_EQUAL(freq1.step_type(), interval_type::days);
	CHECK_EQUAL(freq2.num_steps(), 50);
	CHECK_EQUAL(freq2.step_type(), interval_type::weeks);
	CHECK_EQUAL(freq3.num_steps(), -3);
	CHECK_EQUAL(freq3.step_type(), interval_type::months);
	CHECK_EQUAL(freq4.num_steps(), 0);
	CHECK_EQUAL(freq4.step_type(), interval_type::month_ends);

	CHECK_EQUAL(freq5.step_type(), interval_type::days);
	CHECK_EQUAL(freq5.num_steps(), 1);
	CHECK_EQUAL(freq6.step_type(), interval_type::weeks);
	CHECK_EQUAL(freq6.num_steps(), -1);
	CHECK_EQUAL(freq7.step_type(), interval_type::months);
	CHECK_EQUAL(freq7.num_steps(), -5014);
	CHECK_EQUAL(freq8.step_type(), interval_type::month_ends);
	CHECK_EQUAL(freq8.num_steps(), 0);

	Frequency const freq1a(freq1);
	Frequency const freq2a = freq2;
	Frequency const freq3a(freq3);
	Frequency const freq4a = freq4;

	Frequency const freq5a = freq5;
	Frequency const freq6a(freq6);
	Frequency const freq7a = freq7;
	Frequency const freq8a(freq8);

	CHECK_EQUAL(freq1a.num_steps(), 76);
	CHECK_EQUAL(freq1a.step_type(), interval_type::days);
	CHECK_EQUAL(freq2a.num_steps(), 50);
	CHECK_EQUAL(freq2a.step_type(), interval_type::weeks);
	CHECK_EQUAL(freq3a.num_steps(), -3);
	CHECK_EQUAL(freq3a.step_type(), interval_type::months);
	CHECK_EQUAL(freq4a.num_steps(), 0);
	CHECK_EQUAL(freq4a.step_type(), interval_type::month_ends);

	CHECK_EQUAL(freq5a.step_type(), interval_type::days);
	CHECK_EQUAL(freq5a.num_steps(), 1);
	CHECK_EQUAL(freq6a.step_type(), interval_type::weeks);
	CHECK_EQUAL(freq6a.num_steps(), -1);
	CHECK_EQUAL(freq7a.step_type(), interval_type::months);
	CHECK_EQUAL(freq7a.num_steps(), -5014);
	CHECK_EQUAL(freq8a.step_type(), interval_type::month_ends);
	CHECK_EQUAL(freq8a.num_steps(), 0);
}



TEST(test_frequency_phrase_description)
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
