#ifndef GUARD_account_entry_list_ctrl_hpp
#define GUARD_account_entry_list_ctrl_hpp

#include "entry_list_ctrl.hpp"

namespace phatbooks
{
namespace gui
{

/**
 * An EntryListCtrl which is filtered by Account (and may also be
 * filtered by date).
 */
class AccountEntryListCtrl: public EntryListCtrl
{
public:
	virtual ~AccountEntryListCtrl();

private:

};  // class AccountEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_entry_list_ctrl_hpp
