/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_filtered_entry_list_ctrl_hpp_790944654397175
#define GUARD_filtered_entry_list_ctrl_hpp_790944654397175

#include "account.hpp"
#include "entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <wx/window.h>
#include <memory>

namespace phatbooks
{

// begin forward declarations

class Entry;

// end forward declarations

namespace gui
{

/**
 * An EntryListCtrl which is filtered by Account and may also be
 * filtered by date.
 *
 * Shows only \e actual (non-budget) and ordinary (non-draft) Entries.
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

	virtual bool do_approve_entry
	(	sqloxx::Handle<Entry> const& p_entry
	) const override;

	virtual void do_set_column_widths() override;

	virtual int do_get_num_columns() const = 0;

	virtual int do_get_comment_col_num() const = 0;

	virtual std::unique_ptr<sqloxx::SQLStatement>
		do_create_entry_selector() override;

	sqloxx::Handle<Account> const m_account;
	boost::gregorian::date m_min_date;
	boost::optional<boost::gregorian::date> const m_maybe_max_date;

};  // class FilteredEntryListCtrl
	

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_filtered_entry_list_ctrl_hpp_790944654397175
