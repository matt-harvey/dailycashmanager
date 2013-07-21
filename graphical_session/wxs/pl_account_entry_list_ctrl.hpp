#ifndef GUARD_pl_account_entry_list_ctrl_hpp
#define GUARD_pl_account_entry_list_ctrl_hpp

#include "account_entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/gdicmn.h>
#include <wx/window.h>

namespace phatbooks
{

// Begin forward declarations

class Entry;

// End forward declarations

namespace gui
{

/**
 * An AccountEntryListCtrl where the Account is of account_super_type::pl.
 */
class PLAccountEntryListCtrl: public AccountEntryListCtrl
{
public:
	PLAccountEntryListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		Account const& p_account,
		boost::optional<gregorian::date> const& p_maybe_min_date,
		boost::optional<gregorian::date> const& p_maybe_max_date
	);

	virtual ~PLAccountEntryListCtrl();

private:

	// TODO Could factor shared code up in hierarchy here.
	void set_non_date_columns(long p_row, Entry const& p_entry);

	virtual void do_insert_columns();

	// TODO Could factor shared code up in hierarchy here.
	virtual void do_push_entry(Entry const& p_entry);

	// TODO Could factor shared code up in hierarchy here.
	virtual void do_update_row_for_entry(long p_row, Entry const& p_entry);

	virtual int do_get_num_columns() const;

};  // class PLAccountEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_pl_account_entry_list_ctrl_hpp
