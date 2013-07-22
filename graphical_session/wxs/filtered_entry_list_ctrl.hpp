#ifndef GUARD_filtered_entry_list_ctrl_hpp
#define GUARD_filtered_entry_list_ctrl_hpp

#include "account.hpp"
#include "entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/window.h>

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
 * @todo Need to override do_update_for_amended(Account const& p_account).
 */
class FilteredEntryListCtrl: public EntryListCtrl
{
public:
	FilteredEntryListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		Account const& p_account,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date,
		boost::optional<boost::gregorian::date> const& p_maybe_max_date
	);

	virtual ~FilteredEntryListCtrl();

private:
	virtual bool do_require_progress_log() const;
	virtual void do_insert_non_date_columns() = 0;
	virtual bool do_approve_entry(Entry const& p_entry) const;
	virtual void do_set_column_widths();
	virtual int do_get_num_columns() const = 0;
	virtual int do_get_comment_col_num() const = 0;
	
	Account const m_account;
	boost::gregorian::date const m_min_date;
	boost::optional<boost::gregorian::date> const m_maybe_max_date;

};  // class FilteredEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_filtered_entry_list_ctrl_hpp