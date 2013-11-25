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

#include "account.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "interval_type.hpp"
#include "dcm_tests_common.hpp"
#include "frequency.hpp"
#include "repeater.hpp"
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

namespace dcm
{
namespace test
{

TEST_FIXTURE(TestFixture, test_draft_journal_repeater_description)
{
	DcmDatabaseConnection& dbc = *pdbc;
	Handle<DraftJournal> const dj1(dbc);
	dj1->set_transaction_type(TransactionType::generic);
	dj1->set_comment("draft journal to test repeater_description");
	dj1->set_name("test");

	Handle<Entry> const entry1(dbc);
	entry1->
		set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "cash")));
	wxString const test_comment("test");
	entry1->set_comment(test_comment);
	entry1->set_amount(Decimal("0.00"));
	entry1->set_whether_reconciled(false);
	dj1->push_entry(entry1);

	wxString target = wxString("");
	CHECK_EQUAL(dj1->repeater_description(), "");

	Handle<Repeater> const repeater1a(dbc);
	repeater1a->set_frequency(Frequency(1, IntervalType::months));
	repeater1a->set_next_date(date(2524, 9, 15));
	dj1->push_repeater(repeater1a);

	target = wxString
	(	"This transaction is automatically recorded every month, "
		"with the next recording due on 2524-Sep-15."
	);
	CHECK_EQUAL(dj1->repeater_description(), target);

	Handle<Repeater> const repeater1b(dbc);
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
}  // namespace dcm
