// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "draft_journal_list_ctrl.hpp"
#include "b_string.hpp"
#include "date.hpp"
#include "draft_journal.hpp"
#include "draft_journal_reader.hpp"
#include "frequency.hpp"
#include "persistent_object_event.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include <wx/event.h>
#include <wx/listctrl.h>
#include <algorithm>
#include <string>
#include <vector>

using std::max;
using std::string;
using std::vector;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(DraftJournalListCtrl, wxListCtrl)
	EVT_LIST_ITEM_ACTIVATED
	(	wxID_ANY,
		DraftJournalListCtrl::on_item_activated
	)
END_EVENT_TABLE()

DraftJournalListCtrl::DraftJournalListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	UserDraftJournalReader const& p_reader,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxListCtrl
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		p_size,
		wxLC_REPORT | wxLC_SINGLE_SEL | wxFULL_REPAINT_ON_RESIZE
	),
	m_database_connection(p_database_connection)
{
	update(p_reader);
}

void
DraftJournalListCtrl::selected_draft_journals(vector<DraftJournal>& out)
{
	size_t i = 0;
	size_t const lim = GetItemCount();
	for ( ; i != lim; ++i)
	{
		if (GetItemState(i, wxLIST_STATE_SELECTED))
		{
			DraftJournal const dj(m_database_connection, GetItemData(i));
			out.push_back(dj);
		}
	}
	return;
}

void
DraftJournalListCtrl::on_item_activated(wxListEvent& event)
{
	DraftJournal journal
	(	m_database_connection,
		GetItemData(event.GetIndex())
	);
	
	// Fire a PersistentJournal editing request. This will be handled
	// higher up the window hierarchy.
	PersistentObjectEvent::fire
	(	this,
		PHATBOOKS_JOURNAL_EDITING_EVENT,
		journal
	);
	return;
}

void
DraftJournalListCtrl::update(UserDraftJournalReader const& p_reader)
{
	// Remember which rows are selected currently
	vector<size_t> selected_rows;
	size_t const lim = GetItemCount();
	for (size_t j = 0; j != lim; ++j)
	{
		if (GetItemState(j, wxLIST_STATE_SELECTED))
		{
			selected_rows.push_back(j);
		}
	}

	// Now (re)draw
	ClearAll();
	InsertColumn(s_name_col, "Recurring transaction", wxLIST_FORMAT_LEFT);
	InsertColumn(s_frequency_col, "Frequency", wxLIST_FORMAT_LEFT);
	InsertColumn(s_next_date_col, "Next date", wxLIST_FORMAT_RIGHT);
	
	UserDraftJournalReader::size_type i = 0;	
	UserDraftJournalReader::const_iterator it = p_reader.begin();
	UserDraftJournalReader::const_iterator const end = p_reader.end();
	for ( ; it != end; ++it, ++i)
	{
		// Insert item, with string for Column 0
		InsertItem(i, bstring_to_wx(it->name()));
		
		// The item may change position due to e.g. sorting, so store the
		// Journal ID in the item's data
		// TODO Do a static assert to ensure second param will fit the id.
		assert (it->has_id());
		SetItemData(i, it->id());

		// Set the frequency and next-date columns.

		// TODO Deal properly with the case where the
		// DraftJournal has a number of Repeaters that is not 1? Or are we
		// just going to rule that out by making it impossible for the user
		// to create such DraftJournals?

		vector<Repeater> const& repeaters = it->repeaters();

		if (repeaters.empty())
		{
			SetItem(i, s_frequency_col, wxEmptyString);
			SetItem(i, s_next_date_col, wxEmptyString);
		}
		else
		{
			wxString frequency_description("Multiple cycles");
			wxString next_date_string("Multiple cycles");
			assert (repeaters.size() >= 1);
			if (repeaters.size() == 1)
			{
				frequency_description = std8_to_wx
				(	phatbooks::frequency_description
					(	repeaters[0].frequency(),
						string("every")
					)
				);
				next_date_string =
					date_format_wx(repeaters[0].next_date());
			}
			SetItem(i, s_frequency_col, frequency_description);
			SetItem(i, s_next_date_col, next_date_string);
		}
	}

	// Reinstate the selections we remembered
	size_t const sel_sz = selected_rows.size();
	for (size_t k = 0; k != sel_sz; ++k)
	{
		SetItemState
		(	selected_rows[k],
			wxLIST_STATE_SELECTED,
			wxLIST_STATE_SELECTED
		);
	}

	// Configure column widths
	SetColumnWidth(s_name_col, wxLIST_AUTOSIZE_USEHEADER);
	SetColumnWidth(s_name_col, max(GetColumnWidth(s_name_col), 400));
	SetColumnWidth(s_frequency_col, wxLIST_AUTOSIZE_USEHEADER);
	SetColumnWidth(s_next_date_col, wxLIST_AUTOSIZE);

	Layout();

	return;
}



}  // namespace gui
}  // namespace phatbooks
