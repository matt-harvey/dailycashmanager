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

#include "dcm_tests_common.hpp"
#include "account.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "proto_journal.hpp"
#include "dcm_exceptions.hpp"
#include "dcm_tests_common.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/test/unit_test.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <vector>

using boost::gregorian::date;
using jewel::Decimal;
using sqloxx::Handle;
using std::vector;

namespace dcm
{
namespace test
{

BOOST_FIXTURE_TEST_CASE(test_ordinary_journal_mimic, TestFixture)
{
    DcmDatabaseConnection& dbc = *pdbc;

    ProtoJournal journal1;
    journal1.set_transaction_type(TransactionType::generic);
    journal1.set_comment("igloo");

    Handle<Entry> entry1a(dbc);
    entry1a->
        set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "cash")));
    entry1a->set_comment("igloo entry a");
    entry1a->set_whether_reconciled(true);
    entry1a->set_amount(Decimal("0.99"));
    entry1a->set_transaction_side(TransactionSide::source);
    journal1.push_entry(entry1a);

    Handle<Entry> const entry1b(dbc);
    entry1b->
        set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "food")));
    entry1b->set_comment("igloo entry b");
    entry1b->set_whether_reconciled(false);
    entry1b->set_amount(Decimal("-0.99"));
    entry1b->set_transaction_side(TransactionSide::destination);
    journal1.push_entry(entry1b);
    Handle<OrdinaryJournal> const oj1(dbc);
    oj1->set_date(date(3000, 1, 5));
    oj1->mimic(journal1);
    BOOST_CHECK_EQUAL(oj1->date(), date(3000, 1, 5));
    BOOST_CHECK_EQUAL(oj1->is_actual(), true);
    BOOST_CHECK_EQUAL(oj1->comment(), "igloo");
    BOOST_CHECK_EQUAL(oj1->entries().size(), size_t(2));
    BOOST_CHECK
    (   oj1->transaction_type() ==
        TransactionType::generic
    );
    oj1->save();
    BOOST_CHECK(!oj1->entries().empty());

    for (Handle<Entry> const& entry: oj1->entries())
    {
        BOOST_CHECK(entry->id() == 1 || entry->id() == 2);
        if
        (   entry->account() ==
            Handle<Account>(dbc, Account::id_for_name(dbc, "cash"))
        )
        {
            BOOST_CHECK_EQUAL(entry->comment(), "igloo entry a");
            BOOST_CHECK_EQUAL(entry->amount(), Decimal("0.99"));
            BOOST_CHECK_EQUAL(entry->is_reconciled(), true);
            BOOST_CHECK(entry->transaction_side() == TransactionSide::source);
        }
        else
        {
            BOOST_CHECK
            (   entry->account() ==
                Handle<Account>(dbc, Account::id_for_name(dbc, "food"))
            );
            BOOST_CHECK_EQUAL(entry->is_reconciled(), false);
            BOOST_CHECK_EQUAL(entry->amount(), Decimal("-0.99"));
            BOOST_CHECK_EQUAL(entry->comment(), "igloo entry b");
            BOOST_CHECK
            (   entry->transaction_side() ==
                TransactionSide::destination
            );
        }
    }
    Handle<DraftJournal> const dj2(dbc);
    dj2->set_transaction_type(TransactionType::envelope);
    dj2->set_comment("steam engine");
    dj2->set_name("some journal");
    
    Handle<Entry> const entry2a(dbc);
    entry2a->set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "food")));
    entry2a->set_comment("steam");
    entry2a->set_amount(Decimal("0"));
    entry2a->set_whether_reconciled(false);
    entry2a->set_transaction_side(TransactionSide::source);
    dj2->push_entry(entry2a);
    
    oj1->mimic(*dj2);

    BOOST_CHECK_EQUAL(oj1->is_actual(), false);
    BOOST_CHECK_EQUAL(oj1->comment(), "steam engine");
    BOOST_CHECK_EQUAL(oj1->entries().size(), size_t(1));
    BOOST_CHECK_EQUAL(oj1->date(), date(3000, 1, 5));
    BOOST_CHECK
    (   oj1->transaction_type() ==
        TransactionType::envelope
    );
    oj1->save();
    for (Handle<Entry> const& entry: oj1->entries())
    {
        BOOST_CHECK_EQUAL
        (   entry->account()->id(),
            Handle<Account>(dbc, Account::id_for_name(dbc, "food"))->id()
        );
        BOOST_CHECK_EQUAL(entry->comment(), "steam");
        BOOST_CHECK_EQUAL(entry->is_reconciled(), false);
        BOOST_CHECK(entry->transaction_side() == TransactionSide::source);
    }
}

BOOST_FIXTURE_TEST_CASE(test_ordinary_journal_is_balanced, TestFixture)
{
    DcmDatabaseConnection& dbc = *pdbc;

    Handle<OrdinaryJournal> const journal1(dbc);
    journal1->set_transaction_type(TransactionType::generic);
    journal1->set_comment("igloo");

    Handle<Entry> const entry1a(dbc);
    entry1a->
        set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "cash")));
    entry1a->set_comment("igloo entry a");
    entry1a->set_whether_reconciled(true);
    entry1a->set_amount(Decimal("-10.99"));
    entry1a->set_transaction_side(TransactionSide::source);
    journal1->push_entry(entry1a);
    BOOST_CHECK_THROW(journal1->save(), UnbalancedJournalException);

    Handle<Entry> const entry1b(dbc);
    entry1b->
        set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "cash")));
    entry1b->set_comment("igloo entry b");
    entry1b->set_whether_reconciled(false);
    entry1b->set_amount(Decimal("50.09"));
    entry1b->set_transaction_side(TransactionSide::destination);
    journal1->push_entry(entry1b);
    journal1->set_date(date(3000, 1, 5));

    BOOST_CHECK(!journal1->is_balanced());
    BOOST_CHECK_THROW(journal1->save(), UnbalancedJournalException);
    entry1b->set_amount(Decimal("10.99"));
    BOOST_CHECK(journal1->is_balanced());

    journal1->save();

    // We already have a system journal (the budget instrument) so
    // we expect journal1b to have an id of 2, not 1.
    Handle<OrdinaryJournal> const journal1b(dbc, 2);

    BOOST_CHECK(journal1b->is_balanced());
    Handle<Entry> const entry1c(dbc);
    entry1c->
        set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "food")));
    entry1c->set_comment("Ummm");
    entry1c->set_whether_reconciled(true);
    entry1c->set_amount(Decimal(0, 0));
    entry1c->set_transaction_side(TransactionSide::destination);
    journal1b->push_entry(entry1c);
    BOOST_CHECK(journal1b->is_balanced());
    BOOST_CHECK(journal1->is_balanced());
    journal1b->save();
    
    entry1c->set_amount(Decimal("0.0000001"));
    BOOST_CHECK_EQUAL(journal1->is_balanced(), false);
    BOOST_CHECK(!journal1b->is_balanced());
}

BOOST_FIXTURE_TEST_CASE(test_ordinary_journal_remove_entry, TestFixture)
{
    DcmDatabaseConnection& dbc = *pdbc;
    Handle<OrdinaryJournal> const journal1(dbc);
    journal1->set_transaction_type(TransactionType::generic);
    journal1->set_comment("igloo");

    Handle<Entry> const entry1a(dbc);
    entry1a->
        set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "cash")));
    entry1a->set_comment("igloo entry a");
    entry1a->set_whether_reconciled(true);
    entry1a->set_amount(Decimal("-10.99"));
    entry1a->set_transaction_side(TransactionSide::source);
    journal1->push_entry(entry1a);

    Handle<Entry> const entry1b(dbc);
    entry1b->
        set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "cash")));
    entry1b->set_comment("igloo entry b");
    entry1b->set_whether_reconciled(false);
    entry1b->set_transaction_side(TransactionSide::destination);
    journal1->push_entry(entry1b);
    journal1->set_date(date(3000, 1, 5));
    entry1b->set_amount(Decimal("10.99"));

    JEWEL_ASSERT (journal1->is_balanced());
    journal1->save();

    Handle<OrdinaryJournal> const journal1b = journal1;
    BOOST_CHECK_EQUAL(journal1b->entries().size(), static_cast<size_t>(2));
    journal1b->remove_entry(entry1a);
    BOOST_CHECK_EQUAL(journal1->entries().size(), static_cast<size_t>(1));
    BOOST_CHECK_EQUAL(journal1b->entries().size(), static_cast<size_t>(1));
    journal1b->ghostify();
    BOOST_CHECK_EQUAL(journal1->entries().size(), static_cast<size_t>(2));
    BOOST_CHECK_EQUAL(journal1b->entries().size(), static_cast<size_t>(2));
    journal1b->remove_entry(entry1a);
    BOOST_CHECK(!journal1b->is_balanced());
    journal1b->entries().back()->set_amount(Decimal("0.00"));
    BOOST_CHECK(journal1b->is_balanced());
    journal1->save();
    BOOST_CHECK_EQUAL(journal1b->entries().size(), static_cast<size_t>(1));
    BOOST_CHECK_EQUAL(journal1->entries().size(), static_cast<size_t>(1));

    Handle<Entry> const entry1c(dbc);
    Handle<Entry> const entry1d(dbc); 
    journal1b->push_entry(entry1c);
    entry1b->set_comment("b");
    entry1c->set_comment("c");
    entry1d->set_comment("d");
    journal1b->push_entry(entry1d);
    BOOST_CHECK_EQUAL(journal1->entries().size(), static_cast<size_t>(3));
    journal1->remove_entry(entry1c);
    BOOST_CHECK_EQUAL(journal1->entries().size(), static_cast<size_t>(2));
    BOOST_CHECK_EQUAL(journal1->entries().at(0)->comment(), "b");
    BOOST_CHECK_EQUAL(journal1->entries().at(1)->comment(), "d");
}

}  // namespace test
}  // namespace dcm
