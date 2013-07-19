#include "entry_list_panel.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
#include "account_reader.hpp"
#include "date.hpp"
#include "date_ctrl.hpp"
#include "entry.hpp"
#include "entry_list_ctrl.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "sizing.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/window.h>
#include <vector>

using boost::optional;
using jewel::value;
using std::vector;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(EntryListPanel, wxPanel)
	EVT_BUTTON(s_refresh_button_id, EntryListPanel::on_refresh_button_click)
END_EVENT_TABLE()


EntryListPanel::EntryListPanel
(	wxWindow* p_parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxPanel(p_parent, wxID_ANY),
	m_next_row(0),
	m_top_sizer(0),
	m_account_ctrl(0),
	m_min_date_ctrl(0),
	m_max_date_ctrl(0),
	m_refresh_button(0),
	m_entry_list_ctrl(0),
	m_database_connection(p_database_connection)
{
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);

	++m_next_row;  // To leave some space at top.

	wxStaticText* account_label =
		new wxStaticText(this, wxID_ANY, wxString(" Account or category:"));
	m_top_sizer->Add(account_label, wxGBPosition(m_next_row, 1));
	wxStaticText* min_date_label =
		new wxStaticText(this, wxID_ANY, wxString(" From:"));
	m_top_sizer->Add(min_date_label, wxGBPosition(m_next_row, 2));
	wxStaticText* max_date_label =
		new wxStaticText(this, wxID_ANY, wxString(" To:"));
	m_top_sizer->Add(max_date_label, wxGBPosition(m_next_row, 3));

	++m_next_row;

	ImpureAccountReader const reader(m_database_connection);
	assert (!reader.empty());  // TODO What if this fails?
	m_account_ctrl = new AccountCtrl
	(	this,
		s_account_ctrl_id,
		*(reader.begin()),
		wxSize(large_width(), wxDefaultSize.y),
		reader.begin(),
		reader.end()
	);
	int const std_height = m_account_ctrl->GetSize().GetHeight();
	m_top_sizer->Add(m_account_ctrl, wxGBPosition(m_next_row, 1));
	m_min_date_ctrl = new DateCtrl
	(	this,
		s_min_date_ctrl_id,
		wxSize(medium_width(), std_height),
		today(),
		true
	);
	m_top_sizer->Add(m_min_date_ctrl, wxGBPosition(m_next_row, 2));
	m_max_date_ctrl = new DateCtrl
	(	this,
		s_max_date_ctrl_id,
		wxSize(medium_width(), std_height),
		today(),
		true
	);
	m_top_sizer->Add(m_max_date_ctrl, wxGBPosition(m_next_row, 3));
	m_refresh_button = new wxButton
	(	this,
		s_refresh_button_id,
		wxString("&Refresh"),
		wxDefaultPosition,
		m_max_date_ctrl->GetSize()
	);
	m_refresh_button->SetDefault();
	m_top_sizer->Add(m_refresh_button, wxGBPosition(m_next_row, 4));

	++m_next_row;

	configure_entry_list_ctrl();

	// "Admin"
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Fit();
	Layout();
}

void
EntryListPanel::on_refresh_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	configure_entry_list_ctrl();
	return;
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
	if (m_entry_list_ctrl)
	{
		m_top_sizer->Detach(m_entry_list_ctrl);
		m_entry_list_ctrl->Destroy();
		m_entry_list_ctrl = 0;
		--m_next_row;
	}
	m_entry_list_ctrl = EntryListCtrl::create_actual_ordinary_entry_list
	(	this,
		wxSize
		(	large_width() + medium_width() * 3 + standard_gap() * 3,
			wxDefaultSize.y
		),
		selected_account(),
		selected_min_date(),
		selected_max_date()
	);
	m_top_sizer->Add
	(	m_entry_list_ctrl,
		wxGBPosition(m_next_row, 1),
		wxGBSpan(1, 4)
	);
	++m_next_row;
	Fit();
	Layout();
	return;
}

Account
EntryListPanel::selected_account() const
{
	return m_account_ctrl->account();
}

optional<gregorian::date>
EntryListPanel::selected_min_date() const
{
	return m_min_date_ctrl->date();
}

optional<gregorian::date>
EntryListPanel::selected_max_date() const
{
	return m_max_date_ctrl->date();
}
	


}  // namespace gui
}  // namespace phatbooks
