// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_pl_account_entry_list_ctrl_hpp_025696059700891404
#define GUARD_pl_account_entry_list_ctrl_hpp_025696059700891404

#include "filtered_entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal_fwd.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/string.h>

namespace phatbooks
{

// begin forward declarations

class Account;
class Entry;

// end forward declarations

namespace gui
{

/**
 * A FilteredEntryListCtrl where the Account is of AccountSuperType::pl.
 */
class PLAccountEntryListCtrl: public FilteredEntryListCtrl
{
public:
	PLAccountEntryListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		sqloxx::Handle<Account> const& p_account,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date,
		boost::optional<boost::gregorian::date> const& p_maybe_max_date
	);

	PLAccountEntryListCtrl(PLAccountEntryListCtrl const&) = delete;
	PLAccountEntryListCtrl(PLAccountEntryListCtrl&&) = delete;
	PLAccountEntryListCtrl& operator=(PLAccountEntryListCtrl const&) = delete;
	PLAccountEntryListCtrl& operator=(PLAccountEntryListCtrl&&) = delete;
	virtual ~PLAccountEntryListCtrl();

private:

	virtual void do_set_non_date_columns
	(	long p_row,
		sqloxx::Handle<Entry> const& p_entry
	) override;

	virtual void do_insert_non_date_columns() override;

	virtual int do_get_comment_col_num() const override;

	virtual int do_get_num_columns() const override;

	wxString verb() const;

	jewel::Decimal friendly_amount
	(	sqloxx::Handle<Entry> const& p_entry
	) const;

	bool const m_reverse_signs;

};  // class PLAccountEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_pl_account_entry_list_ctrl_hpp_025696059700891404
