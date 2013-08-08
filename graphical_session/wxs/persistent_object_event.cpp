#include "persistent_object_event.hpp"
#include "phatbooks_persistent_object.hpp"
#include <boost/optional.hpp>
#include <wx/event.h>

namespace phatbooks
{
namespace gui
{

DEFINE_EVENT_TYPE(AccountCreationEvent)
DEFINE_EVENT_TYPE(AccountUpdateEvent)
DEFINE_EVENT_TYPE(AccountDeletionEvent)

DEFINE_EVENT_TYPE(JournalCreationEvent)
DEFINE_EVENT_TYPE(JournalUpdateEvent)
DEFINE_EVENT_TYPE(JournalDeletionEvent)

IMPLEMENT_DYNAMIC_CLASS(PersistentObjectEvent, wxCommandEvent)

PersistentObjectEvent::PersistentObjectEvent
(	wxEventType p_event_type,
	int p_event_id,
	boost::optional<Id> p_maybe_po_id
):
	wxCommandEvent(p_event_type, p_event_id),
	m_maybe_po_id(p_maybe_po_id)
{
}

PersistentObjectEvent::PersistentObjectEvent
(	PersistentObjectEvent const& rhs
):
	wxCommandEvent(rhs.GetEventType(), rhs.GetId()),
	m_maybe_po_id(rhs.m_maybe_po_id)
{
}

wxEvent*
PersistentObjectEvent::Clone()
{
	return new PersistentObjectEvent(*this);
}

}  // namespace gui
}  // namespace phatbooks
