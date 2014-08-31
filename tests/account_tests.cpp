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
#include "account_type.hpp"
#include "budget_item.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "ordinary_journal.hpp"
#include "dcm_database_connection.hpp"
#include "dcm_tests_common.hpp"
#include "visibility.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/test/unit_test.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <wx/string.h>
#include <vector>

using jewel::Decimal;
using sqloxx::DatabaseTransaction;
using sqloxx::Handle;
using sqloxx::Id;
using std::vector;

namespace gregorian = boost::gregorian;

// TODO LOW PRIORITY There is a lot of repeated setup code in these tests
// which could be moved into dcm::test::TestFixture.

namespace dcm
{
namespace test
{

BOOST_FIXTURE_TEST_CASE(test_account_exists, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;

	// These were saved to the database in TestFixture		
	BOOST_CHECK(Account::exists(dbc, "cash"));
	BOOST_CHECK(Account::exists(dbc, "cASh"));  // case-insensitive
	BOOST_CHECK(Account::exists(dbc, "food"));
	BOOST_CHECK(!Account::exists(dbc, "food "));  // but doesn't trim spaces

	// These are way off
	BOOST_CHECK(!Account::exists(dbc, "f oo d"));
	BOOST_CHECK(!Account::exists(dbc, "phasdfo"));

	// Some edge cases
	BOOST_CHECK(!Account::exists(dbc, ""));
	BOOST_CHECK(!Account::exists(dbc, "*"));

	// Create another Account
	Handle<Account> a0(dbc);
	a0->set_account_type(AccountType::expense);
	a0->set_name("Gardening Supplies");
	a0->set_commodity(Handle<Commodity>(dbc, 1));
	a0->set_description("stuff for like, gardening");
	a0->set_visibility(Visibility::hidden);

	BOOST_CHECK(!Account::exists(dbc, "Gardening Supplies"));
	a0->save();
	BOOST_CHECK(Account::exists(dbc, "Gardening Supplies"));
	BOOST_CHECK(Account::exists(dbc, "gardening supplies"));
	a0->set_name("bleugh");
	BOOST_CHECK(!Account::exists(dbc, "bleugh"));
	BOOST_CHECK(Account::exists(dbc, "gardening supplies"));
	a0->save();
	BOOST_CHECK(Account::exists(dbc, "bleugh"));
	BOOST_CHECK(!Account::exists(dbc, "gardening supplies"));
	BOOST_CHECK(Account::exists(dbc, "food"));
}

BOOST_FIXTURE_TEST_CASE(test_no_user_pl_accounts_saved, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;

	// These were saved to the database in TestFixture		
	JEWEL_ASSERT(Account::exists(dbc, "cash"));
	JEWEL_ASSERT(Account::exists(dbc, "cASh"));  // case-insensitive
	JEWEL_ASSERT(Account::exists(dbc, "food"));
	JEWEL_ASSERT(!Account::exists(dbc, "food "));  // but doesn't trim spaces

	BOOST_CHECK(!Account::no_user_pl_accounts_saved(dbc));
	Handle<Account> const a0(dbc, Account::id_for_name(dbc, "food"));
	a0->remove();
	BOOST_CHECK(Account::no_user_pl_accounts_saved(dbc));
	Handle<Account> const a1(dbc);
	a1->set_account_type(AccountType::revenue);
	a1->set_name("salary");
	a1->set_commodity(Handle<Commodity>(dbc, 1));
	a1->set_description("salaries and wages received");
	a1->set_visibility(Visibility::hidden);
	BOOST_CHECK(Account::no_user_pl_accounts_saved(dbc));
	a1->save();
	BOOST_CHECK(!Account::no_user_pl_accounts_saved(dbc));
	a1->remove();
	BOOST_CHECK(Account::no_user_pl_accounts_saved(dbc));

	Handle<Account> const a2(dbc);
	a2->set_account_type(AccountType::pure_envelope);
	a2->set_name("fund - general");
	a2->set_commodity(Handle<Commodity>(dbc, 1));
	a2->set_description("");
	a2->set_visibility(Visibility::visible);
	BOOST_CHECK(Account::no_user_pl_accounts_saved(dbc));
	a2->save();
	BOOST_CHECK(!Account::no_user_pl_accounts_saved(dbc));
	a2->set_account_type(AccountType::asset);
	BOOST_CHECK(!Account::no_user_pl_accounts_saved(dbc));
	a2->save();
	BOOST_CHECK(Account::no_user_pl_accounts_saved(dbc));
	a2->set_account_type(AccountType::liability);
	a2->save();
	BOOST_CHECK(Account::no_user_pl_accounts_saved(dbc));
	a2->set_account_type(AccountType::equity);
	a2->save();
	BOOST_CHECK(Account::no_user_pl_accounts_saved(dbc));
	a2->set_account_type(AccountType::expense);
	BOOST_CHECK(Account::no_user_pl_accounts_saved(dbc));
	a2->save();
	BOOST_CHECK(!Account::no_user_pl_accounts_saved(dbc));
}

BOOST_FIXTURE_TEST_CASE(test_get_and_set_account_name, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Id const id1 = a1->id();
	Id const id2 = a2->id();
	Handle<Account> a3(dbc);

	BOOST_CHECK_EQUAL(a1->name(), "cash");
	BOOST_CHECK_NE(a1->name(), "Cash");
	BOOST_CHECK_NE(a1->name(),  "");
	BOOST_CHECK_EQUAL(a2->name(), "food");

	a2->set_name("hello");
	BOOST_CHECK_EQUAL(a2->name(), "hello");
	Handle<Account> const a1b(dbc, id1);
	Handle<Account> const a2b(dbc, id2);
	BOOST_CHECK_EQUAL(a1b->name(), "cash");
	BOOST_CHECK_EQUAL(a2b->name(), "hello");
	a2b->ghostify();
	BOOST_CHECK_EQUAL(a2->name(), "food");
	
	a3->set_name("hello");
	BOOST_CHECK_EQUAL(a3->name(), "hello");

	a1->set_name("yep");
	a1->save();
	BOOST_CHECK_EQUAL(a1->name(), "yep");
}

BOOST_FIXTURE_TEST_CASE(test_get_and_set_account_commodity, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;
	Handle<Commodity> const c1(dbc, Commodity::id_for_abbreviation(dbc, "AUD"));
	Handle<Commodity> const c2(dbc, Commodity::id_for_abbreviation(dbc, "USD"));
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> a3(dbc);
	Id const aid1 = a1->id();
	Id const aid2 = a2->id();
	Handle<Account> const a1b(dbc, aid1);
	Handle<Account> const a2b(dbc, aid2);

	BOOST_CHECK(a1->commodity() == c1);
	BOOST_CHECK(a2->commodity() == c1);
	a3->set_commodity(c1);
	BOOST_CHECK(a3->commodity() == c1);

	a1->set_commodity(c2);
	BOOST_CHECK(a1->commodity() == c2);
	BOOST_CHECK(a2->commodity() == c1);
	a2->ghostify();
	BOOST_CHECK(a2->commodity() == c1);
	a1->ghostify();
	BOOST_CHECK(a1->commodity() == c1);
	a2->set_commodity(c2);
	BOOST_CHECK(a1b->commodity() == a1->commodity());
	BOOST_CHECK(a2b->commodity() == a2->commodity());
	a2->save();
	BOOST_CHECK(a2->commodity() == c2);
	BOOST_CHECK(a2->commodity() != c1);
	a3->set_commodity(c2);
	BOOST_CHECK(a3->commodity() == c2);
	a3->set_commodity(c1);
	BOOST_CHECK(a3->commodity() == c1);
	BOOST_CHECK(a1b->commodity() == a1->commodity());
	BOOST_CHECK(a2b->commodity() == a2->commodity());
}

BOOST_FIXTURE_TEST_CASE(test_get_and_set_account_type, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> a3(dbc);
	Id const aid1 = a1->id();
	Id const aid2 = a2->id();
	Handle<Account> const a1b(dbc, aid1);
	Handle<Account> const a2b(dbc, aid2);

	BOOST_CHECK(a1->account_type() == AccountType::asset);
	BOOST_CHECK(a2->account_type() == AccountType::expense);
	a3->set_account_type(AccountType::liability);
	BOOST_CHECK(a3->account_type() == AccountType::liability);

	a1->set_account_type(AccountType::revenue);
	BOOST_CHECK(a1->account_type() == AccountType::revenue);
	BOOST_CHECK(a2->account_type() == AccountType::expense);
	a2->ghostify();
	BOOST_CHECK(a2->account_type() == AccountType::expense);
	a1->ghostify();
	BOOST_CHECK(a1->account_type() == AccountType::asset);
	a2->set_account_type(AccountType::pure_envelope);
	BOOST_CHECK(a1b->account_type() == a1->account_type());
	BOOST_CHECK(a2b->account_type() == a2->account_type());
	a2->save();
	BOOST_CHECK(a2->account_type() == AccountType::pure_envelope);
	BOOST_CHECK(a2->account_type() != AccountType::expense);
	a3->set_account_type(AccountType::equity);
	BOOST_CHECK(a3->account_type() == AccountType::equity);
	a3->set_account_type(AccountType::asset);
	BOOST_CHECK(a3->account_type() == AccountType::asset);
	BOOST_CHECK(a1b->account_type() == a1->account_type());
	BOOST_CHECK(a2b->account_type() == a2->account_type());
}

BOOST_FIXTURE_TEST_CASE(test_account_super_type, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> a3(dbc);
	Id const aid1 = a1->id();
	Id const aid2 = a2->id();
	Handle<Account> const a1b(dbc, aid1);
	Handle<Account> const a2b(dbc, aid2);

	BOOST_CHECK(a1->account_super_type() == AccountSuperType::balance_sheet);
	BOOST_CHECK(a2->account_super_type() == AccountSuperType::pl);
	a3->set_account_type(AccountType::liability);
	BOOST_CHECK(a3->account_super_type() == AccountSuperType::balance_sheet);

	a1->set_account_type(AccountType::revenue);
	BOOST_CHECK(a1->account_super_type() == AccountSuperType::pl);
	BOOST_CHECK(a2->account_super_type() == AccountSuperType::pl);
	a2->ghostify();
	BOOST_CHECK(a2->account_super_type() == AccountSuperType::pl);
	a1->ghostify();
	BOOST_CHECK(a1->account_super_type() == AccountSuperType::balance_sheet);
	a2->set_account_type(AccountType::pure_envelope);
	BOOST_CHECK(a1b->account_super_type() == a1->account_super_type());
	BOOST_CHECK(a2b->account_super_type() == a2->account_super_type());
	a2->save();
	BOOST_CHECK(a2->account_super_type() == AccountSuperType::pl);
	BOOST_CHECK(a2->account_super_type() != AccountSuperType::balance_sheet);
	a3->set_account_type(AccountType::equity);
	BOOST_CHECK(a3->account_super_type() == AccountSuperType::balance_sheet);
	a3->set_account_type(AccountType::asset);
	BOOST_CHECK(a3->account_super_type() == AccountSuperType::balance_sheet);
}

BOOST_FIXTURE_TEST_CASE(test_get_and_set_account_description, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> a3(dbc);
	Id const aid1 = a1->id();
	Id const aid2 = a2->id();
	Handle<Account> const a1b(dbc, aid1);
	Handle<Account> const a2b(dbc, aid2);

	BOOST_CHECK_EQUAL(a1->description(), "notes and coins");
	BOOST_CHECK_EQUAL(a2->description(), "food and drink");
	a3->set_description("puma food");
	BOOST_CHECK_EQUAL(a3->description(), "puma food");

	a1->set_description("physical tokens of currency");
	BOOST_CHECK_EQUAL(a1->description(), "physical tokens of currency");
	BOOST_CHECK_EQUAL(a2->description(), "food and drink");
	a2->ghostify();
	BOOST_CHECK_EQUAL(a2->description(), "food and drink");
	a1->ghostify();
	BOOST_CHECK_EQUAL(a1->description(), "notes and coins");
	a2->set_description("comestibles");
	BOOST_CHECK_EQUAL(a1b->description(), a1->description());
	BOOST_CHECK_EQUAL(a2b->description(), a2->description());
	a2->save();
	BOOST_CHECK_EQUAL(a2->description(), wxString("comestibles"));
	BOOST_CHECK_NE(a2->description(), wxString("Comestibles"));
	a3->set_description(wxString("xyz"));
	BOOST_CHECK_EQUAL(a3->description(), wxString("xyz"));
	a3->set_description("");
	BOOST_CHECK_EQUAL(a3->description(), "");
	BOOST_CHECK_EQUAL(a1b->description(), a1->description());
	BOOST_CHECK_EQUAL(a2b->description(), a2->description());
}

BOOST_FIXTURE_TEST_CASE(test_get_and_set_account_visibility, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> const a3(dbc);
	Id const aid1 = a1->id();
	Id const aid2 = a2->id();
	Handle<Account> const a1b(dbc, aid1);
	Handle<Account> const a2b(dbc, aid2);

	BOOST_CHECK(a1->visibility() == Visibility::visible);
	BOOST_CHECK(a2->visibility() == Visibility::visible);
	a3->set_visibility(Visibility::hidden);
	BOOST_CHECK(a3->visibility() == Visibility::hidden);

	a1->set_visibility(Visibility::hidden);
	BOOST_CHECK(a1->visibility() == Visibility::hidden);
	BOOST_CHECK(a2->visibility() == Visibility::visible);
	a2->ghostify();
	BOOST_CHECK(a2->visibility() == Visibility::visible);
	a1->ghostify();
	BOOST_CHECK(a1->visibility() == Visibility::visible);
	a2->set_visibility(Visibility::hidden);
	BOOST_CHECK(a1b->visibility() == a1->visibility());
	BOOST_CHECK(a2b->visibility() == a2->visibility());
	a2->save();
	BOOST_CHECK(a2->visibility() == Visibility::hidden);
	BOOST_CHECK(a2->visibility() != Visibility::visible);
	a3->set_visibility(Visibility::visible);
	BOOST_CHECK(a3->visibility() == Visibility::visible);
}

BOOST_FIXTURE_TEST_CASE(test_account_balance_1, TestFixture)
{
	// Changing the balance of a small number of Accounts.

	DcmDatabaseConnection& dbc = *pdbc;
	Handle<Commodity> const c1(dbc, Commodity::id_for_abbreviation(dbc, "AUD"));
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	BOOST_CHECK_EQUAL(a1->technical_balance(), Decimal("0.00"));
	BOOST_CHECK_EQUAL(a1->friendly_balance(), Decimal("0.00"));
	BOOST_CHECK_EQUAL(a2->technical_balance(), Decimal(0, 0));
	BOOST_CHECK_EQUAL(a2->technical_balance(), Decimal(0, 2));
	BOOST_CHECK_EQUAL(a2->technical_balance(), Decimal("0.00"));
	BOOST_CHECK_EQUAL(a2->friendly_balance(), Decimal("0.00"));
	JEWEL_ASSERT (a1->commodity() == c1);
	JEWEL_ASSERT (a2->commodity() == c1);
	BOOST_CHECK_EQUAL(a1->friendly_balance().places(), c1->precision());
	BOOST_CHECK_EQUAL(a1->technical_balance().places(), c1->precision());
	BOOST_CHECK_EQUAL(a2->friendly_balance().places(), c1->precision());
	BOOST_CHECK_EQUAL(a2->technical_balance().places(), c1->precision());
	
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

	BOOST_CHECK_EQUAL(a1->technical_balance(), Decimal("-605.30"));
	BOOST_CHECK_EQUAL(a2->technical_balance(), Decimal("605.30"));
	BOOST_CHECK_EQUAL(a1->friendly_balance(), Decimal("-605.30"));
	BOOST_CHECK_EQUAL(a2->friendly_balance(), Decimal("-605.30"));

	Handle<OrdinaryJournal> const obj2 =
		create_opening_balance_journal(a2, Decimal("-100.53"));
	obj2->save();
	BOOST_CHECK_EQUAL(a2->technical_balance(), Decimal("504.77"));
	BOOST_CHECK_EQUAL(a2->friendly_balance(), Decimal("-504.77"));
}

BOOST_FIXTURE_TEST_CASE(test_account_balance_2, TestFixture)
{
	// Changing the balance of a large number of Accounts.
	
	DcmDatabaseConnection& dbc = *pdbc;
	Handle<Commodity> const aud(dbc, Commodity::id_for_abbreviation(dbc, "AUD"));
	Handle<Account> const cash(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const food(dbc, Account::id_for_name(dbc, "food"));

	Handle<Account> const credit_card(dbc);
	credit_card->set_account_type(AccountType::liability);
	credit_card->set_name("Credit card");
	credit_card->set_commodity(aud);
	credit_card->set_description("");
	credit_card->set_visibility(Visibility::visible);
	credit_card->save();

	Handle<Account> const chequing(dbc);
	chequing->set_account_type(AccountType::asset);
	chequing->set_name("Chequing account");
	chequing->set_commodity(aud);
	chequing->set_description("");
	chequing->set_visibility(Visibility::visible);
	chequing->save();

	Handle<Account> const household_supplies(dbc);
	household_supplies->set_account_type(AccountType::expense);
	household_supplies->set_name("Household supplies");
	household_supplies->set_commodity(aud);
	household_supplies->set_description("");
	household_supplies->set_visibility(Visibility::visible);
	household_supplies->save();

	Handle<Account> const salary(dbc);
	salary->set_account_type(AccountType::revenue);
	salary->set_name("Salary");
	salary->set_commodity(aud);
	salary->set_description("Salary and wages income");
	salary->set_visibility(Visibility::visible);
	salary->save();

	Handle<Account> const recreation(dbc);
	recreation->set_account_type(AccountType::expense);
	recreation->set_name("Recreation");
	recreation->set_commodity(aud);
	recreation->set_description("");
	recreation->set_visibility(Visibility::visible);
	recreation->save();

	Handle<OrdinaryJournal> const oj0(dbc);
	oj0->set_transaction_type(TransactionType::generic);
	oj0->set_comment("test");
	oj0->set_date(today() + gregorian::date_duration(100));
	Handle<Entry> const cash_entry(dbc);
	Handle<Entry> const food_entry(dbc);
	Handle<Entry> const credit_card_entry(dbc);
	Handle<Entry> const chequing_entry(dbc);
	Handle<Entry> const household_supplies_entry(dbc);
	Handle<Entry> const salary_entry(dbc);
	Handle<Entry> const recreation_entry(dbc);
	Handle<Entry> const entries[] =
	{	cash_entry,
		food_entry,
		credit_card_entry,
		chequing_entry,
		household_supplies_entry,
		salary_entry,
		recreation_entry
	};
	for (auto const& entry: entries)
	{
		entry->set_comment("");
		entry->set_whether_reconciled(false);
		entry->set_amount(Decimal("798.25"));
		entry->set_transaction_side(TransactionSide::destination);
	}
	cash_entry->set_account(cash);
	food_entry->set_account(food);
	credit_card_entry->set_account(credit_card);
	chequing_entry->set_account(chequing);
	household_supplies_entry->set_account(household_supplies);
	salary_entry->set_account(salary);
	recreation_entry->set_account(recreation);
	salary_entry->set_transaction_side(TransactionSide::source);
	salary_entry->set_amount(Decimal("-4789.50"));
	for (auto const& entry: entries)
	{
		oj0->push_entry(entry);
	}
	oj0->save();

	BOOST_CHECK_EQUAL(cash->friendly_balance(), Decimal("798.25"));
	BOOST_CHECK_EQUAL(food->friendly_balance(), Decimal("-798.25"));
	BOOST_CHECK_EQUAL(credit_card->technical_balance(), Decimal("798.25"));
	BOOST_CHECK_EQUAL(chequing->friendly_balance(), Decimal("798.25"));
	BOOST_CHECK_EQUAL(household_supplies->technical_balance(), Decimal("798.25"));
	BOOST_CHECK_EQUAL(salary->friendly_balance(), Decimal("4789.50"));
	BOOST_CHECK_EQUAL(recreation->friendly_balance(), Decimal("-798.25"));

	Handle<OrdinaryJournal> const oj1(dbc);
	oj1->mimic(*oj0);
	oj1->set_date(today() + gregorian::date_duration(129));
	oj1->save();

	BOOST_CHECK_EQUAL(cash->friendly_balance(), Decimal("1596.50"));
	BOOST_CHECK_EQUAL(food->friendly_balance(), Decimal("-1596.50"));
	BOOST_CHECK_EQUAL(credit_card->friendly_balance(), Decimal("1596.50"));
	BOOST_CHECK_EQUAL(chequing->friendly_balance(), Decimal("1596.50"));
	BOOST_CHECK_EQUAL(household_supplies->friendly_balance(), Decimal("-1596.50"));
	BOOST_CHECK_EQUAL(salary->technical_balance(), Decimal("-9579.00"));
	BOOST_CHECK_EQUAL(recreation->technical_balance(), Decimal("1596.50"));
}

BOOST_FIXTURE_TEST_CASE(test_account_opening_balance, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;
	Handle<Commodity> const c1(dbc, Commodity::id_for_abbreviation(dbc, "AUD"));
	Handle<Account> const a1(dbc, Account::id_for_name(dbc, "cash"));
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	BOOST_CHECK_EQUAL(a1->technical_opening_balance(), Decimal("0.00"));
	BOOST_CHECK_EQUAL(a1->friendly_opening_balance(), Decimal("0.00"));
	BOOST_CHECK_EQUAL(a2->technical_opening_balance(), Decimal(0, 0));
	BOOST_CHECK_EQUAL(a2->technical_opening_balance(), Decimal(0, 2));
	BOOST_CHECK_EQUAL(a2->technical_opening_balance(), Decimal("0.00"));
	BOOST_CHECK_EQUAL(a2->friendly_opening_balance(), Decimal("0.00"));
	JEWEL_ASSERT (a1->commodity() == c1);
	JEWEL_ASSERT (a2->commodity() == c1);
	BOOST_CHECK_EQUAL(a1->friendly_opening_balance().places(), c1->precision());
	BOOST_CHECK_EQUAL(a1->technical_opening_balance().places(), c1->precision());
	BOOST_CHECK_EQUAL(a2->friendly_opening_balance().places(), c1->precision());
	BOOST_CHECK_EQUAL(a2->technical_opening_balance().places(), c1->precision());

	Handle<OrdinaryJournal> const obj1 =
		create_opening_balance_journal(a1, Decimal("306.90"));
	obj1->save();
	BOOST_CHECK_EQUAL(a1->technical_opening_balance(), Decimal("306.90"));
	BOOST_CHECK_EQUAL(a1->friendly_opening_balance(), Decimal("306.90"));
	Handle<OrdinaryJournal> const obj2 =
		create_opening_balance_journal(a2, Decimal("-50.00"));
	BOOST_CHECK_EQUAL(a2->technical_opening_balance(), Decimal("0.00"));
	BOOST_CHECK_EQUAL(a2->friendly_opening_balance(), Decimal("0.00"));
	obj2->save();
	BOOST_CHECK_EQUAL(a2->technical_opening_balance(), Decimal("-50.00"));
	BOOST_CHECK_EQUAL(a2->friendly_opening_balance(), Decimal("50.00"));

	Handle<OrdinaryJournal> const obj1b =
		create_opening_balance_journal(a1, Decimal("7.01"));
	BOOST_CHECK_EQUAL(a1->technical_opening_balance(), Decimal("306.90"));
	BOOST_CHECK_EQUAL(a1->friendly_opening_balance(), Decimal("306.90"));
	obj1b->save();
	BOOST_CHECK_EQUAL(a1->technical_opening_balance(), Decimal("7.01"));
	BOOST_CHECK_EQUAL(a1->friendly_opening_balance(), Decimal("7.01"));
}

BOOST_FIXTURE_TEST_CASE(test_account_budget_and_budget_items, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;
	Handle<Account> const a2(dbc, Account::id_for_name(dbc, "food"));
	Handle<Account> const a3(dbc);
	a3->set_account_type(AccountType::revenue);
	a3->set_name("Salary");
	a3->set_commodity(dbc.default_commodity());
	a3->set_description("");
	a3->set_visibility(Visibility::visible);
	a3->save();
	JEWEL_ASSERT (dbc.budget_frequency() == Frequency(1, IntervalType::days));
	BOOST_CHECK_EQUAL(a3->budget(), Decimal("0.00"));
	BOOST_CHECK_EQUAL(a2->budget(), Decimal("0.00"));

	vector<Handle<BudgetItem> > bis1;
	BOOST_CHECK(a3->budget_items() == bis1);

	Handle<BudgetItem> const b1(dbc);
	b1->set_description("");
	b1->set_account(a3);
	b1->set_frequency(Frequency(1, IntervalType::weeks));
	b1->set_amount(Decimal("7.14"));
	BOOST_CHECK_EQUAL(a3->budget(), Decimal("0.00"));
	b1->save();
	bis1.push_back(b1);
	BOOST_CHECK_EQUAL(a3->budget(), Decimal("1.02"));

	Handle<BudgetItem> const b1b(dbc);
	b1b->set_description("");
	b1b->set_account(a3);
	b1b->set_frequency(Frequency(12, IntervalType::months));
	b1b->set_amount(Decimal("365.25"));
	b1b->save();
	bis1.push_back(b1b);
	BOOST_CHECK_EQUAL(a3->budget(), Decimal("2.02"));
	BOOST_CHECK(a3->budget_items() == bis1);

	b1->remove();
	BOOST_CHECK_EQUAL(a3->budget(), Decimal("1.00"));

	bis1.clear();
	bis1.push_back(b1b);
	BOOST_CHECK(a3->budget_items() == bis1);

	Handle<BudgetItem> const b2(dbc);
	b2->set_description("asdfj");
	b2->set_account(a2);
	b2->set_frequency(Frequency(3, IntervalType::months));
	b2->set_amount(Decimal("32.35"));
	b2->save();
	BOOST_CHECK_EQUAL(a2->budget(), Decimal("0.35"));
	b1b->set_account(a2);
	b1b->save();
	BOOST_CHECK_EQUAL(a3->budget(), Decimal("0.00"));
	BOOST_CHECK_EQUAL(a2->budget(), Decimal("1.35"));
}

}  // namespace test
}  // namespace dcm
