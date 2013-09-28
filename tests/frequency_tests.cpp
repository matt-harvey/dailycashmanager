// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "frequency.hpp"
#include "amalgamated_budget.hpp"
#include "interval_type.hpp"
#include "phatbooks_tests_common.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <UnitTest++/UnitTest++.h>
#include <iostream>
#include <typeinfo>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
using jewel::Decimal;


namespace phatbooks
{
namespace test
{


TEST(test_frequency_constructors_assignment_num_steps_and_step_type)
{
	Frequency const freq1(76, IntervalType::days);
	Frequency const freq2(50, IntervalType::weeks);
	Frequency const freq3(-3, IntervalType::months);
	Frequency const freq4(0, IntervalType::month_ends);

	Frequency const freq5(1, IntervalType::days);
	Frequency const freq6(-1, IntervalType::weeks);
	Frequency const freq7(-5014, IntervalType::months);
	Frequency const freq8(0, IntervalType::month_ends);

	CHECK_EQUAL(freq1.num_steps(), 76);
	CHECK(freq1.step_type() == IntervalType::days);
	CHECK_EQUAL(freq2.num_steps(), 50);
	CHECK(freq2.step_type() == IntervalType::weeks);
	CHECK_EQUAL(freq3.num_steps(), -3);
	CHECK(freq3.step_type() == IntervalType::months);
	CHECK_EQUAL(freq4.num_steps(), 0);
	CHECK(freq4.step_type() == IntervalType::month_ends);

	CHECK(freq5.step_type() == IntervalType::days);
	CHECK_EQUAL(freq5.num_steps(), 1);
	CHECK(freq6.step_type() == IntervalType::weeks);
	CHECK_EQUAL(freq6.num_steps(), -1);
	CHECK(freq7.step_type() == IntervalType::months);
	CHECK_EQUAL(freq7.num_steps(), -5014);
	CHECK(freq8.step_type() == IntervalType::month_ends);
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
	CHECK(freq1a.step_type() == IntervalType::days);
	CHECK_EQUAL(freq2a.num_steps(), 50);
	CHECK(freq2a.step_type() == IntervalType::weeks);
	CHECK_EQUAL(freq3a.num_steps(), -3);
	CHECK(freq3a.step_type() == IntervalType::months);
	CHECK_EQUAL(freq4a.num_steps(), 0);
	CHECK(freq4a.step_type() == IntervalType::month_ends);

	CHECK(freq5a.step_type() == IntervalType::days);
	CHECK_EQUAL(freq5a.num_steps(), 1);
	CHECK(freq6a.step_type() == IntervalType::weeks);
	CHECK_EQUAL(freq6a.num_steps(), -1);
	CHECK(freq7a.step_type() == IntervalType::months);
	CHECK_EQUAL(freq7a.num_steps(), -5014);
	CHECK(freq8a.step_type() == IntervalType::month_ends);
	CHECK_EQUAL(freq8a.num_steps(), 0);
}



TEST(test_frequency_phrase_description)
{
	Frequency const frequency1(1, IntervalType::days);
	CHECK_EQUAL(frequency_description(frequency1, "every"), "every day");
	CHECK
	(	typeid(frequency_description(frequency1)) ==
		typeid(std::string)
	);
	CHECK_EQUAL(frequency_description(frequency1), wxString("per day"));

	Frequency const frequency2(12, IntervalType::days);
	CHECK_EQUAL(frequency_description(frequency2), "per 12 days");

	Frequency const frequency3(1, IntervalType::weeks);
	CHECK_EQUAL(frequency_description(frequency3, "every"), "every week");
	
	Frequency const frequency4(2, IntervalType::weeks);
	CHECK_EQUAL(frequency_description(frequency4, " !! "), " !!  2 weeks");

	Frequency const frequency5(3, IntervalType::months);
	CHECK_EQUAL(frequency_description(frequency5), "per 3 months");

	Frequency const frequency6(1, IntervalType::months);
	CHECK_EQUAL(frequency_description(frequency6), "per month");

	Frequency const frequency7(12, IntervalType::months);
	CHECK_EQUAL(frequency_description(frequency7), "per 12 months");

	Frequency const frequency8(1, IntervalType::month_ends);
	CHECK_EQUAL
	(	frequency_description(frequency8, "every"),
		"every month, on the last day of the month"
	);

	Frequency const frequency9(10, IntervalType::month_ends);
	CHECK_EQUAL
	(	frequency_description(frequency9, "every"),
		"every 10 months, on the last day of the month"
	);
	CHECK_EQUAL
	(	frequency_description(frequency9),
		wxString("per 10 months, on the last day of the month")
	);
}


TEST(test_frequency_convert_to_annual)
{
	Frequency const freq_1_days(1, IntervalType::days);
	Frequency const freq_5_days(5, IntervalType::days);
	Frequency const freq_4732_days(4732, IntervalType::days);
	Frequency const freq_1_weeks(1, IntervalType::weeks);
	Frequency const freq_340_weeks(340, IntervalType::weeks);
	Frequency const freq_2_weeks(2, IntervalType::weeks);
	Frequency const freq_1_months(1, IntervalType::months);
	Frequency const freq_5_months(5, IntervalType::months);
	Frequency const freq_6049_months(6049, IntervalType::months);
	Frequency const freq_1_month_ends(1, IntervalType::month_ends);
	Frequency const freq_5_month_ends(5, IntervalType::month_ends);
	
	Frequency const freq_12_months(12, IntervalType::months);
	Frequency const freq_12_month_ends(12, IntervalType::month_ends);

	CHECK_EQUAL
	(	convert_to_annual(freq_1_days, Decimal("100")),
		Decimal("36525")
	);
	CHECK_EQUAL
	(	convert_to_annual(freq_1_days, Decimal("0")),
		Decimal("0")
	);
	CHECK_EQUAL
	(	convert_to_annual(freq_1_days, Decimal("-0.000015")),
		Decimal("-0.00547875")
	);
	CHECK_EQUAL
	(	convert_to_annual(freq_5_days, Decimal("19.6")),
		Decimal("1431.78")
	);
	CHECK_EQUAL
	(	convert_to_annual(freq_5_days, Decimal("6788.97")),
		Decimal("495934.2585")
	);
	Decimal const res_a =
		convert_to_annual(freq_4732_days, Decimal("96.555"));
	CHECK(res_a > Decimal("7.452813"));
	CHECK(res_a < Decimal("7.452814"));
	
	Decimal const res_b =
		convert_to_annual(freq_4732_days, Decimal("-0.0003"));
	CHECK(res_b < Decimal("-0.00002315"));
	CHECK(res_b > Decimal("-0.00002316"));

	Decimal const res_c =
		convert_to_annual(freq_1_weeks, Decimal("1"));
	CHECK(res_c < Decimal("52.1785715"));
	CHECK(res_c > Decimal("52.1785714"));

	Decimal const res_d =
		convert_to_annual(freq_1_weeks, Decimal("9007823.24"));
	CHECK(res_d > Decimal("470015348"));
	CHECK(res_d < Decimal("470015349"));

	Decimal const res_e =
		convert_to_annual(freq_340_weeks, Decimal("4.778245"));
	CHECK(res_e > Decimal("0.73329"));
	CHECK(res_e < Decimal("0.73331"));

	CHECK_EQUAL
	(	convert_to_annual(freq_2_weeks, Decimal("-5.25")),
		Decimal("-136.96875")
	);

	CHECK_EQUAL
	(	convert_to_annual(freq_1_months, Decimal("-6000.26")),
		Decimal("-72003.12")
	);

	CHECK_EQUAL
	(	convert_to_annual(freq_1_months, Decimal("-1")),
		Decimal("-12.00000")
	);

	CHECK_EQUAL
	(	convert_to_annual(freq_1_months, Decimal("0.0000000")),
		Decimal("-.0")
	);

	CHECK_EQUAL
	(	convert_to_annual(freq_5_months, Decimal("40.9800")),
		Decimal("98.352")
	);

	Decimal const res_f =
		convert_to_annual(freq_6049_months, Decimal(-245, 1));
	CHECK(res_f < Decimal("-0.048603"));
	CHECK(res_f > Decimal("-0.048604"));

	CHECK_EQUAL
	(	convert_to_annual(freq_1_month_ends, Decimal("9182.73")),
		Decimal("110192.76")
	);

	CHECK_EQUAL
	(	convert_to_annual(freq_5_month_ends, Decimal("900")),
		Decimal("2160")
	);

	CHECK_EQUAL
	(	convert_to_annual(freq_5_month_ends, Decimal(0, 0)),
		Decimal("0")
	);

	Decimal const amt_a("989.826001");
	CHECK_EQUAL(convert_to_annual(freq_12_months, amt_a), amt_a); 
	Decimal const amt_b("-0.00023");
	CHECK_EQUAL(convert_to_annual(freq_12_months, amt_b), amt_b);
	Decimal const amt_c("1");
	CHECK_EQUAL(convert_to_annual(freq_12_month_ends, amt_c), amt_c);
	Decimal const amt_d("0");
	CHECK_EQUAL(convert_to_annual(freq_12_month_ends, amt_d), amt_d);
}


TEST(frequency_test_convert_from_annual)
{
	CHECK_EQUAL
	(	convert_from_annual
		(	Frequency(1, IntervalType::days),
			Decimal("365.25")
		),
		Decimal("1")
	);
	Decimal const res_a = convert_from_annual
	(	Frequency(15, IntervalType::days),
		Decimal("98.05")
	);
	CHECK(res_a > Decimal("4.0266940"));
	CHECK(res_a < Decimal("4.0266941"));
	Decimal const res_b = convert_from_annual
	(	Frequency(2, IntervalType::weeks),
		Decimal("4000")
	);
	CHECK(res_b > Decimal("153.3196"));
	CHECK(res_b < Decimal("153.3197"));
	Decimal const res_c = convert_from_annual
	(	Frequency(1249, IntervalType::weeks),
		Decimal("-9000.78")
	);
	CHECK(res_c < Decimal("-215451.935"));
	CHECK(res_c > Decimal("-215451.936"));
	Decimal const res_d = convert_from_annual
	(	Frequency(19, IntervalType::months),
		Decimal(".0001001")
	);
	CHECK(res_d > Decimal("0.0001584"));
	CHECK(res_d < Decimal("0.0001585"));
	Decimal const res_e = convert_from_annual
	(	Frequency(1, IntervalType::months),
		Decimal("-3")
	);
	CHECK_EQUAL(res_e, Decimal("-0.25"));
	Decimal const res_f = convert_from_annual
	(	Frequency(3, IntervalType::months),
		Decimal("0")
	);
	CHECK_EQUAL(res_f, Decimal("0"));
	Decimal const res_g = convert_from_annual
	(	Frequency(5000, IntervalType::month_ends),
		Decimal("19.26898")
	);
	CHECK(res_g > Decimal("8028.74166"));
	CHECK(res_g < Decimal("8028.74167"));
	Decimal const res_h = convert_from_annual
	(	Frequency(5, IntervalType::month_ends),
		Decimal("20000000")
	);
	CHECK(res_h > Decimal("8333333.33"));
	CHECK(res_h < Decimal("8333333.34"));
	Decimal const amt_a("-999.9200001");
	Decimal const res_i = convert_from_annual
	(	Frequency(12, IntervalType::month_ends),
		amt_a
	);
	CHECK_EQUAL(res_i, amt_a);
	Decimal const amt_b("0");
	Decimal const res_j = convert_from_annual
	(	Frequency(12, IntervalType::months),
		amt_b
	);
	CHECK_EQUAL(res_j, amt_b);
	Decimal const amt_c("60000000");
	Decimal const res_k = convert_from_annual
	(	Frequency(12, IntervalType::month_ends),
		amt_c
	);
	CHECK_EQUAL(res_k, amt_c);
}


TEST(frequency_test_convert_to_and_from_canonical)
{
	// We test only the Frequencies that are supported by
	// AmalgamatedBudget. These are deliberately restricted,
	// to avoid the possibility of overflow and precision loss on
	// the "round trip" that we are testing here.
	vector<Frequency> frequencies;
	AmalgamatedBudget::generate_supported_frequencies(frequencies);
	vector<Decimal> amounts;
	amounts.push_back(Decimal("0"));
	amounts.push_back(Decimal("1"));
	amounts.push_back(Decimal("3"));
	amounts.push_back(Decimal("2.50"));
	amounts.push_back(Decimal("9.65"));
	amounts.push_back(Decimal("-1"));
	amounts.push_back(Decimal("-9.61"));
	amounts.push_back(Decimal("0.0000015"));
	amounts.push_back(Decimal("6980097228.02"));
	amounts.push_back(Decimal("9.555555"));
	amounts.push_back(Decimal("20000000.01"));
	amounts.push_back(Decimal("-0.0000005"));
	for (vector<Decimal>::size_type i = 0; i != amounts.size(); ++i)
	{
		for (vector<Frequency>::size_type j = 0; j != frequencies.size(); ++j)
		{
			Decimal const amount = amounts[i];
			Frequency const freq = frequencies[j];
			JEWEL_ASSERT (AmalgamatedBudget::supports_frequency(freq));
			Decimal const res_a = convert_to_canonical(freq, amount);
			Decimal const res_b = convert_from_canonical(freq, res_a);
			CHECK_EQUAL(res_b, amount);
		}
	}
}




}  // namespace test
}  // namespace phatbooks
