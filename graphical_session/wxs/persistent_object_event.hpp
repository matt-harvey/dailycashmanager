#ifndef GUARD_persistent_object_event_hpp
#define GUARD_persistent_object_event_hpp

#include "phatbooks_persistent_object.hpp"
#include <boost/optional.hpp>
#include <wx/event.h>

namespace phatbooks
{
namespace gui
{

class PersistentObjectEvent: public wxCommandEvent
{
public:
	typedef PhatbooksPersistentObjectBase::Id Id;

	/**
	 * @param p_event_id is the wxWidgets event id.
	 *
	 * @param p_maybe_po_id may contain the id() of the
	 * PhatbooksPersistentObject to which this event pertains.
	 */
	PersistentObjectEvent
	(	wxEventType p_event_type = wxEVT_NULL,
		int p_event_id = 0,
		boost::optional<Id> p_maybe_po_id = boost::optional<Id>()
	);

	PersistentObjectEvent(PersistentObjectEvent const& rhs);

	boost::optional<Id> maybe_po_id() const;

    /**
	 * Required for sending with wxPostEvent().
	 */
    wxEvent* Clone();

private:
	boost::optional<Id> m_maybe_po_id;	

	DECLARE_DYNAMIC_CLASS(PersistentObjectEvent)
};


BEGIN_DECLARE_EVENT_TYPES()

	/**
	 * Fire to signify that we want the user to be given the opportunity
	 * to create an Account.
	 */
	DECLARE_EVENT_TYPE(PHATBOOKS_ACCOUNT_CREATING_EVENT, -1)

	/**
	 * Fire to signify that we want the user to be given the opportunity
	 * to edit an Account.
	 */
	DECLARE_EVENT_TYPE(PHATBOOKS_ACCOUNT_EDITING_EVENT, -1)

	/**
	 * Fire to signifiy that the user has just created (and saved) an
	 * Account.
	 */
	DECLARE_EVENT_TYPE(PHATBOOKS_ACCOUNT_CREATED_EVENT, -1)

	/**
	 * Fire to signify that the user has just edited an Account
	 * (and saved the changes).
	 */
	DECLARE_EVENT_TYPE(PHATBOOKS_ACCOUNT_EDITED_EVENT, -1)

	/**
	 * Fire to indicated that the user has just deleted an Account.
	 */
	DECLARE_EVENT_TYPE(PHATBOOKS_ACCOUNT_DELETED_EVENT, -1)

	// Analogous to "_ACCOUNT" event types, but for
	// PersistentJournals.

	DECLARE_EVENT_TYPE(PHATBOOKS_JOURNAL_CREATING_EVENT, -1)
	DECLARE_EVENT_TYPE(PHATBOOKS_JOURNAL_EDITING_EVENT, -1)
	DECLARE_EVENT_TYPE(PHATBOOKS_JOURNAL_CREATED_EVENT, -1)
	DECLARE_EVENT_TYPE(PHATBOOKS_JOURNAL_EDITED_EVENT, -1)
	DECLARE_EVENT_TYPE(PHATBOOKS_JOURNAL_DELETED_EVENT, -1)

END_DECLARE_EVENT_TYPES()



// Various boilerplate called for by the wxWidgets event system.

typedef
	void (wxEvtHandler::*PersistentObjectEventFunction)
	(PersistentObjectEvent&);

#define PHATBOOKS_EVT_ACCOUNT_CREATING(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_ACCOUNT_CREATING_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_ACCOUNT_EDITING(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_ACCOUNT_EDITING_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_ACCOUNT_CREATED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_ACCOUNT_CREATED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_ACCOUNT_EDITED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_ACCOUNT_EDITED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_ACCOUNT_DELETED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_ACCOUNT_DELETED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_JOURNAL_CREATING(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_JOURNAL_CREATING_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_JOURNAL_EDITING(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_JOURNAL_EDITING_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_JOURNAL_CREATED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_JOURNAL_CREATED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_JOURNAL_EDITED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_JOURNAL_EDITED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_JOURNAL_DELETED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_JOURNAL_DELETED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),





}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_persistent_object_event_hpp
