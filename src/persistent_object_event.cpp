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

#include "gui/persistent_object_event.hpp"
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/id.hpp>
#include <wx/event.h>
#include <wx/window.h>
#include <vector>

using boost::optional;
using jewel::value;
using sqloxx::Id;
using std::vector;


namespace dcm
{
namespace gui
{

DEFINE_EVENT_TYPE(DCM_ACCOUNT_CREATING_EVENT)
DEFINE_EVENT_TYPE(DCM_ACCOUNT_EDITING_EVENT)
DEFINE_EVENT_TYPE(DCM_ACCOUNT_CREATED_EVENT)
DEFINE_EVENT_TYPE(DCM_ACCOUNT_EDITED_EVENT)
DEFINE_EVENT_TYPE(DCM_ACCOUNT_DELETED_EVENT)
DEFINE_EVENT_TYPE(DCM_JOURNAL_CREATING_EVENT)
DEFINE_EVENT_TYPE(DCM_JOURNAL_EDITING_EVENT)
DEFINE_EVENT_TYPE(DCM_JOURNAL_CREATED_EVENT)
DEFINE_EVENT_TYPE(DCM_JOURNAL_EDITED_EVENT)
DEFINE_EVENT_TYPE(DCM_DRAFT_JOURNAL_DELETED_EVENT)
DEFINE_EVENT_TYPE(DCM_ORDINARY_JOURNAL_DELETED_EVENT)
DEFINE_EVENT_TYPE(DCM_DRAFT_ENTRY_DELETED_EVENT)
DEFINE_EVENT_TYPE(DCM_ORDINARY_ENTRY_DELETED_EVENT)
DEFINE_EVENT_TYPE(DCM_BUDGET_EDITED_EVENT)
DEFINE_EVENT_TYPE(DCM_RECONCILIATION_STATUS_EVENT)

IMPLEMENT_DYNAMIC_CLASS(PersistentObjectEvent, wxCommandEvent)

PersistentObjectEvent::PersistentObjectEvent
(   wxEventType p_event_type,
    int p_event_id,
    Id p_po_id
):
    wxCommandEvent(p_event_type, p_event_id),
    m_maybe_po_id(p_po_id)
{
}

PersistentObjectEvent::PersistentObjectEvent
(   wxEventType p_event_type,
    int p_event_id
):
    wxCommandEvent(p_event_type, p_event_id)
{
    JEWEL_ASSERT (!m_maybe_po_id);
}

PersistentObjectEvent::PersistentObjectEvent
(   PersistentObjectEvent const& rhs
):
    wxCommandEvent(rhs.GetEventType(), rhs.GetId()),
    m_maybe_po_id(rhs.m_maybe_po_id)
{
}

PersistentObjectEvent::~PersistentObjectEvent()
{
}

Id
PersistentObjectEvent::po_id() const
{
    JEWEL_LOG_TRACE();
    return value(m_maybe_po_id);
}

wxEvent*
PersistentObjectEvent::Clone()
{
    return new PersistentObjectEvent(*this);
}

void
PersistentObjectEvent::fire
(   wxWindow* p_originator,
    wxEventType p_event_type
)
{
    PersistentObjectEvent event(p_event_type, wxID_ANY);
    event.SetEventObject(p_originator);
    p_originator->GetEventHandler()->ProcessEvent(event);
    return;
}

void
PersistentObjectEvent::fire
(   wxWindow* p_originator,
    wxEventType p_event_type,
    Id p_po_id
)
{
    PersistentObjectEvent event(p_event_type, wxID_ANY, p_po_id);
    event.SetEventObject(p_originator);
    p_originator->GetEventHandler()->ProcessEvent(event);
    return;
}

void
PersistentObjectEvent::notify_doomed_draft_entries
(   wxWindow* p_originator,
    vector<Id> const& p_doomed_ids
)
{
    notify_many
    (   p_originator,
        DCM_DRAFT_ENTRY_DELETED_EVENT,
        p_doomed_ids
    );
    return;
}

void
PersistentObjectEvent::notify_doomed_ordinary_entries
(   wxWindow* p_originator,
    vector<Id> const& p_doomed_ids
)
{
    notify_many
    (   p_originator,
        DCM_ORDINARY_ENTRY_DELETED_EVENT,
        p_doomed_ids
    );
    return;
}

void
PersistentObjectEvent::notify_many
(   wxWindow* p_originator,
    wxEventType p_event_type,
    vector<Id> const& p_po_ids
)
{
    JEWEL_ASSERT (p_originator);
    for (Id const elem: p_po_ids)
    {
        fire(p_originator, p_event_type, elem);
    }
    return;
    
}
    

}  // namespace gui
}  // namespace dcm
