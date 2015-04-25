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

#include "gui/draft_journal_list_ctrl.hpp"
#include "date.hpp"
#include "draft_journal.hpp"
#include "draft_journal_table_iterator.hpp"
#include "frequency.hpp"
#include "dcm_database_connection.hpp"
#include "repeater.hpp"
#include "string_conv.hpp"
#include "gui/persistent_object_event.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/on_windows.hpp>
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <algorithm>
#include <string>
#include <type_traits>
#include <vector>

using jewel::Log;
using sqloxx::Handle;
using std::is_signed;
using std::max;
using std::string;
using std::vector;

namespace dcm
{
namespace gui
{

BEGIN_EVENT_TABLE(DraftJournalListCtrl, wxListCtrl)
    EVT_LIST_ITEM_ACTIVATED
    (   wxID_ANY,
        DraftJournalListCtrl::on_item_activated
    )
END_EVENT_TABLE()

DraftJournalListCtrl::DraftJournalListCtrl
(   wxWindow* p_parent,
    wxSize const& p_size,
    DraftJournalTableIterator p_beg,
    DraftJournalTableIterator p_end,
    DcmDatabaseConnection& p_database_connection
):
    wxListCtrl
    (   p_parent,
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

vector<Handle<DraftJournal> >
DraftJournalListCtrl::selected_draft_journals()
{
    vector<Handle<DraftJournal> > ret;
    size_t i = 0;
    size_t const lim = GetItemCount();
    for ( ; i != lim; ++i)
    {
        if (GetItemState(i, wxLIST_STATE_SELECTED))
        {
            Handle<DraftJournal> const dj
            (   m_database_connection,
                GetItemData(i)
            );
            ret.push_back(dj);
        }
    }
    return ret;
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
    (   this,
        DCM_JOURNAL_EDITING_EVENT,
        journal_id
    );
    return;
}

void
DraftJournalListCtrl::update
(   DraftJournalTableIterator p_beg,
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
    
    long i = 0;  // because wxWidgets uses long
    for ( ; p_beg != p_end; ++p_beg, ++i)
    {
        Handle<DraftJournal> const& dj = *p_beg;

        // Insert item, with string for Column 0
        InsertItem(i, dj->name());
        
        // The item may change position due to e.g. sorting, so store the
        // Journal ID in the item's data
        JEWEL_ASSERT (dj->has_id());
        static_assert
        (   sizeof(long) >= sizeof(dj->id()) &&
            is_signed<long>::value &&
            is_signed<decltype(dj->id())>::value,
            "Cannot safely fit Id into type required for SetItemData."
        );
        SetItemData(i, dj->id());

        // Set the frequency and next-date columns.

        // TODO MEDIUM PRIORITY We are assuming here a DraftJournal cannot
        // have a number of Repeaters that is not 1. For the moment this
        // assumption is true, but it might not always be so. Put
        // safeguards here against the possibility that the assumption
        // might one day cease to hold.

        vector<Handle<Repeater> > const& repeaters = dj->repeaters();

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
                (   dcm::frequency_description
                    (   repeaters[0]->frequency(),
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
        (   selected_rows[k],
            wxLIST_STATE_SELECTED,
            wxLIST_STATE_SELECTED
        );
    }

    // Configure column widths
    SetColumnWidth(s_name_col, wxLIST_AUTOSIZE);
    SetColumnWidth(s_frequency_col, wxLIST_AUTOSIZE);
    SetColumnWidth(s_next_date_col, wxLIST_AUTOSIZE);
#   ifdef JEWEL_ON_WINDOWS
        SetColumnWidth(s_name_col, std::max(GetColumnWidth(s_name_col), 150));
        SetColumnWidth
        (   s_frequency_col,
            std::max(GetColumnWidth(s_frequency_col), 100)
        );
        SetColumnWidth
        (   s_next_date_col,
            std::max(GetColumnWidth(s_next_date_col), 75)
        );
#   endif
    Layout();

    JEWEL_LOG_TRACE();
    return;
}

}  // namespace gui
}  // namespace dcm
