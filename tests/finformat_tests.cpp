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

#include "finformat.hpp"
#include <boost/test/unit_test.hpp>
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <jewel/log.hpp>
#include <wx/app.h>
#include <wx/intl.h>
#include <wx/string.h>

using jewel::Decimal;
using jewel::DecimalFromStringException;

namespace dcm
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
(   Decimal const& decimal,
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

/** Helper function used below */
void localize(wxString& wxs, wxLocale const& loc)
{
    wxString const spot =
        loc.GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_MONEY);
    wxString const sep =
        loc.GetInfo(wxLOCALE_THOUSANDS_SEP, wxLOCALE_CAT_MONEY);
    wxs.Replace(".", "S");
    wxs.Replace(",", "T");
    wxs.Replace("S", spot);
    wxs.Replace("T", sep);
    return;
}

/**
 * A wrapper around wx_to_decimal in which the string passed to \e wxs must
 * use "," and ".", respectively, for thousands separator (if any) and decimal
 * point. The wrapper function then converts these to the thousands separator
 * and decimal point used by locale \e loc, before passing to the underlying
 * wx_to_decimal function. This enables us to test wx_to_decimal in a manner
 * that is independent of the locale of the machine on which we are running the
 * test.
 */
Decimal wx_to_decimal_b
(   wxString wxs,
    wxLocale const& loc,
    DecimalParsingFlags p_flags = DecimalParsingFlags()
)
{
    localize(wxs, loc);
    return wx_to_decimal(wxs, loc, p_flags);
}

/**
 * A wrapper around wx_to_simple_sum in which the string passed to \e wxs must
 * use "," and ".", respectively, for thousands separator (if any) and
 * decimal point. The wrapper fucntion then converts these to the thousands
 * separator and decimal point used by locale \e loc, before passing to the
 * underlying wx_to_simple_sum function. This enables use to test
 * wx_to_simple_sum in a manner that is independent of the locale of the machine
 * on which we are running the test.
 */
Decimal wx_to_simple_sum_b(wxString wxs, wxLocale const& loc)
{
    localize(wxs, loc);
    return wx_to_simple_sum(wxs, loc);
}

BOOST_FIXTURE_TEST_CASE(test_finformat_wx, FinformatTestFixture)
{
    using string_flags::dash_for_zero;
    using string_flags::hard_align_right;

    Decimal const d0(30910, 3);
    BOOST_CHECK_EQUAL(finformat_wx_b(d0, loc), "30.910 ");
    BOOST_CHECK_EQUAL
    (   finformat_wx_b(d0, loc, DecimalFormatFlags().clear(dash_for_zero)),
        "30.910 "
    );
    BOOST_CHECK_EQUAL
    (   finformat_wx_b(d0, loc, DecimalFormatFlags().set(hard_align_right)),
        "30.910"
    );

    Decimal const d1(0, 5);
    BOOST_CHECK_EQUAL
    (   finformat_wx_b(d1, loc, DecimalFormatFlags().clear(dash_for_zero)),
        "0.00000 "
    );
    BOOST_CHECK_EQUAL(finformat_wx_b(d1, loc), "-      ");
    BOOST_CHECK_EQUAL
    (   finformat_wx_b(d1, loc, DecimalFormatFlags().set(hard_align_right)),
        "-     "
    );
    BOOST_CHECK_EQUAL
    (   finformat_wx_b
        (   d1,
            loc,
            DecimalFormatFlags().clear(dash_for_zero).set(hard_align_right)
        ),
        "0.00000"
    );

    Decimal const d2(0, 0);
    BOOST_CHECK_EQUAL(finformat_wx_b(d2, loc), "- ");
    BOOST_CHECK_EQUAL
    (   finformat_wx_b(d2, loc, DecimalFormatFlags().set(hard_align_right)),
        "-"
    );
    BOOST_CHECK_EQUAL
    (   finformat_wx_b(d2, loc, DecimalFormatFlags().clear(dash_for_zero)),
        "0 "
    );

    Decimal const d3(-89677569898, 2);
    BOOST_CHECK_EQUAL(finformat_wx_b(d3, loc), "(896,775,698.98)");
    BOOST_CHECK_EQUAL
    (   finformat_wx_b(d3, loc, DecimalFormatFlags().set(hard_align_right)),
        "(896,775,698.98)"
    );

    Decimal const d4(78699850032, 6);
    BOOST_CHECK_EQUAL(finformat_wx_b(d4, loc), "78,699.850032 ");
    BOOST_CHECK_EQUAL
    (   finformat_wx_b(d4, loc, DecimalFormatFlags().set(hard_align_right)),
        "78,699.850032"
    );

    Decimal const d5(7, 0);
    BOOST_CHECK_EQUAL(finformat_wx_b(d5, loc), "7 ");
    BOOST_CHECK_EQUAL
    (   finformat_wx_b(d5, loc, DecimalFormatFlags().set(hard_align_right)),
        "7"
    );

    Decimal const d6(-58, 0);
    BOOST_CHECK_EQUAL(finformat_wx_b(d6, loc), "(58)");
    BOOST_CHECK_EQUAL
    (   finformat_wx_b(d6, loc, DecimalFormatFlags().set(hard_align_right)),
        "(58)"
    );

    Decimal const d7(1, 3);
    BOOST_CHECK_EQUAL(finformat_wx_b(d7, loc), "0.001 ");

    Decimal const d8(-98, 6);
    BOOST_CHECK_EQUAL(finformat_wx_b(d8, loc), "(0.000098)");
}

BOOST_FIXTURE_TEST_CASE(test_wx_to_decimal, FinformatTestFixture)
{
    using string_flags::allow_negative_parens;

    BOOST_CHECK_EQUAL(wx_to_decimal_b("", loc), Decimal(0, 0));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("-", loc), Decimal(0, 0));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("   ", loc), Decimal(0, 0));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("   - ", loc), Decimal(0, 0));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("", loc).places(), 0);
    BOOST_CHECK_EQUAL(wx_to_decimal_b("-", loc).places(), 0);
    BOOST_CHECK_EQUAL(wx_to_decimal_b("    ", loc).places(), 0);
    BOOST_CHECK_EQUAL(wx_to_decimal_b("   -  ", loc).places(), 0);

    BOOST_CHECK_EQUAL(wx_to_decimal_b("98", loc), Decimal(98, 0));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("98", loc).places(), 0);
    BOOST_CHECK_EQUAL(wx_to_decimal_b(" 98.6986", loc), Decimal(986986, 4));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("-0", loc), Decimal(0, 0));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("0.0000", loc).places(), 4);
    BOOST_CHECK_EQUAL(wx_to_decimal_b("(6,915,768.23)", loc), Decimal(-691576823, 2));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("(6915768.23)", loc), Decimal(-691576823, 2));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("   00000.68  ", loc), Decimal(68, 2));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("-.590", loc), Decimal(-590, 3));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("-.590", loc).places(), 3);
    BOOST_CHECK_EQUAL(wx_to_decimal_b("5080", loc), Decimal(5080, 0));
    BOOST_CHECK_EQUAL(wx_to_decimal_b("5,080", loc), Decimal(5080, 0));

    BOOST_CHECK_EQUAL
    (   wx_to_decimal_b("5", loc, DecimalParsingFlags().
            clear(allow_negative_parens)),
        Decimal(5, 0)
    );
    BOOST_CHECK_THROW
    (   wx_to_decimal_b("(5)", loc, DecimalParsingFlags().
            clear(allow_negative_parens)),
        DecimalFromStringException
    );
    BOOST_CHECK_EQUAL(wx_to_decimal_b("-5", loc), Decimal(-5, 0));
}

BOOST_FIXTURE_TEST_CASE(test_wx_to_simple_sum, FinformatTestFixture)
{
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("", loc), Decimal(0, 0));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("-", loc), Decimal(0, 0));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("   ", loc), Decimal(0, 0));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("   - ", loc), Decimal(0, 0));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("", loc).places(), 0);
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("-", loc).places(), 0);
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("    ", loc).places(), 0);
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("   -  ", loc).places(), 0);

    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("98", loc), Decimal(98, 0));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("98", loc).places(), 0);
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b(" 98.6986", loc), Decimal(986986, 4));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("-0", loc), Decimal(0, 0));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("0.0000", loc).places(), 4);
    BOOST_CHECK_THROW
    (   wx_to_simple_sum_b("(6,915,768.23)", loc),
        DecimalFromStringException
    );
    BOOST_CHECK_THROW
    (   wx_to_simple_sum_b("(6915768.23)", loc),
        DecimalFromStringException
    );
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("   00000.68  ", loc), Decimal(68, 2));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("-.590", loc), Decimal(-590, 3));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("-.590", loc).places(), 3);
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("5080", loc), Decimal(5080, 0));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("5,080", loc), Decimal(5080, 0));
    BOOST_CHECK_EQUAL(wx_to_simple_sum_b("-5", loc), Decimal(-5, 0));

    BOOST_CHECK_EQUAL(wx_to_simple_sum("+898", loc), Decimal(898, 0));
    BOOST_CHECK_EQUAL(wx_to_simple_sum("+987 +.57", loc), Decimal(98757, 2));
    BOOST_CHECK_EQUAL(wx_to_simple_sum("79.1- 3+0.1", loc), Decimal(762, 1));
    JEWEL_LOG_TRACE();
    wxString const exp0(" 500.677 - 1.09 + 2 - 50.007");
    BOOST_CHECK_EQUAL(wx_to_simple_sum(exp0, loc), Decimal(451580, 3));
    BOOST_CHECK_EQUAL(wx_to_simple_sum(exp0, loc).places(), 3);
    BOOST_CHECK_EQUAL(wx_to_simple_sum(" -70 + 0 + 0", loc), Decimal(-70, 0));
}

}  // namespace test
}  // namespace dcm
