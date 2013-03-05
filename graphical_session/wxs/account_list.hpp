#ifndef GUARD_account_list_hpp
#define GUARD_account_list_hpp

#include <boost/noncopyable.hpp>
#include <wx/listctrl.h>

namespace phatbooks
{

class PhatbooksDatabaseConnection;


namespace gui
{


typedef wxListCtrl AccountList;

/**
 * @returns a pointer to a heap-allocation AccountList.
 * The client does not need to take care of the memory - the memory
 * is taken care of by the parent class.
 */
AccountList* create_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
);
	



}  // namespace gui
}  // namespace phatbooks



#endif  // GUARD_account_list_hpp
