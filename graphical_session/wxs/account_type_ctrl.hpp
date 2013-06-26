#ifndef GUARD_account_type_ctrl_hpp
#define GUARD_account_type_ctrl_hpp

#include "account_type.hpp"
#include <boost/noncopyable.hpp>
#include <wx/combobox.h>

namespace phatbooks
{
namespace gui
{

/**
 * Widget from which user can select an AccountType.
 */
class AccountTypeCtrl: public wxComboBox, private boost::noncopyable
{
public:


private:

};  // class AccountTypeCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_type_ctrl_hpp
