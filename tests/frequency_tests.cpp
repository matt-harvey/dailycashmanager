/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


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
	// TODO HIGH PRIORITY Write some more tests for Frequency conversions.
}

}  // namespace test
}  // namespace phatbooks
