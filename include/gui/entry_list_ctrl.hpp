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

#ifndef GUARD_entry_list_ctrl_hpp_03525603377970682
#define GUARD_entry_list_ctrl_hpp_03525603377970682

#include "entry_table_iterator.hpp"
#include "reconciliation_list_panel.hpp"
#include "summary_datum.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/listctrl.h>
#include <memory>
#include <unordered_set>
#include <vector>

namespace dcm
{

// Begin forward declarations

class Account;
class Entry;
class DateParser;
class OrdinaryJournal;
class DcmDatabaseConnection;

namespace gui
{

class ReconciliationEntryListCtrl;

// End forward declarations

/**
 * Displays a list of Entries. Derived classes must define various
 * pure virtual functions - and may override certain impure virtual functions
 * - to govern which Entries are displayed, and which columns of data are
 * displayed. However every EntryListCtrl will have the date of the Entry
 * showing in the first column, and should only ever show Entries from
 * OrdinaryJournals (not DraftJournals).
 */
class EntryListCtrl: public wxListCtrl
{
public:

    /**
     * @returns a pointer to a heap-allocated EntryListCtrl, listing
     * all and only the \e actual (non-budget) OrdinaryEntries stored in
     * \e p_account.database_connection() which have \e p_account as their
     * Account, and which lie between the date stored in p_maybe_min_date
     * and the date stored in p_maybe_max_date, inclusive. If
     * p_maybe_min_date is an uninitialized optional, then there is no
     * minimum date; and if p_maybe_max_date is an uninitialized optional
     * then there is no maximum date. Having said this, even if there is no
     * minimum date, then the opening balance date may act as an effective
     * minimum date...
     *
     * Caller has responsibility for managing the pointed-to memory.
     */
    static EntryListCtrl* create_actual_ordinary_entry_list
    (   wxWindow* p_parent,
        wxSize const& p_size,
        sqloxx::Handle<Account> const& p_account,
        boost::optional<boost::gregorian::date> const& p_maybe_min_date =
            boost::optional<boost::gregorian::date>(),
        boost::optional<boost::gregorian::date> const& p_maybe_max_date =
            boost::optional<boost::gregorian::date>()
    );

    /**
     * @returns a pointer to a heap-allocated EntryListCtrl, especially
     * designed to facilitate balance sheet Account reconciliations.
     *
     * \e p_account should be handle to a balance sheet Account.
     *
     * Caller has responsibility for managing the pointed-to memory.
     */
    static ReconciliationEntryListCtrl* create_reconciliation_entry_list
    (   ReconciliationListPanel* p_parent,
        wxSize const& p_size,
        sqloxx::Handle<Account> const& p_account,
        boost::gregorian::date const& p_min_date,
        boost::gregorian::date const& p_max_date
    );

    EntryListCtrl(EntryListCtrl const&) = delete;
    EntryListCtrl(EntryListCtrl&&) = delete;
    EntryListCtrl& operator=(EntryListCtrl const&) = delete;
    EntryListCtrl& operator=(EntryListCtrl&&) = delete;
    virtual ~EntryListCtrl();

    /**
     * Update displayed entries to reflect that \e p_journal has been newly
     * posted (having not previously existed in the database).
     */
    void update_for_new(sqloxx::Handle<OrdinaryJournal> const& p_journal);

    /**
     * Update displayed entries to reflect that \e p_account has been newly
     * created and saved (having not previously existed in the database.
     */
    void update_for_new(sqloxx::Handle<Account> const& p_account);

    /**
     * Update displayed entries to reflect that an already-saved
     * OrdinaryJournal \e p_journal has just been amended, and the amendments
     * saved.
     */
    void update_for_amended(sqloxx::Handle<OrdinaryJournal> const& p_journal);

    /**
     * Update displayed entries to reflect that an already-saved Account \e
     * p_account has just been amended, and the amendments saved.
     */
    void update_for_amended(sqloxx::Handle<Account> const& p_account);

    /**
     * Update displayed entries to reflect that the Entries with IDs
     * (as in Entry::id()) in p_doomed_ids have been deleted from the
     * database.
     */
    void update_for_deleted(std::vector<sqloxx::Id> const& p_doomed_ids);

    /**
     * @returns a vector populated with handles to the currently selected
     * Entries (if any).
     */
    std::vector<sqloxx::Handle<Entry> > selected_entries();

    /**
     * Scroll to the bottom of the displayed list.
     */
    void scroll_to_bottom();

    /**
     * @returns a reference to a vector of SummaryDatum instances representing
     * a summary of the information embodied in the EntryListCtrl.
     */
    std::vector<SummaryDatum> const& summary_data() const;

protected:
    EntryListCtrl
    (   wxWindow* p_parent,
        wxSize const& p_size,
        DcmDatabaseConnection& p_database_connection
    );

    /**
     * @returns the number of columns in the displayed list.
     */
    int num_columns() const;

    /**
     * @returns the (0-based) index of the column containing the date.
     */
    int date_col_num() const;

    /**
     * Sets column widths to a reasonable default size.
     */
    void autosize_column_widths();

    /**
     * Resizes the column in which Entry comments are displayed, so that, as far
     * as possible, there is enough room for all the displayed contents as
     * well as for a scrollbar. The comment column is shrunk, in preference to
     * shrinking other columns, if required to fit all the columns in the
     * visible area.
     */
    void adjust_comment_column_to_fit();

    /**
     * @returns the date displayed in the row indexed by \e p_row, using \e
     * p_parser to parse the text of the date.
     */
    boost::gregorian::date date_displayed
    (   long p_row,
        DateParser const& p_parser
    ) const;

    /**
     * \e Assuming the displayed Entries are already ordered in increasing order
     * of date, returns the row index such that, if a new Entry dated \e p_date
     * were to be inserted so as to preserve this order (while ordering
     * equal-dated Entries from oldest to newest) then it would be inserted into
     * a new row indexed with this index.
     */
    long row_for_date(boost::gregorian::date const& p_date) const;

    DcmDatabaseConnection& database_connection();

    DcmDatabaseConnection const& database_connection() const;

private:

    /**
     * Inheriting class should define to return \e true if and only if
     * progress log should be displayed to user.
     */
    virtual bool do_require_progress_log() const = 0;

    /**
     * Inheriting class should implement to insert columns (using InsertColumn
     * method of wxListCtrl), other than the date column.
     */
    virtual void do_insert_non_date_columns() = 0;

    /**
     * Inheriting class should implement to inspect an arbitrary
     * sqloxx::Handle<Entry> \e p_entry and return \e true if and only if that
     * Entry should be included in the displayed list.
     */
    virtual bool do_approve_entry
    (   sqloxx::Handle<Entry> const& p_entry
    ) const = 0;

    /**
     * Inheriting class should implement to set the value for each of the
     * non-date columns in arbitrary row indexed by \e p_row, to reflect the
     * Entry represented by \e p_entry. (An implementation would generally use
     * wxListCtrl::SetItem to do this.)
     */
    virtual void do_set_non_date_columns
    (   long p_row,
        sqloxx::Handle<Entry> const& p_entry
    ) = 0;

    /**
     * Inheriting class should implement to set widths of all its columns.
     */
    virtual void do_set_column_widths() = 0;

    /**
     * Inheriting class should implement to return number of columns.
     */
    virtual int do_get_num_columns() const = 0;

    /**
     * Inheriting class should implement to return the index of the column
     * that contains the comment of each Entry (can return -1 if there is no
     * such column).
     */
    virtual int do_get_comment_col_num() const = 0;

    /**
     * Inheriting class should implement this so as to return a std::unique_ptr
     * to a heap-allocated sqloxx::SQLStatement which is a "SELECT" SQL
     * statement that selects just the primary key column from the database
     * table in which Entry instances are stored.
     */
    virtual std::unique_ptr<sqloxx::SQLStatement>
        do_create_entry_selector() = 0;

    /**
     * This is called to update the displayed list to reflect that the Account
     * handled by \e p_account has been edited and the edits saved. By default
     * this does nothing. Inheriting class is free to override.
     */
    virtual void do_update_for_amended
    (   sqloxx::Handle<Account> const& p_account
    );

    /**
     * @returns a vector of SummaryDatum instances, representing a summary
     * of the contents of the displayed list. By default this returns
     * an empty vector. Inheriting classes are free to override to return
     * meaningful data.
     */
    virtual std::vector<SummaryDatum> const& do_get_summary_data() const;

    /**
     * Called during initialization of the EntryListCtrl, to perform any
     * actions required to initialize the vector of SummaryData that
     * should be returned via summary_data(). This is called prior
     * to populating the list with actual data for the Entries. By default,
     * this does nothing. Inheriting classes are free to override this function.
     */
    virtual void do_initialize_summary_data();

    /**
     * Upon an Entry being added to the displayed list, this function is
     * called to reflect the Entry, as appropriate, within the data that
     * should be returned via summary_data(). By default this does nothing.
     * Inheriting classes are free to override this function.
     */
    virtual void do_process_candidate_entry_for_summary
    (   sqloxx::Handle<Entry> const& p_entry
    );

    /**
     * Upon an Entry being removed from the displayed list, this function is
     * called to reflect the removal of the Entry, as appropriate, from the
     * data that should be returned via summary_data(). By default this
     * does nothing. Inheriting classes are free to override this function.
     */
    virtual void do_process_removal_for_summary(long p_row);

    void on_item_activated(wxListEvent& event);

    void set_column_widths();
    int scrollbar_width_allowance() const;

     // To be called by factory functions prior to returning pointer to newly
     // created EntryListCtrl.
    static void initialize(EntryListCtrl* p_entry_list_ctrl);

    void insert_columns();
    void insert_date_column();
    void populate();
    void process_push_candidate_entry(sqloxx::Handle<Entry> const& p_entry);

    // If p_row is set to -1 (as it is by default) and the candidate Entry
    // is "approved", then the row it will be inserted into will be determined
    // automatically; otherwise, the row will be given by p_row.
    void process_insertion_candidate_entry
    (   sqloxx::Handle<Entry> const& p_entry,
        long p_row = -1
    );

    // This inserts in correct date order (if p_row is -1) or at an explicitly
    // specified row (if p_row is non-negative). If p_row is less than -1,
    // then behaviour is undefined.
    void insert_entry(sqloxx::Handle<Entry> const& p_entry, long p_row = -1);
    
    void remove_if_present(sqloxx::Id p_entry_id);

    // This doesn't take care of sorting by date
    void push_back_entry(sqloxx::Handle<Entry> const& p_entry);

    // To remember which Entries have been added.
    typedef std::unordered_set<sqloxx::Id> IdSet;
    IdSet m_id_set;

    DcmDatabaseConnection& m_database_connection;

    DECLARE_EVENT_TABLE()

};  // class EntryListCtrl






}  // namespace gui
}  // namespace dcm

#endif  // GUARD_entry_list_ctrl_hpp_03525603377970682
