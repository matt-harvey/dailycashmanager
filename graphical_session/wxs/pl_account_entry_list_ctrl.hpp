#ifndef GUARD_pl_account_entry_list_ctrl_hpp
#define GUARD_pl_account_entry_list_ctrl_hpp

#include "account_entry_list_ctrl.hpp"

namespace phatbooks
{
namespace gui
{

/**
 * An AccountEntryListCtrl where the Account is of account_super_type::pl.
 */
class PLAccountEntryListCtrl: public AccountEntryListCtrl
{
public:
	virtual ~PLAccountEntryListCtrl();

private:

};  // class PLAccountEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_pl_account_entry_list_ctrl_hpp
