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

#ifndef GUARD_ordinary_journal_hpp_6580145273627781
#define GUARD_ordinary_journal_hpp_6580145273627781

#include "date.hpp"
#include "persistent_journal.hpp"
#include "dcm_database_connection.hpp"
#include "proto_journal.hpp"
#include "string_conv.hpp"
#include "transaction_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/persistence_traits.hpp>
#include <string>
#include <vector>

// Begin forward declarations

namespace dcm
{

class OrdinaryJournal;

}  // namespace dcm

// End forward declarations


// Template specialization

namespace sqloxx
{

template <>
struct PersistenceTraits<dcm::OrdinaryJournal>
{
    typedef dcm::PersistentJournal Base;
};

}  // namespace sqloxx




namespace dcm
{

/**
 * Represents an accounting journal that is, or will be, posted and
 * thereby reflected as a change in the economic state of the accounting
 * entity. The posting occurs when the \e save() method is called on the
 * OrdinaryJournal.
 */
class OrdinaryJournal: public PersistentJournal
{
public:

    static std::string exclusive_table_name();

    /**
     * Create the tables required for the persistence of
     * OrdinaryJournal instances in a SQLite database.
     */
    static void setup_tables(DcmDatabaseConnection& dbc);

    /**
     * Construct a "raw" OrdinaryJournal, that will not yet
     * correspond to any particular object in the database
     */
    OrdinaryJournal
    (   IdentityMap& p_identity_map,
        IdentityMap::Signature const& p_signature
    );

    /**
     * Get an OrdinaryJournal by id from the database.
     */
    OrdinaryJournal
    (   IdentityMap& p_identity_map,    
        sqloxx::Id p_id,
        IdentityMap::Signature const& p_signature
    );

    // copy constructor is private
    
    OrdinaryJournal(OrdinaryJournal&&) = delete;
    OrdinaryJournal& operator=(OrdinaryJournal const&) = delete;
    OrdinaryJournal& operator=(OrdinaryJournal&&) = delete;

    ~OrdinaryJournal() = default;

    /**
     * Can throw InvalidJournalDateException, if we attempt to
     * set to a prohibited date.
     */
    void set_date(boost::gregorian::date const& p_date);

    /**
     * Allows us to set to any date. Should not normally be used by
     * client code.
     */
    void set_date_unrestricted(boost::gregorian::date const& p_date);

    boost::gregorian::date date();

    /**
     * Take on the attributes \e rhs, where these exist and are
     * applicable to OrdinaryJournal; but do \e not take on the \e id
     * attribute of \e rhs, or the date.
     */
    void mimic(Journal& rhs);

private:

    void swap(OrdinaryJournal& rhs);

    /**
     * Copy constructor - implemented, but deliberately private.
     */
    OrdinaryJournal(OrdinaryJournal const& rhs);

    void do_load() override;
    void do_save_existing() override;
    void do_save_new() override;
    void do_ghostify() override;
    void do_remove() override;

    // Sole non-inherited data member. Note this is of a type where copying
    // does not throw. If we ever add more data members here and/or change
    // this one's type, it MAY be necessary to wrap this with pimpl to
    // to preserve exception-safe loading via copy-and-swap.
    boost::optional<DateRep> m_date;
};


/**
 * @returns a Handle to an OrdinaryJournal which, <em>if saved</em>,
 * will adjust the technical opening balance of p_account to become
 * p_desired_opening_balance. The journal will be an actual
 * (non-budget) journal if p_account is a balance sheet Account;
 * otherwise, if p_account is a P&L account, it will be
 * a budget journal.
 *
 * The OrdinaryJournal should not be saved unless and until
 * p_account has an id (i.e. is persisted to the database).
 *
 * Between creating the OrdinaryJournal from this
 * function, and saving it, there should be no
 * other adjustments made to the opening balances of
 * Accounts.
 *
 * Note the returned OrdinaryJournal will be ready to save immediately;
 * all of its attributes will be in a valid state. In particular, it
 * will be marked as \e reconciled.
 */
sqloxx::Handle<OrdinaryJournal> create_opening_balance_journal
(   sqloxx::Handle<Account> const& p_account,
    jewel::Decimal const& p_desired_opening_balance
);

            
                

}  // namespace dcm


#endif  // GUARD_ordinary_journal_hpp_6580145273627781
