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

#include "gui/filtered_entry_list_ctrl.hpp"
#include "account.hpp"
#include "entry.hpp"
#include "gui/entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/sql_statement.hpp>
#include <wx/window.h>
#include <memory>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::Handle;
using sqloxx::SQLStatement;
using std::unique_ptr;

namespace gregorian = boost::gregorian;

namespace dcm
{
namespace gui
{

// Anonymous namespace
namespace
{
	bool lies_within
	(	gregorian::date const& p_target,
		gregorian::date const& p_min,
		optional<gregorian::date> const& p_max
	)
	{
		bool const ok_with_min = (p_target >= p_min);
		bool const ok_with_max = (!p_max || (p_target <= value(p_max)));
		return ok_with_min && ok_with_max;
	}

}  // End anonymous namespace

FilteredEntryListCtrl::FilteredEntryListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	Handle<Account> const& p_account,
	boost::optional<gregorian::date> const& p_maybe_min_date,
	boost::optional<gregorian::date> const& p_maybe_max_date
):
	EntryListCtrl(p_parent, p_size, p_account->database_connection()),
	m_account(p_account),
	m_min_date
	(	p_account->database_connection().opening_balance_journal_date() +
		gregorian::date_duration(1)
	),
	m_maybe_max_date(p_maybe_max_date)
{
	if (p_maybe_min_date)
	{
		gregorian::date const provided_min_date = value(p_maybe_min_date);
		if (provided_min_date > m_min_date)
		{
			m_min_date = provided_min_date;
		}
	}
}

FilteredEntryListCtrl::~FilteredEntryListCtrl()
{
}

Handle<Account> const&
FilteredEntryListCtrl::account() const
{
	return m_account;
}

gregorian::date
FilteredEntryListCtrl::min_date() const
{
	return m_min_date;
}

bool
FilteredEntryListCtrl::do_require_progress_log() const
{
	return false;
}

bool
FilteredEntryListCtrl::do_approve_entry(Handle<Entry> const& p_entry) const
{
	return
	(	(p_entry->account() == m_account) &&
		lies_within(p_entry->date(), m_min_date, m_maybe_max_date)
	);
	return true;
}

void
FilteredEntryListCtrl::do_set_column_widths()
{
	autosize_column_widths();
	adjust_comment_column_to_fit();
	return;
}

unique_ptr<SQLStatement>
FilteredEntryListCtrl::do_create_entry_selector()
{
	unique_ptr<SQLStatement> ret
	(	create_date_ordered_actual_ordinary_entry_selector
		(	database_connection(),
			m_min_date,
			m_maybe_max_date,
			m_account
		)
	);
	return ret;
}

}  // namespace gui
}  // namespace dcm
