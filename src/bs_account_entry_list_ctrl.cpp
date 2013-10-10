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


#include "gui/bs_account_entry_list_ctrl.hpp"
#include "account.hpp"
#include "entry.hpp"
#include "gui/filtered_entry_list_ctrl.hpp"
#include "gui/locale.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>

using boost::optional;
using jewel::value;
using sqloxx::Handle;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

namespace
{
	int comment_col_num()
	{
		return 1;
	}
	int amount_col_num()
	{
		return 2;
	}
	int anon_num_columns()
	{
		return 3;
	}

}  // end anonymous namespace


BSAccountEntryListCtrl::BSAccountEntryListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	Handle<Account> const& p_account,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date
):
	FilteredEntryListCtrl
	(	p_parent,
		p_size,
		p_account,
		p_maybe_min_date,
		p_maybe_max_date
	)
{
}

void
BSAccountEntryListCtrl::do_set_non_date_columns
(	long p_row,
	Handle<Entry> const& p_entry
)
{
	SetItem
	(	p_row,
		comment_col_num(),
		p_entry->comment()
	);
	SetItem
	(	p_row,
		amount_col_num(),
		finformat_wx
		(	p_entry->amount(),
			locale(),
			DecimalFormatFlags().clear(string_flags::dash_for_zero)
		)
	);
	JEWEL_ASSERT (num_columns() == 3);
	return;
}

void
BSAccountEntryListCtrl::do_insert_non_date_columns()
{
	InsertColumn
	(	comment_col_num(),
		wxString("Memo"),
		wxLIST_FORMAT_LEFT
	);
	InsertColumn
	(	amount_col_num(),
		wxString("Amount"),
		wxLIST_FORMAT_RIGHT
	);
	JEWEL_ASSERT (num_columns() == 3);
	return;
}

int
BSAccountEntryListCtrl::do_get_comment_col_num() const
{
	return comment_col_num();
}

int
BSAccountEntryListCtrl::do_get_num_columns() const
{
	return anon_num_columns();
}


}  // namespace gui
}  // namespace phatbooks
