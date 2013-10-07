// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_filtered_entry_list_ctrl_hpp_790944654397175
#define GUARD_filtered_entry_list_ctrl_hpp_790944654397175

#include "account.hpp"
#include "entry_handle_fwd.hpp"
#include "entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <wx/window.h>
#include <memory>

namespace phatbooks
{
namespace gui
{

/**
 * An EntryListCtrl which is filtered by Account and may also be
 * filtered by date.
 *
 * Shows only \e actual (non-budget) and ordinary (non-draft) Entries.
 *
 * @todo HIGH PRIORITY Need to override
 * do_update_for_amended(sqloxx::Handle<Account> const& p_account).
 */
class FilteredEntryListCtrl: public EntryListCtrl
{
public:
	FilteredEntryListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		sqloxx::Handle<Account> const& p_account,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date,
		boost::optional<boost::gregorian::date> const& p_maybe_max_date
	);

	FilteredEntryListCtrl(FilteredEntryListCtrl const&) = delete;
	FilteredEntryListCtrl(FilteredEntryListCtrl&&) = delete;
	FilteredEntryListCtrl& operator=(FilteredEntryListCtrl const&) = delete;
	FilteredEntryListCtrl& operator=(FilteredEntryListCtrl&&) = delete;
	virtual ~FilteredEntryListCtrl();

protected:
	sqloxx::Handle<Account> const& account() const;
	boost::gregorian::date min_date() const;

private:
	virtual bool do_require_progress_log() const override;
	virtual void do_insert_non_date_columns() = 0;
	virtual bool do_approve_entry(EntryHandle const& p_entry) const override;
	virtual void do_set_column_widths() override;
	virtual int do_get_num_columns() const = 0;
	virtual int do_get_comment_col_num() const = 0;
	virtual std::unique_ptr<sqloxx::SQLStatement> do_create_entry_selector() override;
	
	sqloxx::Handle<Account> const m_account;
	boost::gregorian::date m_min_date;
	boost::optional<boost::gregorian::date> const m_maybe_max_date;

};  // class FilteredEntryListCtrl
	

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_filtered_entry_list_ctrl_hpp_790944654397175
