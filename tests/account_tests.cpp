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

#include "account.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_tests_common.hpp"
#include "visibility.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <UnitTest++/UnitTest++.h>
#include <wx/string.h>

using jewel::Decimal;
using sqloxx::DatabaseTransaction;
using sqloxx::Handle;
using sqloxx::Id;

namespace gregorian = boost::gregorian;

// TODO Put tests in here which exercise AmalgamatedBudget as well
// as just Account.

// TODO LOW PRIORITY There is a lot of repeated setup code in these tests
// which could be moved into phatbooks::test::TestFixture.

namespace phatbooks
{
namespace test
{

TEST_FIXTURE(TestFixture, test_account_exists)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;

	// These were saved to the database in TestFixture		
	CHECK(Account::exists(dbc, "cash"));
	CHECK(Account::exists(dbc, "cASh"));  // case-insensitive
	CHECK(Account::exists(dbc, "food"));
	CHECK(!Account::exists(dbc, "food "));  // but doesn't trim spaces

	// These are way off
	CHECK(!Account::exists(dbc, "f oo d"));
	CHECK(!Account::exists(dbc, "phasdfo"));

	// Some edge cases
	CHECK(!Account::exists(dbc, ""));
	CHECK(!Account::exists(dbc, "*"));

	// Create another Account
	Handle<Account> a0(dbc);
	a0->set_account_type(AccountType::expense);
	a0->set_name("Gardening Supplies");
	a0->set_commodity(Handle<Commodity>(dbc, 1));
	a0->set_description("stuff for like, gardening");
	a0->set_visibility(Visibility::hidden);

	CHECK(!Account::exists(dbc, "Gardening Supplies"));
	a0->save();
	CHECK(Account::exists(dbc, "Gardening Supplies"));
	CHECK(Account::exists(dbc, "gardening supplies"));
	a0->set_name("bleugh");
	CHECK(!Account::exists(dbc, "bleugh"));
	CHECK(Account::exists(dbc, "gardening supplies"));
	a0->save();
	CHECK(Account::exists(dbc, "bleugh"));
	CHECK(!Account::exists(dbc, "gardening supplies"));
	CHECK(Account::exists(dbc, "food"));
}

TEST_FIXTURE(TestFixture, test_no_user_pl_accounts_saved)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;

	// These were saved to the database in TestFixture		
	JEWEL_ASSERT(Account::exists(dbc, "cash"));
	JEWEL_ASSERT(Account::exists(dbc, "cASh"));  // case-insensitive
	JEWEL_ASSERT(Account::exists(dbc, "food"));
	JEWEL_ASSERT(!Account::exists(dbc, "food "));  // but doesn't trim spaces

	CHECK(!Account::no_user_pl_accounts_saved(dbc));
	Handle<Account> const a0(dbc, Account::id_for_name(dbc, "food"));
	a0->remove();
	CHECK(Account::no_user_pl_accounts_saved(dbc));
	Handle<Account> const a1(dbc);
	a1->set_account_type(AccountType::revenue);
	a1->set_name("salary");
	a1->set_commodity(Handle<Commodity>(dbc, 1));
	a1->set_description("salaries and wages received");
	a1->set_visibility(Visibility::hidden);
	CHECK(Account::no_user_pl_accounts_saved(dbc));
	a1->save();
	CHECK(!Account::no_user_pl_accounts_saved(dbc));
	a1->remove();
	CHECK(Account::no_user_pl_accounts_saved(dbc));

	Handle<Account> const a2(dbc);
	a2->set_account_type(AccountType::pure_envelope);
	a2->set_name("fund - general");
	a2->set_commodity(Handle<Commodity>(dbc, 1));
	a2->set_description("");
	a2->set_visibility(Visibility::visible);
	CHECK(Account::no_user_pl_accounts_saved(dbc));
	a2->save();
	CHECK(!Account::no_user_pl_accounts_saved(dbc));
	a2->set_account_type(AccountType::asset);
	CHECK(!Account::no_user_pl_accounts_saved(dbc));
	a2->save();
	CHECK(Account::no_user_pl_accounts_saved(dbc));
	a2->set_account_type(AccountType::liability);
	a2->save();
	CHECK(Account::no_user_pl_accounts_saved(dbc));
	a2->set_account_type(AccountType::equity);
	a2->save();
	CHECK(Account::no_user_pl_accounts_saved(dbc));
	a2->set_account_type(AccountType::expense);
	CHECK(Account::no_user_pl_accounts_saved(dbc));
	a2->save();
	CHECK(!Account::no_user_pl_accounts_saved(dbc));
}

TEST_FIXTURE(TestFixture, test_get_and_set_account_name)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Id const id1 = a1->id();
	Id const id2 = a2->id();
	Handle<Account> a3(dbc);

	CHECK_EQUAL(a1->name(), "cash");
	CHECK(a1->name() != "Cash");
	CHECK(a1->name() != "");
	CHECK_EQUAL(a2->name(), "food");

	a2->set_name("hello");
	CHECK_EQUAL(a2->name(), "hello");
	Handle<Account> const a1b(dbc, id1);
	Handle<Account> const a2b(dbc, id2);
	CHECK_EQUAL(a1b->name(), "cash");
	CHECK_EQUAL(a2b->name(), "hello");
	a2b->ghostify();
	CHECK_EQUAL(a2->name(), "food");
	
	a3->set_name("hello");
	CHECK_EQUAL(a3->name(), "hello");

	a1->set_name("yep");
	a1->save();
	CHECK_EQUAL(a1->name(), "yep");
}

TEST_FIXTURE(TestFixture, test_get_and_set_account_commodity)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;
	Handle<Commodity> const c1(dbc, Commodity::id_for_abbreviation(dbc, "AUD"));
	Handle<Commodity> const c2(dbc, Commodity::id_for_abbreviation(dbc, "USD"));
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> a3(dbc);
	Id const aid1 = a1->id();
	Id const aid2 = a2->id();
	Handle<Account> const a1b(dbc, aid1);
	Handle<Account> const a2b(dbc, aid2);

	CHECK(a1->commodity() == c1);
	CHECK(a2->commodity() == c1);
	a3->set_commodity(c1);
	CHECK(a3->commodity() == c1);

	a1->set_commodity(c2);
	CHECK(a1->commodity() == c2);
	CHECK(a2->commodity() == c1);
	a2->ghostify();
	CHECK(a2->commodity() == c1);
	a1->ghostify();
	CHECK(a1->commodity() == c1);
	a2->set_commodity(c2);
	CHECK(a1b->commodity() == a1->commodity());
	CHECK(a2b->commodity() == a2->commodity());
	a2->save();
	CHECK(a2->commodity() == c2);
	CHECK(a2->commodity() != c1);
	a3->set_commodity(c2);
	CHECK(a3->commodity() == c2);
	a3->set_commodity(c1);
	CHECK(a3->commodity() == c1);
	CHECK(a1b->commodity() == a1->commodity());
	CHECK(a2b->commodity() == a2->commodity());
}

TEST_FIXTURE(TestFixture, test_get_and_set_account_type)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> a3(dbc);
	Id const aid1 = a1->id();
	Id const aid2 = a2->id();
	Handle<Account> const a1b(dbc, aid1);
	Handle<Account> const a2b(dbc, aid2);

	CHECK(a1->account_type() == AccountType::asset);
	CHECK(a2->account_type() == AccountType::expense);
	a3->set_account_type(AccountType::liability);
	CHECK(a3->account_type() == AccountType::liability);

	a1->set_account_type(AccountType::revenue);
	CHECK(a1->account_type() == AccountType::revenue);
	CHECK(a2->account_type() == AccountType::expense);
	a2->ghostify();
	CHECK(a2->account_type() == AccountType::expense);
	a1->ghostify();
	CHECK(a1->account_type() == AccountType::asset);
	a2->set_account_type(AccountType::pure_envelope);
	CHECK(a1b->account_type() == a1->account_type());
	CHECK(a2b->account_type() == a2->account_type());
	a2->save();
	CHECK(a2->account_type() == AccountType::pure_envelope);
	CHECK(a2->account_type() != AccountType::expense);
	a3->set_account_type(AccountType::equity);
	CHECK(a3->account_type() == AccountType::equity);
	a3->set_account_type(AccountType::asset);
	CHECK(a3->account_type() == AccountType::asset);
	CHECK(a1b->account_type() == a1->account_type());
	CHECK(a2b->account_type() == a2->account_type());
}

TEST_FIXTURE(TestFixture, test_account_super_type)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> a3(dbc);
	Id const aid1 = a1->id();
	Id const aid2 = a2->id();
	Handle<Account> const a1b(dbc, aid1);
	Handle<Account> const a2b(dbc, aid2);

	CHECK(a1->account_super_type() == AccountSuperType::balance_sheet);
	CHECK(a2->account_super_type() == AccountSuperType::pl);
	a3->set_account_type(AccountType::liability);
	CHECK(a3->account_super_type() == AccountSuperType::balance_sheet);

	a1->set_account_type(AccountType::revenue);
	CHECK(a1->account_super_type() == AccountSuperType::pl);
	CHECK(a2->account_super_type() == AccountSuperType::pl);
	a2->ghostify();
	CHECK(a2->account_super_type() == AccountSuperType::pl);
	a1->ghostify();
	CHECK(a1->account_super_type() == AccountSuperType::balance_sheet);
	a2->set_account_type(AccountType::pure_envelope);
	CHECK(a1b->account_super_type() == a1->account_super_type());
	CHECK(a2b->account_super_type() == a2->account_super_type());
	a2->save();
	CHECK(a2->account_super_type() == AccountSuperType::pl);
	CHECK(a2->account_super_type() != AccountSuperType::balance_sheet);
	a3->set_account_type(AccountType::equity);
	CHECK(a3->account_super_type() == AccountSuperType::balance_sheet);
	a3->set_account_type(AccountType::asset);
	CHECK(a3->account_super_type() == AccountSuperType::balance_sheet);
}

TEST_FIXTURE(TestFixture, test_get_and_set_account_description)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> a3(dbc);
	Id const aid1 = a1->id();
	Id const aid2 = a2->id();
	Handle<Account> const a1b(dbc, aid1);
	Handle<Account> const a2b(dbc, aid2);

	CHECK_EQUAL(a1->description(), "notes and coins");
	CHECK_EQUAL(a2->description(), "food and drink");
	a3->set_description("puma food");
	CHECK_EQUAL(a3->description(), "puma food");

	a1->set_description("physical tokens of currency");
	CHECK_EQUAL(a1->description(), "physical tokens of currency");
	CHECK_EQUAL(a2->description(), "food and drink");
	a2->ghostify();
	CHECK_EQUAL(a2->description(), "food and drink");
	a1->ghostify();
	CHECK_EQUAL(a1->description(), "notes and coins");
	a2->set_description("comestibles");
	CHECK_EQUAL(a1b->description(), a1->description());
	CHECK_EQUAL(a2b->description(), a2->description());
	a2->save();
	CHECK_EQUAL(a2->description(), wxString("comestibles"));
	CHECK(a2->description() != wxString("Comestibles"));
	a3->set_description(wxString("xyz"));
	CHECK_EQUAL(a3->description(), wxString("xyz"));
	a3->set_description("");
	CHECK_EQUAL(a3->description(), "");
	CHECK_EQUAL(a1b->description(), a1->description());
	CHECK_EQUAL(a2b->description(), a2->description());
}

TEST_FIXTURE(TestFixture, test_get_and_set_account_visibility)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> const a3(dbc);
	Id const aid1 = a1->id();
	Id const aid2 = a2->id();
	Handle<Account> const a1b(dbc, aid1);
	Handle<Account> const a2b(dbc, aid2);

	CHECK(a1->visibility() == Visibility::visible);
	CHECK(a2->visibility() == Visibility::visible);
	a3->set_visibility(Visibility::hidden);
	CHECK(a3->visibility() == Visibility::hidden);

	a1->set_visibility(Visibility::hidden);
	CHECK(a1->visibility() == Visibility::hidden);
	CHECK(a2->visibility() == Visibility::visible);
	a2->ghostify();
	CHECK(a2->visibility() == Visibility::visible);
	a1->ghostify();
	CHECK(a1->visibility() == Visibility::visible);
	a2->set_visibility(Visibility::hidden);
	CHECK(a1b->visibility() == a1->visibility());
	CHECK(a2b->visibility() == a2->visibility());
	a2->save();
	CHECK(a2->visibility() == Visibility::hidden);
	CHECK(a2->visibility() != Visibility::visible);
	a3->set_visibility(Visibility::visible);
	CHECK(a3->visibility() == Visibility::visible);
}

TEST_FIXTURE(TestFixture, test_account_balance)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;
	Handle<Commodity> const c1(dbc, Commodity::id_for_abbreviation(dbc, "AUD"));
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	CHECK_EQUAL(a1->technical_balance(), Decimal("0.00"));
	CHECK_EQUAL(a1->friendly_balance(), Decimal("0.00"));
	CHECK_EQUAL(a2->technical_balance(), Decimal(0, 0));
	CHECK_EQUAL(a2->technical_balance(), Decimal(0, 2));
	CHECK_EQUAL(a2->technical_balance(), Decimal("0.00"));
	CHECK_EQUAL(a2->friendly_balance(), Decimal("0.00"));
	JEWEL_ASSERT (a1->commodity() == c1);
	JEWEL_ASSERT (a2->commodity() == c1);
	CHECK_EQUAL(a1->friendly_balance().places(), c1->precision());
	CHECK_EQUAL(a1->technical_balance().places(), c1->precision());
	CHECK_EQUAL(a2->friendly_balance().places(), c1->precision());
	CHECK_EQUAL(a2->technical_balance().places(), c1->precision());
	
	DatabaseTransaction dt(dbc);
	for (size_t i = 0; i != 10; ++i)
	{
		Handle<OrdinaryJournal> const oj0(dbc);
		oj0->set_transaction_type(TransactionType::expenditure);
		oj0->set_comment("");
		
		switch (i % 3)
		{
		case 0:
			oj0->set_date(today() + gregorian::date_duration(10));
			break;
		case 1:
			oj0->set_date(today() + gregorian::date_duration(306));
			break;
		case 2:
			oj0->set_date(today());
			break;
		default:
			JEWEL_HARD_ASSERT (false);
		}

		Handle<Entry> const e0(dbc);
		e0->set_account(a1);
		e0->set_comment("");
		e0->set_whether_reconciled(false);
		e0->set_amount(Decimal("-60.53"));
		e0->set_transaction_side
		(	i % 2?
			TransactionSide::destination:
			TransactionSide::source
		);
		oj0->push_entry(e0);

		Handle<Entry> const e1(dbc);
		e1->set_account(a2);
		e1->set_comment("");
		e1->set_whether_reconciled(false);
		e1->set_amount(Decimal("60.53"));
		e1->set_transaction_side
		(	i % 2?
			TransactionSide::source:
			TransactionSide::destination
		);
		oj0->push_entry(e1);

		JEWEL_ASSERT (oj0->is_balanced());
		oj0->save();
	}
	dt.commit();

	CHECK_EQUAL(a1->technical_balance(), Decimal("-605.30"));
	CHECK_EQUAL(a2->technical_balance(), Decimal("605.30"));
	CHECK_EQUAL(a1->friendly_balance(), Decimal("-605.30"));
	CHECK_EQUAL(a2->friendly_balance(), Decimal("-605.30"));
}

TEST_FIXTURE(TestFixture, test_account_budget)
{
	// TODO
}

TEST_FIXTURE(TestFixture, test_account_budget_items)
{
	// TODO
}

}  // namespace test
}  // namespace phatbooks
