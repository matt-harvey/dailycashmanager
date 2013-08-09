#include "persistent_object_event.hpp"
#include "phatbooks_persistent_object.hpp"
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/event.h>
#include <wx/window.h>
#include <vector>

using boost::optional;
using jewel::value;
using std::vector;

// for debugging
	#include <jewel/debug_log.hpp>
	#include <iostream>
	using std::endl;

namespace phatbooks
{
namespace gui
{

DEFINE_EVENT_TYPE(PHATBOOKS_ACCOUNT_CREATING_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_ACCOUNT_EDITING_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_ACCOUNT_CREATED_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_ACCOUNT_EDITED_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_ACCOUNT_DELETED_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_JOURNAL_CREATING_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_JOURNAL_EDITING_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_JOURNAL_CREATED_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_JOURNAL_EDITED_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_DRAFT_JOURNAL_DELETED_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_ORDINARY_JOURNAL_DELETED_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_DRAFT_ENTRY_DELETED_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_ORDINARY_ENTRY_DELETED_EVENT)
DEFINE_EVENT_TYPE(PHATBOOKS_BUDGET_EDITED_EVENT)

IMPLEMENT_DYNAMIC_CLASS(PersistentObjectEvent, wxCommandEvent)

PersistentObjectEvent::PersistentObjectEvent
(	wxEventType p_event_type,
	int p_event_id,
	Id p_po_id
):
	wxCommandEvent(p_event_type, p_event_id),
	m_maybe_po_id(p_po_id)
{
}

PersistentObjectEvent::PersistentObjectEvent
(	wxEventType p_event_type,
	int p_event_id
):
	wxCommandEvent(p_event_type, p_event_id)
{
	assert (!m_maybe_po_id);
}

PersistentObjectEvent::PersistentObjectEvent
(	PersistentObjectEvent const& rhs
):
	wxCommandEvent(rhs.GetEventType(), rhs.GetId()),
	m_maybe_po_id(rhs.m_maybe_po_id)
{
}

PersistentObjectEvent::Id
PersistentObjectEvent::po_id() const
{
	return value(m_maybe_po_id);
}

wxEvent*
PersistentObjectEvent::Clone()
{
	return new PersistentObjectEvent(*this);
}

void
PersistentObjectEvent::fire
(	wxWindow* p_originator,
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
(	wxWindow* p_originator,
	wxEventType p_event_type,
	PhatbooksPersistentObjectBase& p_object
)
{
	assert (p_object.has_id());  // precondition
	fire(p_originator, p_event_type, p_object.id());
	return;
}

void
PersistentObjectEvent::fire
(	wxWindow* p_originator,
	wxEventType p_event_type,
	PersistentObjectEvent::Id p_po_id
)
{
	PersistentObjectEvent event(p_event_type, wxID_ANY, p_po_id);
	event.SetEventObject(p_originator);
	p_originator->GetEventHandler()->ProcessEvent(event);
	return;
}

void
PersistentObjectEvent::notify_doomed_draft_entries
(	wxWindow* p_originator,
	vector<Id> const& p_doomed_ids
)
{
	notify_many
	(	p_originator,
		PHATBOOKS_DRAFT_ENTRY_DELETED_EVENT,
		p_doomed_ids
	);
	return;
}

void
PersistentObjectEvent::notify_doomed_ordinary_entries
(	wxWindow* p_originator,
	vector<Id> const& p_doomed_ids
)
{
	notify_many
	(	p_originator,
		PHATBOOKS_ORDINARY_ENTRY_DELETED_EVENT,
		p_doomed_ids
	);
	return;
}

void
PersistentObjectEvent::notify_many
(	wxWindow* p_originator,
	wxEventType p_event_type,
	vector<PersistentObjectEvent::Id> const& p_po_ids
)
{
	assert (p_originator);
	vector<Id>::const_iterator it = p_po_ids.begin();
	vector<Id>::const_iterator const end = p_po_ids.end();
	for ( ; it != end; ++it) fire(p_originator, p_event_type, *it);
	return;
	
}
	

}  // namespace gui
}  // namespace phatbooks
