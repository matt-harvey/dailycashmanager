// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_bs_account_entry_list_ctrl_hpp_5331693680645755
#define GUARD_bs_account_entry_list_ctrl_hpp_5331693680645755

#include "filtered_entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/gdicmn.h>
#include <wx/window.h>

namespace phatbooks
{

// Begin forward declarations

class Account;
class Entry;

// End forward declarations

namespace gui
{

/**
 * A FilteredEntryListCtrl where the Account is of
 * account_super_type::balance_sheet.
 */
class BSAccountEntryListCtrl: public FilteredEntryListCtrl
{
public:
	BSAccountEntryListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		Account const& p_account,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date,
		boost::optional<boost::gregorian::date> const& p_maybe_max_date
	);

	virtual ~BSAccountEntryListCtrl();

private:
	virtual void do_set_non_date_columns(long p_row, Entry const& p_entry);
	virtual void do_insert_non_date_columns();
	virtual int do_get_comment_col_num() const;
	virtual int do_get_num_columns() const;

};  // class BSAccountEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_bs_account_entry_list_ctrl_hpp_5331693680645755
