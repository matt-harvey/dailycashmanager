#include "account.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "interval_type.hpp"
#include "phatbooks_tests_common.hpp"
#include "repeater.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/decimal.hpp>
#include <UnitTest++/UnitTest++.h>
#include <string>

namespace gregorian = boost::gregorian;

using jewel::Decimal;
using gregorian::date;
using std::string;

namespace phatbooks
{
namespace test
{

TEST_FIXTURE(TestFixture, test_draft_journal_repeater_description)
{
	DraftJournal dj1(dbc);
	dj1.set_whether_actual(true);
	dj1.set_comment("draft journal to test repeater_description");
	dj1.set_name("test");

	Entry entry1(dbc);
	entry1.set_account(Account(dbc, "cash"));
	entry1.set_comment("test");
	entry1.set_amount(Decimal("0.00"));
	entry1.set_whether_reconciled(false);
	dj1.add_entry(entry1);

	string target = "";
	CHECK_EQUAL(dj1.repeater_description(), "");

	Repeater repeater1a(dbc);
	repeater1a.set_interval_type(interval_type::months);
	repeater1a.set_interval_units(1);
	repeater1a.set_next_date(date(2012, 9, 15));
	dj1.add_repeater(repeater1a);

	target =
		"This transaction is automatically recorded every month, "
		"with the next recording due on 2012-Sep-15.";
	CHECK_EQUAL(dj1.repeater_description(), target);

	Repeater repeater1b(dbc);
	repeater1b.set_interval_type(interval_type::days);
	repeater1b.set_interval_units(3);
	repeater1b.set_next_date(date(2012, 9, 12));
	dj1.add_repeater(repeater1b);

	target =
		"This transaction is automatically recorded every month, "
		"with the next recording due on 2012-Sep-15.\n"
		"In addition, this transaction is automatically recorded every "
		"3 days, with the next recording due on 2012-Sep-12.\n"
		"This transaction will next be recorded on 2012-Sep-12.";
	CHECK_EQUAL(dj1.repeater_description(), target);
}

}  // namespace test
}  // namespace phatbooks
