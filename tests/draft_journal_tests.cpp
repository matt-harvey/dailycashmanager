// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account.hpp"
#include "draft_journal_handle.hpp"
#include "entry_handle.hpp"
#include "interval_type.hpp"
#include "phatbooks_tests_common.hpp"
#include "repeater_handle.hpp"
#include "frequency.hpp"
#include "transaction_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <UnitTest++/UnitTest++.h>
#include <wx/string.h>

namespace gregorian = boost::gregorian;

using jewel::Decimal;
using gregorian::date;
using sqloxx::Handle;

namespace phatbooks
{
namespace test
{

TEST_FIXTURE(TestFixture, test_draft_journal_repeater_description)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;
	DraftJournalHandle const dj1(dbc);
	dj1->set_transaction_type(TransactionType::generic);
	dj1->set_comment("draft journal to test repeater_description");
	dj1->set_name("test");

	EntryHandle entry1(dbc);
	entry1->set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "cash")));
	wxString const test_comment("test");
	entry1->set_comment(test_comment);
	entry1->set_amount(Decimal("0.00"));
	entry1->set_whether_reconciled(false);
	dj1->push_entry(entry1);

	wxString target = wxString("");
	CHECK_EQUAL(dj1->repeater_description(), "");

	RepeaterHandle const repeater1a(dbc);
	repeater1a->set_frequency(Frequency(1, IntervalType::months));
	repeater1a->set_next_date(date(2524, 9, 15));
	dj1->push_repeater(repeater1a);

	target = wxString
	(	"This transaction is automatically recorded every month, "
		"with the next recording due on 2524-Sep-15."
	);
	CHECK_EQUAL(dj1->repeater_description(), target);

	RepeaterHandle const repeater1b(dbc);
	repeater1b->set_frequency(Frequency(3, IntervalType::days));
	repeater1b->set_next_date(date(3950, 9, 12));
	dj1->push_repeater(repeater1b);

	target = wxString
	(	"This transaction is automatically recorded every month, "
		"with the next recording due on 2524-Sep-15.\n"
		"In addition, this transaction is automatically recorded every "
		"3 days, with the next recording due on 3950-Sep-12.\n"
		"This transaction will next be recorded on 2524-Sep-15."
	);
	CHECK_EQUAL(dj1->repeater_description(), target);
}

}  // namespace test
}  // namespace phatbooks
