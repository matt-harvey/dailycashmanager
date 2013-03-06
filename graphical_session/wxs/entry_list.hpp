#ifndef GUARD_entry_list_hpp
#define GUARD_entry_list_hpp


#include <wx/listctrl.h>

namespace phatbooks
{

class PhatbooksDatabaseConnection;

namespace gui
{

typedef wxListCtrl EntryList;

/**
 * @returns a pointer to a heap-allocated EntryList, listing
 * all and only the \e actual (non-budget) OrdinaryEntry's stored in \e dbc.
 * The client does not need to take care of the memory - the memory
 * is taken care of by the parent window.
 */
EntryList* create_actual_ordinary_entry_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
);



}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_list_hpp
