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

namespace dcm
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
}  // namespace dcm

#endif  // GUARD_pl_account_entry_list_ctrl_hpp_025696059700891404
