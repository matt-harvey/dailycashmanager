// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "date.hpp"
#include "date_parser.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <UnitTest++/UnitTest++.h>
#include <iostream>

using boost::gregorian::date;
using boost::optional;
using std::cout;
using std::endl;

namespace phatbooks
{
namespace test
{

// %d  is for day component
// %m  is for month component
// %y  is for year component (2 digit)
// %Y  is for year component (full)
// Additional modifiers might possibly appear between the
// '%' and the format specifier.
// Note the above are not the only format specifiers that can possibly appear
// in a strftime-like string; but they are the only ones relevant when doing
// "tolerant" parsing.

TEST(test_date_parser_intolerant_parse)
{
	DateParser const dp0("%d %m %y", "%d %m %y");
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
	optional<date> const md0h = dp0.parse("21");
	CHECK(!md0h);

	DateParser const dp1("%d/%m/%y", "%d/%m/%y");
	optional<date> const md1a = dp1.parse("13/10/13");
	CHECK(md1a);
	if (md1a) CHECK_EQUAL(*md1a, date(2013, 10, 13));
	optional<date> const md1b = dp1.parse("13 10 13");
	CHECK(!md1b);

	DateParser const dp2("%Y .. %d !!! %m", "%Y .. %d !!! %m");
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
	DateParser const dp3("%Y %m %d", "%Y %m %d");
	optional<date> const md3a = dp3.parse("20190619");
	CHECK(md3a);
	if (md3a) CHECK_EQUAL(*md3a, date(2019, 6, 19));
	optional<date> const md3b = dp3.parse("2019/06/19");
	CHECK(!md3b);

	// But spaces cannot be inserted in the candidate string
	// if the format string doesn't allow for them.
	DateParser const dp4("%Y%m%d", "%Y%m%d");
	optional<date> const md4a = dp4.parse("20130619");
	CHECK(md4a);
	if (md4a) CHECK_EQUAL(*md4a, date(2013, 6, 19));
	optional<date> const md4b = dp4.parse("2013 06 19");
	CHECK(!md4b);
	optional<date> const md4c = dp4.parse("2013 619");
	CHECK(!md4c);

	// "%Y" can accept either 2-digit or 4-digit year.
	DateParser const dp5("%Y . %m . %d", "%Y . %m . %d");
	optional<date> const md5a = dp5.parse("2010.01.05");
	CHECK(md5a);
	if (md5a) CHECK_EQUAL(*md5a, date(2010, 1, 5));
	optional<date> const md5b = dp5.parse("10.05.05");
	CHECK(md5b);
	if (md5b) CHECK_EQUAL(*md5b, date(2010, 5, 5));
	// But "%y" will only accept 2-digit year
	DateParser const dp5x("%y . %m . %d", "%y . %m . %d");
	optional<date> const md5xa = dp5x.parse("2010.01.05");	
	CHECK(!md5xa);
	optional<date> const md5xb = dp5x.parse("10.01.05");
	CHECK(md5xb);
	if (md5xb) CHECK_EQUAL(*md5xb, date(2010, 1, 5));

	// Primary is format is used if possible
	DateParser const dp6("%y-%d-%m", "%y-%m-%d");
	optional<date> const md6a = dp6.parse("10-5-3");
	CHECK(md6a);
	if (md6a) CHECK_EQUAL(*md6a, date(2010, 3, 5));
	// But if not possible, secondary format is used.
	optional<date> const md6b = dp6.parse("10-12-14");
	CHECK(md6b);
	if (md6b) CHECK_EQUAL(*md6b, date(2010, 12, 14));
}

TEST(test_date_parser_tolerant_parse)
{
	DateParser const dp0("%d %m %y", "%d, %m, %y");
	optional<date> const md0a = dp0.parse("13 10 2013", true);
	CHECK(md0a);
	if (md0a) CHECK_EQUAL(*md0a, date(2013, 10, 13));
	optional<date> const md0b = dp0.parse("13 09 13", true);
	CHECK(md0b);
	if (md0b) CHECK_EQUAL(*md0b, date(2013, 9, 13));
	optional<date> const md0c = dp0.parse("13 9 13", true);
	CHECK(md0c);
	if (md0c) CHECK_EQUAL(*md0c, date(2013, 9, 13));
	optional<date> const md0d = dp0.parse("08 31 13", true);
	CHECK(!md0d);
	optional<date> const md0e = dp0.parse("13 09", true);
	CHECK(md0e);
	if (md0e) CHECK_EQUAL(*md0e, date(today().year(), 9, 13));
	optional<date> const md0f = dp0.parse("12 6 99", true);
	CHECK(md0f);
	if (md0f) CHECK_EQUAL(*md0f, date(1999, 6, 12));
	optional<date> const md0g = dp0.parse("13/10/2013", true);
	CHECK(!md0g);
	optional<date> const md0h = dp0.parse("0000030 010 00002013", true);
	CHECK(md0h);
	if (md0h) CHECK_EQUAL(*md0h, date(2013, 10, 30));
	optional<date> const md0i = dp0.parse("21", true);
	CHECK(md0i);
	if (md0i) CHECK_EQUAL(md0i, date(today().year(), today().month(), 21));

	DateParser const dp1("%d/%m/%y", "%d/%m/%y");
	optional<date> const md1a = dp1.parse("13/10/13", true);
	CHECK(md1a);
	if (md1a) CHECK_EQUAL(*md1a, date(2013, 10, 13));
	optional<date> const md1b = dp1.parse("13 10 13", true);
	CHECK(!md1b);

	DateParser const dp2("%Y .. %d !!! %m", "%Y .. %d !! %m");
	optional<date> const md2a = dp2.parse("3019 .. 4 !!! 1", true);
	CHECK(md2a);
	if (md2a) CHECK_EQUAL(*md2a, date(3019, 1, 4));
	optional<date> const md2b = dp2.parse("2019..4 !!! 1", true);
	CHECK(md2b);
	if (md2b) CHECK_EQUAL(*md2b, date(2019, 1, 4));
	optional<date> const md2c = dp2.parse("2019 4 1", true);
	CHECK (!md2c);

	DateParser const dp3("%Y %m %d", "%Y %m %d");
	optional<date> const md3a = dp3.parse("20190619", true);
	CHECK(md3a);
	if (md3a) CHECK_EQUAL(*md3a, date(2019, 6, 19));
	optional<date> const md3b = dp3.parse("2019/06/19", true);
	CHECK(!md3b);

	DateParser const dp4("%Y%m%d", "%Y%m%d");
	optional<date> const md4a = dp4.parse("20130619", true);
	CHECK(md4a);
	if (md4a) CHECK_EQUAL(*md4a, date(2013, 6, 19));
	optional<date> const md4b = dp4.parse("2013 06 19", true);
	CHECK(!md4b);
	optional<date> const md4c = dp4.parse("2013 619", true);
	CHECK(!md4c);

	DateParser const dp5("%m-%d-%y", "%m-%d-%y");
	optional<date> const md5a = dp5.parse("5-3-2013", true);
	CHECK(md5a);
	if (md5a) CHECK_EQUAL(*md5a, date(2013, 5, 3));
	optional<date> const md5b = dp5.parse("5-3", true);
	CHECK(md5b);
	if (md5b) CHECK_EQUAL(*md5b, date(today().year(), 5, 3));
	optional<date> const md5c = dp5.parse("3", true);
	CHECK(md5c);
	if (md5c) CHECK_EQUAL(*md5c, date(today().year(), today().month(), 3));
}


}  // namespace test
}  // namespace phatbooks
