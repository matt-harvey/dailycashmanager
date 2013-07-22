#ifndef GUARD_bs_account_entry_list_ctrl_hpp
#define GUARD_bs_account_entry_list_ctrl_hpp

#include "account_entry_list_ctrl.hpp"
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
 * An AccountEntryListCtrl where the Account is of
 * account_super_type::balance_sheet.
 */
class BSAccountEntryListCtrl: public AccountEntryListCtrl
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
	void set_non_date_columns(long p_row, Entry const& p_entry);
	
	virtual void do_insert_columns();
	virtual void do_push_entry(Entry const& p_entry);
	virtual void do_update_row_for_entry(long p_row, Entry const& p_entry);
	virtual int do_get_comment_col_num() const;
	virtual int do_get_num_columns() const;

};  // class BSAccountEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_bs_account_entry_list_ctrl_hpp
