// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "date_parser.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <UnitTest++/UnitTest++.h>

using boost::gregorian::date;
using boost::optional;

namespace phatbooks
{
namespace test
{

// %d  is for day component
// %m  is for month component
// %y  is for year component (2 digit)
// %Y  is for year component (full)
// Additional non-standard modifiers might possibly also appear between the
// '%' and the specifier.
// Note the above are not the only specifiers that can possibly appear in
// a strftime-like string; but they are the only ones relevant when doing
// "tolerant" parsing.

TEST(test_date_parser_intolerant_parse)
{
	DateParser const dp0("%d %m %y");
	optional<date> const md0a = dp0.parse("13 10 2013");
	CHECK(!md0a);
	optional<date> const md0b = dp0.parse("13 09 13");
	CHECK(md0b);
	if (md0b) CHECK_EQUAL(*md0b, date(2013, 9, 13));
	optional<date> const md0c = dp0.parse("13 9 13");
	CHECK(md0c);
	if (md0c) CHECK_EQUAL(*md0c, date(2013, 9, 13));
	optional<date> const md0d = dp0.parse("08 31 13", false);
	CHECK(!md0d);
	optional<date> const md0e = dp0.parse("13 09");
	CHECK(!md0e);
	optional<date> const md0f = dp0.parse("12 6 99", false);
	CHECK(md0f);
	if (md0f) CHECK_EQUAL(*md0f, date(1999, 6, 12));
	optional<date> const md0g = dp0.parse("13/10/2013");
	CHECK(!md0g);

	DateParser const dp1("%d/%m/%y");
	optional<date> const md1a = dp1.parse("13/10/13");
	CHECK(md1a);
	if (md1a) CHECK_EQUAL(*md1a, date(2013, 10, 13));
	optional<date> const md1b = dp1.parse("13 10 13");
	CHECK(!md1b);

	DateParser const dp2("%Y .. %d !!! %m");
	optional<date> const md2a = dp2.parse("3019 .. 4 !!! 1");
	CHECK(md2a);
	if (md2a) CHECK_EQUAL(*md2a, date(3019, 1, 4));
	optional<date> const md2b = dp2.parse("2019..4 !!! 1");
	CHECK(md2b);
	if (md2b) CHECK_EQUAL(*md2b, date(2019, 1, 4));
	optional<date> const md2c = dp2.parse("2019 4 1");
	CHECK (!md2c);

	// Spaces in the format string can be skipped in the
	// candidate string.
	DateParser const dp3("%Y %m %d");
	optional<date> const md3a = dp3.parse("20190619");
	CHECK(md3a);
	if (md3a) CHECK_EQUAL(*md3a, date(2019, 6, 19));
	optional<date> const md3b = dp3.parse("2019/06/19");
	CHECK(!md3b);

	// But spaces cannot be inserted in the candidate string
	// if the format string doesn't allow for them.
	DateParser const dp4("%Y%m%d");
	optional<date> const md4a = dp4.parse("20130619");
	CHECK(md4a);
	if (md4a) CHECK_EQUAL(md4a, date(2013, 6, 19));
	optional<date> const md4b = dp4.parse("2013 06 19");
	CHECK(!md4b);
	optional<date> const md4c = dp4.parse("2013 619");
	CHECK(!md4c);
}


}  // namespace test
}  // namespace phatbooks
