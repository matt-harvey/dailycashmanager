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
