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

#include "finformat.hpp"
#include <jewel/decimal.hpp>
#include <wx/app.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <UnitTest++/UnitTest++.h>

using jewel::Decimal;

namespace phatbooks
{
namespace test
{

struct FinformatTestFixture
{
	FinformatTestFixture()
	{
		// We need this only to make sure we can use wxLocale.
		wxApp* app = new wxApp;

		(void)app;  // silence compiler warning re. unused parameter
		loc.Init(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT);
	}
	wxLocale loc;
};

/**
 * A wrapper around finformat_wx in which the thousands separator is
 * replaced with "," and the decimal point character with ".". This allows
 * us to test finformat_wx in a manner that is independent of the locale of
 * the machine on which we are running the test.
 */
wxString finformat_wx_b
(	Decimal const& decimal,
	wxLocale const& loc,
	DecimalFormatFlags p_flags = DecimalFormatFlags()
)
{
	wxString ret = finformat_wx(decimal, loc, p_flags);
	wxString const spot =
		loc.GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_MONEY);
	wxString const sep =
		loc.GetInfo(wxLOCALE_THOUSANDS_SEP, wxLOCALE_CAT_MONEY);
	ret.Replace(spot, "S");
	ret.Replace(sep, "T");
	ret.Replace("S", ".");
	ret.Replace("T", ",");
	return ret;
}

TEST_FIXTURE(FinformatTestFixture, test_finformat_wx)
{
	using string_flags::dash_for_zero;
	using string_flags::hard_align_right;

	Decimal const d0(30910, 3);
	CHECK_EQUAL(finformat_wx_b(d0, loc), "30.910 ");
	CHECK_EQUAL
	(	finformat_wx_b(d0, loc, DecimalFormatFlags().clear(dash_for_zero)),
		"30.910 "
	);
	CHECK_EQUAL
	(	finformat_wx_b(d0, loc, DecimalFormatFlags().set(hard_align_right)),
		"30.910"
	);

	Decimal const d1(0, 5);
	CHECK_EQUAL
	(	finformat_wx_b(d1, loc, DecimalFormatFlags().clear(dash_for_zero)),
		"0.00000 "
	);
	CHECK_EQUAL(finformat_wx_b(d1, loc), "-      ");
	CHECK_EQUAL
	(	finformat_wx_b(d1, loc, DecimalFormatFlags().set(hard_align_right)),
		"-     "
	);
	CHECK_EQUAL
	(	finformat_wx_b
		(	d1,
			loc,
			DecimalFormatFlags().clear(dash_for_zero).set(hard_align_right)
		),
		"0.00000"
	);

	Decimal const d2(0, 0);
	CHECK_EQUAL(finformat_wx_b(d2, loc), "- ");
	CHECK_EQUAL
	(	finformat_wx_b(d2, loc, DecimalFormatFlags().set(hard_align_right)),
		"-"
	);
	CHECK_EQUAL
	(	finformat_wx_b(d2, loc, DecimalFormatFlags().clear(dash_for_zero)),
		"0 "
	);

	Decimal const d3(-89677569898, 2);
	CHECK_EQUAL(finformat_wx_b(d3, loc), "(896,775,698.98)");
	CHECK_EQUAL
	(	finformat_wx_b(d3, loc, DecimalFormatFlags().set(hard_align_right)),
		"(896,775,698.98)"
	);

	Decimal const d4(78699850032, 6);
	CHECK_EQUAL(finformat_wx_b(d4, loc), "78,699.850032 ");
	CHECK_EQUAL
	(	finformat_wx_b(d4, loc, DecimalFormatFlags().set(hard_align_right)),
		"78,699.850032"
	);

	Decimal const d5(7, 0);
	CHECK_EQUAL(finformat_wx_b(d5, loc), "7 ");
	CHECK_EQUAL
	(	finformat_wx_b(d5, loc, DecimalFormatFlags().set(hard_align_right)),
		"7"
	);

	Decimal const d6(-58, 0);
	CHECK_EQUAL(finformat_wx_b(d6, loc), "(58)");
	CHECK_EQUAL
	(	finformat_wx_b(d6, loc, DecimalFormatFlags().set(hard_align_right)),
		"(58)"
	);

	Decimal const d7(1, 3);
	CHECK_EQUAL(finformat_wx_b(d7, loc), "0.001 ");

	Decimal const d8(-98, 6);
	CHECK_EQUAL(finformat_wx_b(d8, loc), "(0.000098)");
}

TEST_FIXTURE(FinformatTestFixture, test_wx_to_decimal)
{
	// TODO HIGH PRIORITY Write these tests, and make sure they're
	// locale-neutral.
}

TEST_FIXTURE(FinformatTestFixture, test_wx_to_simple_sum)
{
	// TODO HIGH PRIORITY Write these tests, and make sure they're
	// locale-neutral.
}




}  // namespace test
}  // namespace phatbooks
