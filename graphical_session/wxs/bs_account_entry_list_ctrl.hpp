#ifndef GUARD_bs_account_entry_list_ctrl_hpp
#define GUARD_bs_account_entry_list_ctrl_hpp

#include "account_entry_list_ctrl.hpp"

namespace phatbooks
{
namespace gui
{

/**
 * An AccountEntryListCtrl where the Account is of
 * account_super_type::balance_sheet.
 */
class BSAccountEntryListCtrl: public AccountEntryListCtrl
{
public:
	virtual ~BSAccountEntryListCtrl();

private:

};  // class BSAccountEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_bs_account_entry_list_ctrl_hpp
