#include "entry_list_panel.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
#include "account_reader.hpp"
#include "date_ctrl.hpp"
#include "entry.hpp"
#include "entry_list_ctrl.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "sizing.hpp"
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/window.h>
#include <vector>

using boost::optional;
using jewel::value;
using std::vector;

namespace phatbooks
{
namespace gui
{

namespace
{
	int top_row()
	{
		return 0;
	}

}  // end anonymous namespace

EntryListPanel::EntryListPanel
(	wxWindow* p_parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxPanel(p_parent, wxID_ANY),
	m_top_sizer(0),
	m_account_ctrl(0),
	m_min_date_selector(0),
	m_max_date_selector(0),
	m_refresh_button(0),
	m_entry_list_ctrl(0),
	m_database_connection(p_database_connection)
{
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);
	AccountReader const reader(m_database_connection);
	assert (!reader.empty());  // TODO What if this fails?
	m_account_ctrl = new AccountCtrl
	(	this,
		s_account_ctrl_id,
		*(reader.begin()),
		wxSize(large_width(), wxDefaultSize.y),
		reader.begin(),
		reader.end()
	);
	m_top_sizer->Add(m_account_ctrl, wxGBPosition(top_row(), 1));
	m_min_date_selector = new DateCtrl
	(	this,
		s_min_date_selector_id,
		wxSize(medium_width(), wxDefaultSize.y)
	);
	m_top_sizer->Add(m_min_date_selector, wxGBPosition(top_row(), 2));
	m_max_date_selector = new DateCtrl
	(	this,
		s_max_date_selector_id,
		wxSize(medium_width(), wxDefaultSize.y)
	);
	m_top_sizer->Add(m_max_date_selector, wxGBPosition(top_row(), 3));
	m_refresh_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&Refresh"),
		wxDefaultPosition,
		m_max_date_selector->GetSize()
	);
	m_refresh_button->SetDefault();
	m_top_sizer->Add(m_refresh_button, wxGBPosition(top_row(), 4));

	configure_entry_list_ctrl();

	// "Admin"
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Fit();
	Layout();
}

void
EntryListPanel::update_for_new(OrdinaryJournal const& p_journal)
{
	assert (m_entry_list_ctrl);
	m_entry_list_ctrl->update_for_new(p_journal);
	return;
}

void
EntryListPanel::update_for_amended(OrdinaryJournal const& p_journal)
{
	assert (m_entry_list_ctrl);
	m_entry_list_ctrl->update_for_amended(p_journal);
	return;
}

void
EntryListPanel::update_for_deleted(vector<Entry::Id> const& p_doomed_ids)
{
	assert (m_entry_list_ctrl);
	m_entry_list_ctrl->update_for_deleted(p_doomed_ids);
	return;
}

void
EntryListPanel::selected_entries(vector<Entry>& out)
{
	assert (m_entry_list_ctrl);
	m_entry_list_ctrl->selected_entries(out);
	return;
}

void
EntryListPanel::configure_entry_list_ctrl()
{
	optional<Account> const maybe_acct = maybe_account();

	// TODO Enable filtering by date

	if (m_entry_list_ctrl)
	{
		m_top_sizer->Detach(m_entry_list_ctrl);
		m_entry_list_ctrl->Destroy();
		m_entry_list_ctrl = 0;
	}
	if (maybe_acct)
	{
		m_entry_list_ctrl = EntryListCtrl::create_actual_ordinary_entry_list
		(	this,
			value(maybe_acct)
		);
	}
	else
	{
		m_entry_list_ctrl = EntryListCtrl::create_actual_ordinary_entry_list
		(	this,
			m_database_connection
		);
	}
	m_top_sizer->Add(m_entry_list_ctrl, wxGBPosition(top_row() + 1, 1));
	return;
}

optional<Account>
EntryListPanel::maybe_account() const
{
	optional<Account> ret;
	// TODO We need a mechanism for returning an unitialized optional - which
	// would signify that we will show Entries of any Account.
	assert (m_account_ctrl);
	ret = m_account_ctrl->account();
	return ret;
}

}  // namespace gui
}  // namespace phatbooks
