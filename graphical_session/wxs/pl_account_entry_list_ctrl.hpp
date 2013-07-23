#ifndef GUARD_pl_account_entry_list_ctrl_hpp
#define GUARD_pl_account_entry_list_ctrl_hpp

#include "filtered_entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal_fwd.hpp>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/string.h>

namespace phatbooks
{

// Begin forward declarations

class Entry;

// End forward declarations

namespace gui
{

/**
 * A FilteredEntryListCtrl where the Account is of account_super_type::pl.
 */
class PLAccountEntryListCtrl: public FilteredEntryListCtrl
{
public:
	PLAccountEntryListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		Account const& p_account,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date,
		boost::optional<boost::gregorian::date> const& p_maybe_max_date
	);

	virtual ~PLAccountEntryListCtrl();

private:

	virtual void do_set_non_date_columns(long p_row, Entry const& p_entry);
	virtual void do_insert_non_date_columns();
	virtual int do_get_comment_col_num() const;
	virtual int do_get_num_columns() const;

	wxString verb() const;
	jewel::Decimal friendly_amount(Entry const& p_entry) const;

	bool const m_reverse_signs;

};  // class PLAccountEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_pl_account_entry_list_ctrl_hpp
