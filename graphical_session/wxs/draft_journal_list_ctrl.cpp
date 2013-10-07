// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "draft_journal_list_ctrl.hpp"
#include "date.hpp"
#include "draft_journal.hpp"
#include "draft_journal_table_iterator.hpp"
#include "frequency.hpp"
#include "persistent_object_event.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater_handle.hpp"
#include "string_conv.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <algorithm>
#include <string>
#include <vector>

using jewel::Log;
using sqloxx::Handle;
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
	DraftJournalTableIterator p_beg,
	DraftJournalTableIterator p_end,
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
	JEWEL_LOG_TRACE();
	update(p_beg, p_end);
	JEWEL_LOG_TRACE();
}

void
DraftJournalListCtrl::selected_draft_journals
(	vector<Handle<DraftJournal> >& out
)
{
	size_t i = 0;
	size_t const lim = GetItemCount();
	for ( ; i != lim; ++i)
	{
		if (GetItemState(i, wxLIST_STATE_SELECTED))
		{
			Handle<DraftJournal> const dj
			(	m_database_connection,
				GetItemData(i)
			);
			out.push_back(dj);
		}
	}
	return;
}

void
DraftJournalListCtrl::on_item_activated(wxListEvent& event)
{
	JEWEL_LOG_TRACE();

	// Fire a PersistentJournal editing request. This will be handled
	// higher up the window hierarchy.
	sqloxx::Id const journal_id = GetItemData(event.GetIndex());
	JEWEL_LOG_VALUE(Log::info, journal_id);
	PersistentObjectEvent::fire
	(	this,
		PHATBOOKS_JOURNAL_EDITING_EVENT,
		journal_id
	);
	return;
}

void
DraftJournalListCtrl::update
(	DraftJournalTableIterator p_beg,
	DraftJournalTableIterator p_end
)
{
	JEWEL_LOG_TRACE();
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
	JEWEL_LOG_TRACE();

	// Now (re)draw
	ClearAll();
	InsertColumn(s_name_col, "Recurring transaction", wxLIST_FORMAT_LEFT);
	InsertColumn(s_frequency_col, "Frequency", wxLIST_FORMAT_LEFT);
	InsertColumn(s_next_date_col, "Next date", wxLIST_FORMAT_RIGHT);
	
	size_t i = 0;	
	for ( ; p_beg != p_end; ++p_beg, ++i)
	{
		Handle<DraftJournal> const& dj = *p_beg;

		// Insert item, with string for Column 0
		InsertItem(i, dj->name());
		
		// The item may change position due to e.g. sorting, so store the
		// Journal ID in the item's data
		// TODO Do a static assert to ensure second param will fit the id.
		JEWEL_ASSERT (dj->has_id());
		SetItemData(i, dj->id());

		// Set the frequency and next-date columns.

		// TODO Deal properly with the case where the
		// DraftJournal has a number of Repeaters that is not 1? Or are we
		// just going to rule that out by making it impossible for the user
		// to create such DraftJournals?

		vector<RepeaterHandle> const& repeaters = dj->repeaters();

		if (repeaters.empty())
		{
			SetItem(i, s_frequency_col, wxEmptyString);
			SetItem(i, s_next_date_col, wxEmptyString);
		}
		else
		{
			wxString frequency_description("Multiple cycles");
			wxString next_date_string("Multiple cycles");
			JEWEL_ASSERT (repeaters.size() >= 1);
			if (repeaters.size() == 1)
			{
				frequency_description = std8_to_wx
				(	phatbooks::frequency_description
					(	repeaters[0]->frequency(),
						string("every")
					)
				);
				next_date_string =
					date_format_wx(repeaters[0]->next_date());
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

	JEWEL_LOG_TRACE();
	return;
}



}  // namespace gui
}  // namespace phatbooks
