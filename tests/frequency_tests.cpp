#include "frequency.hpp"
#include "interval_type.hpp"
#include "phatbooks_tests_common.hpp"
#include <jewel/decimal.hpp>
#include <UnitTest++/UnitTest++.h>
#include <typeinfo>

#include <iostream>
using std::cout;
using std::endl;


using jewel::Decimal;

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


TEST(test_frequency_convert_to_annual)
{
	Frequency const freq_1_days(1, interval_type::days);
	Frequency const freq_5_days(5, interval_type::days);
	Frequency const freq_4732_days(4732, interval_type::days);
	Frequency const freq_1_weeks(1, interval_type::weeks);
	Frequency const freq_340_weeks(340, interval_type::weeks);
	Frequency const freq_2_weeks(2, interval_type::weeks);
	Frequency const freq_1_months(1, interval_type::months);
	Frequency const freq_5_months(5, interval_type::months);
	Frequency const freq_6049_months(6049, interval_type::months);
	Frequency const freq_1_month_ends(1, interval_type::month_ends);
	Frequency const freq_5_month_ends(5, interval_type::month_ends);
	
	Frequency const freq_12_months(12, interval_type::months);
	Frequency const freq_12_month_ends(12, interval_type::month_ends);

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
		(	Frequency(1, interval_type::days),
			Decimal("365.25")
		),
		Decimal("1")
	);
	Decimal const res_a = convert_from_annual
	(	Frequency(15, interval_type::days),
		Decimal("98.05")
	);
	CHECK(res_a > Decimal("4.0266940"));
	CHECK(res_a < Decimal("4.0266941"));
	Decimal const res_b = convert_from_annual
	(	Frequency(2, interval_type::weeks),
		Decimal("4000")
	);
	CHECK(res_b > Decimal("153.3196"));
	CHECK(res_b < Decimal("153.3197"));
	Decimal const res_c = convert_from_annual
	(	Frequency(1249, interval_type::weeks),
		Decimal("-9000.78")
	);
	CHECK(res_c < Decimal("-215451.935"));
	CHECK(res_c > Decimal("-215451.936"));
	Decimal const res_d = convert_from_annual
	(	Frequency(19, interval_type::months),
		Decimal(".0001001")
	);
	CHECK(res_d > Decimal("0.0001584"));
	CHECK(res_d < Decimal("0.0001585"));
	Decimal const res_e = convert_from_annual
	(	Frequency(1, interval_type::months),
		Decimal("-3")
	);
	CHECK_EQUAL(res_e, Decimal("-0.25"));
	Decimal const res_f = convert_from_annual
	(	Frequency(3, interval_type::months),
		Decimal("0")
	);
	CHECK_EQUAL(res_f, Decimal("0"));
	Decimal const res_g = convert_from_annual
	(	Frequency(5000, interval_type::month_ends),
		Decimal("19.26898")
	);
	CHECK(res_g > Decimal("8028.74166"));
	CHECK(res_g < Decimal("8028.74167"));
	Decimal const res_h = convert_from_annual
	(	Frequency(5, interval_type::month_ends),
		Decimal("20000000")
	);
	CHECK(res_h > Decimal("8333333.33"));
	CHECK(res_h < Decimal("8333333.34"));
	Decimal const amt_a("-999.9200001");
	Decimal const res_i = convert_from_annual
	(	Frequency(12, interval_type::month_ends),
		amt_a
	);
	CHECK_EQUAL(res_i, amt_a);
	Decimal const amt_b("0");
	Decimal const res_j = convert_from_annual
	(	Frequency(12, interval_type::months),
		amt_b
	);
	CHECK_EQUAL(res_j, amt_b);
	Decimal const amt_c("60000000");
	Decimal const res_k = convert_from_annual
	(	Frequency(12, interval_type::month_ends),
		amt_c
	);
	CHECK_EQUAL(res_k, amt_c);
}


TEST(frequency_test_convert_to_and_from_canonical)
{
	Frequency const freq_1_days(1, interval_type::days);
	Frequency const freq_5_days(5, interval_type::days);
	Frequency const freq_4732_days(4732, interval_type::days);
	Frequency const freq_30681_days(30681, interval_type::days);
	Frequency const freq_1_weeks(1, interval_type::weeks);
	Frequency const freq_340_weeks(340, interval_type::weeks);
	Frequency const freq_3_weeks(2, interval_type::weeks);
	Frequency const freq_1_months(1, interval_type::months);
	Frequency const freq_5_months(5, interval_type::months);
	Frequency const freq_17_months(17, interval_type::months);
	Frequency const freq_6049_months(6049, interval_type::months);
	Frequency const freq_1_month_ends(1, interval_type::month_ends);
	Frequency const freq_5_month_ends(5, interval_type::month_ends);

	Decimal const dec_0("0");
	Decimal const dec_1("1");
	Decimal const dec_3("3");
	Decimal const dec_2_50("2.50");
	Decimal const dec_9_65("9.65");
	Decimal const dec_n1("-1");
	Decimal const dec_n9_61("-9.61");
	Decimal const dec_0_0000015("0.0000015");
	Decimal const dec_n6980097228_02("6980097228.02");
	Decimal const dec_9_55555("9.555555");
	Decimal const dec_20000000_01("20000000.01");
	Decimal const dec_n0000005("-0.0000005");

	// Testing that "round trip" journeys to and from the canonical
	// Frequency, leave us back where we started. Note the actual
	// value of the canonical Frequency is not publically
	// exposed.

	Decimal const res_a = convert_to_canonical(freq_1_days, dec_0);
	Decimal const res_b = convert_from_canonical(freq_1_days, res_a);
	CHECK_EQUAL(res_b, dec_0);

	Decimal const res_c = convert_to_canonical(freq_1_days, dec_n0000005);
	Decimal const res_d = convert_from_canonical(freq_1_days, res_c);
	CHECK_EQUAL(res_d, dec_n0000005);

	Decimal const res_e = convert_to_canonical(freq_5_month_ends, dec_1);
	Decimal const res_f = convert_from_canonical(freq_5_month_ends, res_e);
	CHECK_EQUAL(res_f, dec_1);

	Decimal const res_g =
		convert_to_canonical(freq_1_month_ends, dec_20000000_01);
	Decimal const res_h = convert_from_canonical(freq_1_month_ends, res_g);
	CHECK_EQUAL(res_h, dec_20000000_01);

	Decimal const res_i = convert_to_canonical(freq_6049_months, dec_3);
	// Rounding to prevent DecimalMultiplicationException on the way
	// back. We would have to do this in application code.
	Decimal const res_j = convert_from_canonical(freq_6049_months, res_i);
	CHECK(res_j < dec_3 + Decimal(1, dec_3.places() + 2));
	CHECK(res_j > dec_3 - Decimal(1, dec_3.places() + 2));
	cout << res_j << endl;

	Decimal const res_k = convert_to_canonical(freq_4732_days, dec_9_55555);
	// Rounding to prevent DecimalMultiplicationException on the way
	// back. We would have to do this in application code.
	Decimal const res_kr = round(res_k, dec_9_55555.places() * 2);
	Decimal const res_l = convert_from_canonical(freq_4732_days, res_kr);
	CHECK(res_l < dec_9_55555 + Decimal(1, dec_9_55555.places() + 2));
	CHECK(res_l > dec_9_55555 - Decimal(1, dec_9_55555.places() + 2));
	cout << res_l << endl;
	
	Decimal const res_m = convert_to_canonical(freq_5_months, dec_2_50);
	Decimal const res_n = convert_from_canonical(freq_5_months, res_m);
	CHECK_EQUAL(res_n, dec_2_50);

	Decimal const res_o =
		convert_to_canonical(freq_30681_days, dec_n6980097228_02);
	// Rounding to prevent DecimalMultiplicationException on the way
	// back. We would have to do this in application code.
	Decimal const res_or = round(res_o, dec_n6980097228_02.places() * 2);
	Decimal const res_p = convert_from_canonical(freq_30681_days, res_or);
	CHECK
	(	res_p <
		dec_n6980097228_02 + Decimal(1, dec_n6980097228_02.places() + 2)
	);
	CHECK
	(	res_p >
		dec_n6980097228_02 - Decimal(1, dec_n6980097228_02.places() + 2)
	);

	Decimal const res_q = convert_to_canonical(freq_1_weeks, dec_9_65);
	Decimal const res_r = convert_from_canonical(freq_1_weeks, res_q);
	CHECK_EQUAL(res_r, dec_9_65);

	Decimal const res_s = convert_to_canonical(freq_340_weeks, dec_0_0000015);
	Decimal const res_t = convert_from_canonical(freq_340_weeks, res_s);
	CHECK_EQUAL(res_t, dec_0_0000015);

	Decimal const res_u = convert_to_canonical(freq_3_weeks, dec_n1);
	Decimal const res_v = convert_from_canonical(freq_3_weeks, res_u);
	CHECK_EQUAL(res_v, dec_n1);

	Decimal const res_w = convert_to_canonical(freq_1_months, dec_n9_61);
	Decimal const res_x = convert_from_canonical(freq_1_months, res_w);
	CHECK_EQUAL(res_x, dec_n9_61);

	Decimal const res_y = convert_to_canonical(freq_17_months, dec_n9_61);
	// Rounding to prevent DecimalMultiplicationException on the way
	// back. We would have to do this in application code.
	Decimal const res_yr = round(res_y, dec_n9_61.places() * 2);
	Decimal const res_z = convert_from_canonical(freq_17_months, res_yr);
	CHECK(res_z < dec_n9_61 + Decimal(1, dec_n9_61.places() + 2));
	CHECK(res_z > dec_n9_61 - Decimal(1, dec_n9_61.places() + 2));
}




}  // namespace test
}  // namespace phatbooks
