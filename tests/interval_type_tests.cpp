/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "interval_type.hpp"
#include <boost/test/unit_test.hpp>
#include <wx/string.h>

namespace dcm
{
namespace test
{

BOOST_AUTO_TEST_CASE(test_interval_type_phrase)
{
    BOOST_CHECK_EQUAL(phrase(IntervalType::days, false), "day");
    BOOST_CHECK_EQUAL(phrase(IntervalType::weeks, false), wxString("week"));
    BOOST_CHECK_EQUAL(phrase(IntervalType::weeks, false), "week");
    BOOST_CHECK_EQUAL(phrase(IntervalType::weeks, false), L"week");
    BOOST_CHECK_EQUAL(phrase(IntervalType::months, false), "month");
    BOOST_CHECK_EQUAL
    (   phrase(IntervalType::month_ends, false),
        "month, on the last day of the month"
    );
    BOOST_CHECK_EQUAL(phrase(IntervalType::days, true), "days");
    BOOST_CHECK_EQUAL(phrase(IntervalType::weeks, true), "weeks");
    BOOST_CHECK_EQUAL(phrase(IntervalType::months, true), "months");
    BOOST_CHECK_EQUAL
    (   phrase(IntervalType::month_ends, true),
        wxString("months, on the last day of the month")
    );
}

}  // namespace test
}  // namespace dcm
