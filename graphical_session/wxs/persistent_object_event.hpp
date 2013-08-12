// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_persistent_object_event_hpp
#define GUARD_persistent_object_event_hpp

#include "phatbooks_persistent_object.hpp"
#include <boost/optional.hpp>
#include <wx/event.h>
#include <wx/window.h>

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
	(	wxEventType p_event_type,
		int p_event_id,
		Id p_po_id
	);

	/**
	 * This constructor should be used in case the p_po_id is
	 * irrelevant for the particular wxEventType passed to
	 * \e p_event_type.
	 */
	PersistentObjectEvent
	(	wxEventType p_event_type = wxEVT_NULL,
		int p_event_id = 0
	);

	PersistentObjectEvent(PersistentObjectEvent const& rhs);

	/**
	 * @returns the id of the PhatbooksPersistentObject with which
	 * this event is associated.
	 * 
	 * @throws jewel::UninitializedOptionalException in case there
	 * is no such id associated with this event.
	 */
	Id po_id() const;

    /**
	 * Required for sending with wxPostEvent().
	 */
    wxEvent* Clone();

	/**
	 * Convenience function to fire a PersistentObjectEvent with which
	 * no existing PhatbooksPersistentObject is associated.
	 */
	static void fire
	(	wxWindow* p_originator,
		wxEventType p_event_type
	);

	/**
	 * Convenience function to fire a PersistentObjectEvent with which
	 * an existing PhatbooksPersistentObject is associated.
	 *
	 * Precondition: \e p_object must have an id.
	 */
	static void fire
	(	wxWindow* p_originator,
		wxEventType p_event_type,
		PhatbooksPersistentObjectBase& p_object
	);

	/**
	 * Convenience function to fire a PersistentObjectEvent with which
	 * an existing-or-just-deleted PhatbooksPersistentObject is associated.
	 * Here we pass the object's id() to the third parameter, rather than
	 * the object itself.
	 */
	static void fire
	(	wxWindow* p_originator,
		wxEventType p_event_type,
		int p_po_id
	);

	/**
	 * Convenience function to fire one or more PersistentObjectEvents
	 * notifying that Entries that had ids \e p_doomed_ids, are now deleted.
	 * Use for Entries that were associated with a a DraftJournal.
	 */
	static void notify_doomed_draft_entries
	(	wxWindow* p_originator,
		std::vector<Id> const& p_doomed_ids
	);
	
	/**
	 * Convenience function to fire on or more PersistentObjectEvents
	 * notifying that Entries that has ids \e p_doomed_ids, are now
	 * deleted. Use for Entries that were associated with an OrdinaryJournal.
	 */
	static void notify_doomed_ordinary_entries
	(	wxWindow* p_originator,
		std::vector<Id> const& p_doomed_ids
	);

private:

	static void notify_many
	(	wxWindow* p_originator,
		wxEventType p_event_type,
		std::vector<Id> const& p_po_ids
	);

	boost::optional<Id> m_maybe_po_id;	

	DECLARE_DYNAMIC_CLASS(PersistentObjectEvent)
};


/**
 * Event types associated with PersistentObjectEvent.
 */
BEGIN_DECLARE_EVENT_TYPES()

	/**
	 * Fire to signify that we want the user to be given the opportunity
	 * to create an Account.
	 *
	 * Note when creating or firing this type of event, the \e p_po_id
	 * parameter is irrelevant.
	 */
	DECLARE_EVENT_TYPE(PHATBOOKS_ACCOUNT_CREATING_EVENT, -1)

	/**
	 * Fire to signify that we want the user to be given the opportunity
	 * to edit an Account.
	 *
	 * Note when creating or firing this type of event, the \e p_po_id
	 * parameter is irrelevant.
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

	/**
	 * The following are analogous to "_ACCOUNT" event types, but for
	 * PersistentJournals.
	 */
	DECLARE_EVENT_TYPE(PHATBOOKS_JOURNAL_CREATING_EVENT, -1)
	DECLARE_EVENT_TYPE(PHATBOOKS_JOURNAL_EDITING_EVENT, -1)
	DECLARE_EVENT_TYPE(PHATBOOKS_JOURNAL_CREATED_EVENT, -1)
	DECLARE_EVENT_TYPE(PHATBOOKS_JOURNAL_EDITED_EVENT, -1)
	DECLARE_EVENT_TYPE(PHATBOOKS_DRAFT_JOURNAL_DELETED_EVENT, -1)
	DECLARE_EVENT_TYPE(PHATBOOKS_ORDINARY_JOURNAL_DELETED_EVENT, -1)

	/**
	 * And for Entries.
	 */
	DECLARE_EVENT_TYPE(PHATBOOKS_DRAFT_ENTRY_DELETED_EVENT, -1)
	DECLARE_EVENT_TYPE(PHATBOOKS_ORDINARY_ENTRY_DELETED_EVENT, -1)

	/**
	 * To notify of a change in the BudgetItems associated
	 * with a given Account. Note that for this event, it is
	 * the id() of the \e Account that is relevant (not the id() of
	 * the BudgetItems (if any)).
	 */
	DECLARE_EVENT_TYPE(PHATBOOKS_BUDGET_EDITED_EVENT, -1)

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

#define PHATBOOKS_EVT_DRAFT_JOURNAL_DELETED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_DRAFT_JOURNAL_DELETED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_ORDINARY_JOURNAL_DELETED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_ORDINARY_JOURNAL_DELETED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_DRAFT_ENTRY_DELETED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_DRAFT_ENTRY_DELETED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_ORDINARY_ENTRY_DELETED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_ORDINARY_ENTRY_DELETED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),

#define PHATBOOKS_EVT_BUDGET_EDITED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( PHATBOOKS_BUDGET_EDITED_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) \
	(wxNotifyEventFunction) \
	wxStaticCastEvent(PersistentObjectEventFunction, &fn), (wxObject*) NULL),





}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_persistent_object_event_hpp
